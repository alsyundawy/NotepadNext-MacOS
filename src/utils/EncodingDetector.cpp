/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry DS Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "EncodingDetector.h"
#include "uchardet.h"

#include <QStringDecoder>
#include <QStringEncoder>
#include <QTextCodec>
#include <QRegularExpression>
#include <QtGlobal>
#include <algorithm>

static const QByteArray BOM_UTF8_BYTES = QByteArray::fromHex("EFBBBF");
static const QByteArray BOM_UTF16LE_BYTES = QByteArray::fromHex("FFFE");
static const QByteArray BOM_UTF16BE_BYTES = QByteArray::fromHex("FEFF");

EncodingDetector::DetectionResult EncodingDetector::detect(const QByteArray &data) {
    DetectionResult result;
    if (data.isEmpty()) {
        result.encoding = QStringLiteral("UTF-8");
        result.friendlyName = QStringLiteral("UTF-8");
        result.bomType = ScintillaNext::BomType::None;
        result.bomLength = 0;
        result.confidence = 100;
        result.detectionMethod = QStringLiteral("Empty Document");
        return result;
    }

    // Stage 1: Explicit Byte Order Mark (BOM)
    if (detectBom(data, result)) {
        result.friendlyName = formatDisplayName(result.encoding, result.bomType);
        return result;
    }

    // Stage 2: Heuristic UTF-16 without BOM (interleaved NULs)
    if (detectInterleavedUtf16(data, result)) {
        result.friendlyName = formatDisplayName(result.encoding, result.bomType);
        return result;
    }

    // Stage 3: Document Header Declaration (XML, HTML, Python/Ruby coding cookies, CSS)
    if (detectHeaderDeclaration(data, result)) {
        result.friendlyName = formatDisplayName(result.encoding, result.bomType);
        return result;
    }

    // Stage 4: Strict RFC 3629 / W3C UTF-8 Validation
    if (detectStrictUtf8(data, result)) {
        result.friendlyName = formatDisplayName(result.encoding, result.bomType);
        return result;
    }

    // Stage 5: Mozilla Universal Charset Detector (uchardet)
    if (detectUchardet(data, result)) {
        result.friendlyName = formatDisplayName(result.encoding, result.bomType);
        return result;
    }

    // Stage 6: Fallback for 8-bit text with high-order bytes (>= 0x80)
    const bool hasHighBytes = std::any_of(data.cbegin(), data.cend(), [](char c) {
        return static_cast<unsigned char>(c) >= 0x80;
    });

    if (hasHighBytes) {
        result.encoding = QStringLiteral("windows-1252");
        result.bomType = ScintillaNext::BomType::None;
        result.bomLength = 0;
        result.confidence = 60;
        result.detectionMethod = QStringLiteral("ANSI / Windows-1252 Fallback");
    } else {
        result.encoding = QStringLiteral("UTF-8");
        result.bomType = ScintillaNext::BomType::None;
        result.bomLength = 0;
        result.confidence = 90;
        result.detectionMethod = QStringLiteral("ASCII Default");
    }

    result.friendlyName = formatDisplayName(result.encoding, result.bomType);
    return result;
}

bool EncodingDetector::detectBom(const QByteArray &data, DetectionResult &result) {
    if (data.startsWith(BOM_UTF8_BYTES)) {
        result.encoding = QStringLiteral("UTF-8");
        result.bomType = ScintillaNext::BomType::Utf8;
        result.bomLength = 3;
        result.confidence = 100;
        result.detectionMethod = QStringLiteral("UTF-8 BOM");
        return true;
    }
    if (data.startsWith(BOM_UTF16LE_BYTES)) {
        result.encoding = QStringLiteral("UTF-16LE");
        result.bomType = ScintillaNext::BomType::Utf16LE;
        result.bomLength = 2;
        result.confidence = 100;
        result.detectionMethod = QStringLiteral("UTF-16LE BOM");
        return true;
    }
    if (data.startsWith(BOM_UTF16BE_BYTES)) {
        result.encoding = QStringLiteral("UTF-16BE");
        result.bomType = ScintillaNext::BomType::Utf16BE;
        result.bomLength = 2;
        result.confidence = 100;
        result.detectionMethod = QStringLiteral("UTF-16BE BOM");
        return true;
    }
    return false;
}

bool EncodingDetector::detectInterleavedUtf16(const QByteArray &data, DetectionResult &result) {
    const int sampleSize = qMin(static_cast<int>(data.size()), 4096);
    if (sampleSize < 16)
        return false;

    int nullsEven = 0;
    int nullsOdd = 0;
    int asciiEven = 0;
    int asciiOdd = 0;

    for (int i = 0; i < sampleSize; ++i) {
        const unsigned char c = static_cast<unsigned char>(data.at(i));
        const bool isAscii = (c >= 0x20 && c <= 0x7E) || c == '\t' || c == '\r' || c == '\n';
        if (i % 2 == 0) {
            if (c == 0x00)
                nullsEven++;
            if (isAscii)
                asciiEven++;
        } else {
            if (c == 0x00)
                nullsOdd++;
            if (isAscii)
                asciiOdd++;
        }
    }

    const int totalPairs = sampleSize / 2;
    // UTF-16LE: High byte is 0x00 (odd index), low byte is ASCII (even index)
    if (nullsOdd >= totalPairs * 0.70 && nullsEven <= totalPairs * 0.05 && asciiEven >= totalPairs * 0.60) {
        result.encoding = QStringLiteral("UTF-16LE");
        result.bomType = ScintillaNext::BomType::None;
        result.bomLength = 0;
        result.confidence = 90;
        result.detectionMethod = QStringLiteral("UTF-16LE (Interleaved NULs)");
        return true;
    }

    // UTF-16BE: High byte is 0x00 (even index), low byte is ASCII (odd index)
    if (nullsEven >= totalPairs * 0.70 && nullsOdd <= totalPairs * 0.05 && asciiOdd >= totalPairs * 0.60) {
        result.encoding = QStringLiteral("UTF-16BE");
        result.bomType = ScintillaNext::BomType::None;
        result.bomLength = 0;
        result.confidence = 90;
        result.detectionMethod = QStringLiteral("UTF-16BE (Interleaved NULs)");
        return true;
    }

    return false;
}

bool EncodingDetector::detectHeaderDeclaration(const QByteArray &data, DetectionResult &result) {
    const int headLen = qMin(static_cast<int>(data.size()), 2048);
    const QString head = QString::fromLatin1(data.constData(), headLen);

    // 1. XML: <?xml version="1.0" encoding="XYZ"?>
    static const QRegularExpression xmlRegex(QStringLiteral(R"(<\?xml\s+[^>]*encoding=["']([a-zA-Z0-9_\-]+)["'])"),
                                             QRegularExpression::CaseInsensitiveOption);
    auto xmlMatch = xmlRegex.match(head);
    if (xmlMatch.hasMatch()) {
        QString enc = canonicalizeEncodingName(xmlMatch.captured(1));
        if (isSupportedEncoding(enc)) {
            result.encoding = enc;
            result.bomType = ScintillaNext::BomType::None;
            result.bomLength = 0;
            result.confidence = 95;
            result.detectionMethod = QStringLiteral("XML Declaration");
            return true;
        }
    }

    // 2. HTML: <meta charset="XYZ"> or <meta http-equiv="Content-Type" content="...; charset=XYZ">
    static const QRegularExpression htmlRegex1(QStringLiteral(R"(<meta\s+[^>]*charset=["']?([a-zA-Z0-9_\-]+)["']?)"),
                                               QRegularExpression::CaseInsensitiveOption);
    auto htmlMatch1 = htmlRegex1.match(head);
    if (htmlMatch1.hasMatch()) {
        QString enc = canonicalizeEncodingName(htmlMatch1.captured(1));
        if (isSupportedEncoding(enc)) {
            result.encoding = enc;
            result.bomType = ScintillaNext::BomType::None;
            result.bomLength = 0;
            result.confidence = 95;
            result.detectionMethod = QStringLiteral("HTML charset");
            return true;
        }
    }

    static const QRegularExpression htmlRegex2(
        QStringLiteral(R"(<meta\s+[^>]*content=["'][^"']*charset=([a-zA-Z0-9_\-]+)["'])"),
        QRegularExpression::CaseInsensitiveOption);
    auto htmlMatch2 = htmlRegex2.match(head);
    if (htmlMatch2.hasMatch()) {
        QString enc = canonicalizeEncodingName(htmlMatch2.captured(1));
        if (isSupportedEncoding(enc)) {
            result.encoding = enc;
            result.bomType = ScintillaNext::BomType::None;
            result.bomLength = 0;
            result.confidence = 95;
            result.detectionMethod = QStringLiteral("HTML Content-Type charset");
            return true;
        }
    }

    // 3. Shebang / Python / Ruby coding cookies: # -*- coding: utf-8 -*- or # coding=utf-8
    static const QRegularExpression cookieRegex(
        QStringLiteral(R"(^[#;].*coding[:=]\s*([a-zA-Z0-9_\-]+))"),
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption);
    auto cookieMatch = cookieRegex.match(head);
    if (cookieMatch.hasMatch()) {
        QString enc = canonicalizeEncodingName(cookieMatch.captured(1));
        if (isSupportedEncoding(enc)) {
            result.encoding = enc;
            result.bomType = ScintillaNext::BomType::None;
            result.bomLength = 0;
            result.confidence = 95;
            result.detectionMethod = QStringLiteral("Script Coding Cookie");
            return true;
        }
    }

    // 4. CSS: @charset "XYZ";
    static const QRegularExpression cssRegex(QStringLiteral(R"(@charset\s+["']([a-zA-Z0-9_\-]+)["'])"),
                                             QRegularExpression::CaseInsensitiveOption);
    auto cssMatch = cssRegex.match(head);
    if (cssMatch.hasMatch()) {
        QString enc = canonicalizeEncodingName(cssMatch.captured(1));
        if (isSupportedEncoding(enc)) {
            result.encoding = enc;
            result.bomType = ScintillaNext::BomType::None;
            result.bomLength = 0;
            result.confidence = 95;
            result.detectionMethod = QStringLiteral("CSS @charset");
            return true;
        }
    }

    return false;
}

bool EncodingDetector::detectStrictUtf8(const QByteArray &data, DetectionResult &result) {
    const int len = qMin(static_cast<int>(data.size()), 65536);
    const unsigned char *bytes = reinterpret_cast<const unsigned char *>(data.constData());

    int validUtf8Count = 0;
    int asciiCount = 0;
    int i = 0;

    while (i < len) {
        const unsigned char b0 = bytes[i];
        if (b0 < 0x80) {
            asciiCount++;
            i++;
        } else if ((b0 >= 0xC2) && (b0 <= 0xDF)) {
            // 2-byte sequence
            if (i + 1 >= len)
                break;
            const unsigned char b1 = bytes[i + 1];
            if (b1 < 0x80 || b1 > 0xBF)
                return false;
            validUtf8Count++;
            i += 2;
        } else if (b0 == 0xE0) {
            // 3-byte sequence starting with 0xE0
            if (i + 2 >= len)
                break;
            const unsigned char b1 = bytes[i + 1];
            const unsigned char b2 = bytes[i + 2];
            if (b1 < 0xA0 || b1 > 0xBF || b2 < 0x80 || b2 > 0xBF)
                return false;
            validUtf8Count++;
            i += 3;
        } else if ((b0 >= 0xE1 && b0 <= 0xEC) || b0 == 0xEE || b0 == 0xEF) {
            // 3-byte sequence
            if (i + 2 >= len)
                break;
            const unsigned char b1 = bytes[i + 1];
            const unsigned char b2 = bytes[i + 2];
            if (b1 < 0x80 || b1 > 0xBF || b2 < 0x80 || b2 > 0xBF)
                return false;
            validUtf8Count++;
            i += 3;
        } else if (b0 == 0xED) {
            // 3-byte sequence starting with 0xED (exclude surrogates U+D800..U+DFFF)
            if (i + 2 >= len)
                break;
            const unsigned char b1 = bytes[i + 1];
            const unsigned char b2 = bytes[i + 2];
            if (b1 < 0x80 || b1 > 0x9F || b2 < 0x80 || b2 > 0xBF)
                return false;
            validUtf8Count++;
            i += 3;
        } else if (b0 == 0xF0) {
            // 4-byte sequence starting with 0xF0
            if (i + 3 >= len)
                break;
            const unsigned char b1 = bytes[i + 1];
            const unsigned char b2 = bytes[i + 2];
            const unsigned char b3 = bytes[i + 3];
            if (b1 < 0x90 || b1 > 0xBF || b2 < 0x80 || b2 > 0xBF || b3 < 0x80 || b3 > 0xBF)
                return false;
            validUtf8Count++;
            i += 4;
        } else if (b0 >= 0xF1 && b0 <= 0xF3) {
            // 4-byte sequence
            if (i + 3 >= len)
                break;
            const unsigned char b1 = bytes[i + 1];
            const unsigned char b2 = bytes[i + 2];
            const unsigned char b3 = bytes[i + 3];
            if (b1 < 0x80 || b1 > 0xBF || b2 < 0x80 || b2 > 0xBF || b3 < 0x80 || b3 > 0xBF)
                return false;
            validUtf8Count++;
            i += 4;
        } else if (b0 == 0xF4) {
            // 4-byte sequence starting with 0xF4 (limit to U+10FFFF)
            if (i + 3 >= len)
                break;
            const unsigned char b1 = bytes[i + 1];
            const unsigned char b2 = bytes[i + 2];
            const unsigned char b3 = bytes[i + 3];
            if (b1 < 0x80 || b1 > 0x8F || b2 < 0x80 || b2 > 0xBF || b3 < 0x80 || b3 > 0xBF)
                return false;
            validUtf8Count++;
            i += 4;
        } else {
            return false;
        }
    }

    if (validUtf8Count > 0) {
        result.encoding = QStringLiteral("UTF-8");
        result.bomType = ScintillaNext::BomType::None;
        result.bomLength = 0;
        result.confidence = 95;
        result.detectionMethod = QStringLiteral("Strict UTF-8 Validator");
        return true;
    }

    if (asciiCount > 0 && validUtf8Count == 0) {
        result.encoding = QStringLiteral("UTF-8");
        result.bomType = ScintillaNext::BomType::None;
        result.bomLength = 0;
        result.confidence = 90;
        result.detectionMethod = QStringLiteral("ASCII (UTF-8 Compatible)");
        return true;
    }

    return false;
}

bool EncodingDetector::detectUchardet(const QByteArray &data, DetectionResult &result) {
    uchardet_t ud = uchardet_new();
    if (!ud)
        return false;

    const size_t sampleLen = static_cast<size_t>(qMin(static_cast<int>(data.size()), 65536));
    int err = uchardet_handle_data(ud, data.constData(), sampleLen);
    uchardet_data_end(ud);

    if (err == 0) {
        const char *cs = uchardet_get_charset(ud);
        if (cs && cs[0] != '\0') {
            const QString detected = canonicalizeEncodingName(QString::fromLatin1(cs));
            uchardet_delete(ud);

            if (isSupportedEncoding(detected)) {
                result.encoding = detected;
                result.bomType = ScintillaNext::BomType::None;
                result.bomLength = 0;
                result.confidence = 85;
                result.detectionMethod = QStringLiteral("Mozilla uchardet");
                return true;
            }
        }
    }

    uchardet_delete(ud);
    return false;
}

QByteArray EncodingDetector::decodeToUtf8(const QByteArray &data, const QString &encodingName,
                                          ScintillaNext::BomType bom, bool *ok) {
    if (ok)
        *ok = true;
    const QString encUpper = encodingName.toUpper();

    // 1. Direct UTF-8
    if (encUpper == QStringLiteral("UTF-8") || encUpper == QStringLiteral("ASCII") ||
        encUpper == QStringLiteral("US-ASCII")) {
        int offset = 0;
        if (bom == ScintillaNext::BomType::Utf8 && data.startsWith(BOM_UTF8_BYTES)) {
            offset = 3;
        }
        return data.mid(offset);
    }

    // 2. UTF-16
    if (encUpper == QStringLiteral("UTF-16LE") || encUpper == QStringLiteral("UTF-16BE")) {
        const bool isBE = (encUpper == QStringLiteral("UTF-16BE"));
        int offset = 0;
        if (isBE && data.startsWith(BOM_UTF16BE_BYTES))
            offset = 2;
        else if (!isBE && data.startsWith(BOM_UTF16LE_BYTES))
            offset = 2;

        QStringDecoder decoder(isBE ? QStringDecoder::Utf16BE : QStringDecoder::Utf16LE);
        QString text = decoder(QByteArrayView(data.constData() + offset, data.size() - offset));
        return text.toUtf8();
    }

    // 3. General Multi-byte and 8-bit Encodings via QTextCodec
    QTextCodec *codec = QTextCodec::codecForName(encodingName.toUtf8());
    if (!codec) {
        codec = QTextCodec::codecForName(canonicalizeEncodingName(encodingName).toUtf8());
    }
    if (!codec) {
        codec = QTextCodec::codecForLocale();
    }

    if (codec) {
        int offset = 0;
        if (bom == ScintillaNext::BomType::Utf8 && data.startsWith(BOM_UTF8_BYTES))
            offset = 3;
        QString text = codec->toUnicode(data.constData() + offset, data.size() - offset);
        return text.toUtf8();
    }

    if (ok)
        *ok = false;
    return data;
}

QByteArray EncodingDetector::encodeFromUtf8(const QByteArray &utf8Data, const QString &encodingName,
                                            ScintillaNext::BomType bom, bool *ok) {
    if (ok)
        *ok = true;
    const QString encUpper = encodingName.toUpper();

    QByteArray bomBytes;
    switch (bom) {
        case ScintillaNext::BomType::Utf8:
            bomBytes = BOM_UTF8_BYTES;
            break;
        case ScintillaNext::BomType::Utf16LE:
            bomBytes = BOM_UTF16LE_BYTES;
            break;
        case ScintillaNext::BomType::Utf16BE:
            bomBytes = BOM_UTF16BE_BYTES;
            break;
        case ScintillaNext::BomType::None:
            break;
    }

    // 1. Target is UTF-8
    if (encUpper == QStringLiteral("UTF-8") || encUpper == QStringLiteral("ASCII")) {
        return bomBytes + utf8Data;
    }

    // 2. Target is UTF-16
    if (encUpper == QStringLiteral("UTF-16LE") || encUpper == QStringLiteral("UTF-16BE")) {
        const bool isBE = (encUpper == QStringLiteral("UTF-16BE"));
        QStringEncoder encoder(isBE ? QStringEncoder::Utf16BE : QStringEncoder::Utf16LE);
        QString text = QString::fromUtf8(utf8Data);
        return bomBytes + encoder(text);
    }

    // 3. Other encodings via QTextCodec
    QTextCodec *codec = QTextCodec::codecForName(encodingName.toUtf8());
    if (!codec) {
        codec = QTextCodec::codecForName(canonicalizeEncodingName(encodingName).toUtf8());
    }
    if (!codec) {
        codec = QTextCodec::codecForLocale();
    }

    if (codec) {
        QString text = QString::fromUtf8(utf8Data);
        QByteArray encoded = codec->fromUnicode(text);
        return bomBytes + encoded;
    }

    if (ok)
        *ok = false;
    return bomBytes + utf8Data;
}

QString EncodingDetector::canonicalizeEncodingName(const QString &rawName) {
    QString s = rawName.trimmed();
    QString lower = s.toLower().remove('-').remove('_');

    if (lower == "utf8")
        return QStringLiteral("UTF-8");
    if (lower == "utf16" || lower == "utf16le")
        return QStringLiteral("UTF-16LE");
    if (lower == "utf16be")
        return QStringLiteral("UTF-16BE");
    if (lower == "ascii" || lower == "usascii")
        return QStringLiteral("UTF-8");
    if (lower == "cp1252" || lower == "windows1252" || lower == "win1252" || lower == "ansi")
        return QStringLiteral("windows-1252");
    if (lower == "latin1" || lower == "iso88591")
        return QStringLiteral("ISO-8859-1");
    if (lower == "latin9" || lower == "iso885915")
        return QStringLiteral("ISO-8859-15");
    if (lower == "cp1250" || lower == "windows1250" || lower == "win1250")
        return QStringLiteral("windows-1250");
    if (lower == "cp1251" || lower == "windows1251" || lower == "win1251")
        return QStringLiteral("windows-1251");
    if (lower == "cp1253" || lower == "windows1253")
        return QStringLiteral("windows-1253");
    if (lower == "cp1254" || lower == "windows1254")
        return QStringLiteral("windows-1254");
    if (lower == "cp1255" || lower == "windows1255")
        return QStringLiteral("windows-1255");
    if (lower == "cp1256" || lower == "windows1256")
        return QStringLiteral("windows-1256");
    if (lower == "cp1257" || lower == "windows1257")
        return QStringLiteral("windows-1257");
    if (lower == "sjis" || lower == "shiftjis" || lower == "cp932")
        return QStringLiteral("Shift-JIS");
    if (lower == "eucjp")
        return QStringLiteral("EUC-JP");
    if (lower == "gb2312" || lower == "gbk" || lower == "gb18030" || lower == "cp936")
        return QStringLiteral("GB18030");
    if (lower == "big5" || lower == "big5hkscs" || lower == "cp950")
        return QStringLiteral("Big5");
    if (lower == "euckr" || lower == "cp949")
        return QStringLiteral("EUC-KR");
    if (lower == "koi8r")
        return QStringLiteral("KOI8-R");
    if (lower == "koi8u")
        return QStringLiteral("KOI8-U");
    if (lower == "cp437" || lower == "ibm437" || lower == "oem437")
        return QStringLiteral("IBM850");
    if (lower == "macroman" || lower == "macintosh")
        return QStringLiteral("macintosh");

    return s;
}

bool EncodingDetector::isSupportedEncoding(const QString &encodingName) {
    const QString encUpper = encodingName.toUpper();
    if (encUpper == "UTF-8" || encUpper == "UTF-16LE" || encUpper == "UTF-16BE")
        return true;

    const QTextCodec *codec = QTextCodec::codecForName(encodingName.toUtf8());
    return codec != nullptr;
}

QString EncodingDetector::formatDisplayName(const QString &encodingName, ScintillaNext::BomType bom) {
    const QString encUpper = encodingName.toUpper();
    if (encUpper == "UTF-8") {
        return (bom == ScintillaNext::BomType::Utf8) ? QStringLiteral("UTF-8 BOM") : QStringLiteral("UTF-8");
    }
    if (encUpper == "UTF-16LE") {
        return (bom == ScintillaNext::BomType::Utf16LE) ? QStringLiteral("UTF-16LE BOM") : QStringLiteral("UTF-16LE");
    }
    if (encUpper == "UTF-16BE") {
        return (bom == ScintillaNext::BomType::Utf16BE) ? QStringLiteral("UTF-16BE BOM") : QStringLiteral("UTF-16BE");
    }
    if (encodingName.compare("windows-1252", Qt::CaseInsensitive) == 0) {
        return QStringLiteral("Windows-1252 (ANSI)");
    }
    return encodingName;
}

QList<EncodingDetector::EncodingInfo> EncodingDetector::supportedEncodings() {
    return {
        // Unicode
        {QStringLiteral("UTF-8"), QStringLiteral("Unicode"), QStringLiteral("UTF-8")},
        {QStringLiteral("UTF-16LE"), QStringLiteral("Unicode"), QStringLiteral("UTF-16 Little Endian (LE)")},
        {QStringLiteral("UTF-16BE"), QStringLiteral("Unicode"), QStringLiteral("UTF-16 Big Endian (BE)")},

        // Western European & ANSI
        {QStringLiteral("windows-1252"), QStringLiteral("Western European"),
         QStringLiteral("Windows-1252 (ANSI / Western)")},
        {QStringLiteral("ISO-8859-1"), QStringLiteral("Western European"), QStringLiteral("ISO-8859-1 (Latin-1)")},
        {QStringLiteral("ISO-8859-15"), QStringLiteral("Western European"), QStringLiteral("ISO-8859-15 (Latin-9)")},
        {QStringLiteral("macintosh"), QStringLiteral("Western European"), QStringLiteral("Macintosh (Mac Roman)")},
        {QStringLiteral("IBM850"), QStringLiteral("Legacy DOS/OEM"), QStringLiteral("DOS-850 (Western European)")},

        // Central & Eastern European
        {QStringLiteral("windows-1250"), QStringLiteral("Central European"),
         QStringLiteral("Windows-1250 (Central European)")},
        {QStringLiteral("ISO-8859-2"), QStringLiteral("Central European"),
         QStringLiteral("ISO-8859-2 (Central European)")},

        // Cyrillic
        {QStringLiteral("windows-1251"), QStringLiteral("Cyrillic"), QStringLiteral("Windows-1251 (Cyrillic)")},
        {QStringLiteral("KOI8-R"), QStringLiteral("Cyrillic"), QStringLiteral("KOI8-R (Russian)")},
        {QStringLiteral("KOI8-U"), QStringLiteral("Cyrillic"), QStringLiteral("KOI8-U (Ukrainian)")},

        // East Asian
        {QStringLiteral("Shift-JIS"), QStringLiteral("East Asian"), QStringLiteral("Shift-JIS (Japanese)")},
        {QStringLiteral("EUC-JP"), QStringLiteral("East Asian"), QStringLiteral("EUC-JP (Japanese)")},
        {QStringLiteral("GB18030"), QStringLiteral("East Asian"), QStringLiteral("GB18030 / GBK (Simplified Chinese)")},
        {QStringLiteral("Big5"), QStringLiteral("East Asian"), QStringLiteral("Big5 (Traditional Chinese)")},
        {QStringLiteral("EUC-KR"), QStringLiteral("East Asian"), QStringLiteral("EUC-KR (Korean)")},

        // Middle Eastern & Mediterranean
        {QStringLiteral("windows-1256"), QStringLiteral("Middle Eastern"), QStringLiteral("Windows-1256 (Arabic)")},
        {QStringLiteral("windows-1255"), QStringLiteral("Middle Eastern"), QStringLiteral("Windows-1255 (Hebrew)")},
        {QStringLiteral("windows-1254"), QStringLiteral("Middle Eastern"), QStringLiteral("Windows-1254 (Turkish)")},
        {QStringLiteral("windows-1253"), QStringLiteral("Middle Eastern"), QStringLiteral("Windows-1253 (Greek)")},

        // Baltic
        {QStringLiteral("windows-1257"), QStringLiteral("Baltic"), QStringLiteral("Windows-1257 (Baltic)")}};
}
