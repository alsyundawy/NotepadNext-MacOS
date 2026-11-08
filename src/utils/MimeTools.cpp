/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUSTION | https://www.alsyundawy.com
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

#include "MimeTools.h"
#include <QCoreApplication>
#include <QUrl>

QString MimeTools::base64Encode(const QString &input) {
    return QString::fromLatin1(input.toUtf8().toBase64(QByteArray::OmitTrailingEquals));
}

QString MimeTools::base64EncodeWithPadding(const QString &input) {
    return QString::fromLatin1(input.toUtf8().toBase64(QByteArray::KeepTrailingEquals));
}

QString MimeTools::base64EncodeUnixEol(const QString &input, int lineLength) {
    QString b64 = base64Encode(input);
    if (lineLength <= 0 || b64.length() <= lineLength) {
        return b64;
    }

    QString result;
    result.reserve(b64.length() + (b64.length() / lineLength) + 1);
    for (qsizetype i = 0; i < b64.length(); i += lineLength) {
        if (i > 0) {
            result.append(QLatin1Char('\n'));
        }
        result.append(b64.mid(i, lineLength));
    }
    return result;
}

QString MimeTools::base64EncodeByLine(const QString &input) {
    return processByLine(input, &MimeTools::base64Encode);
}

QString MimeTools::base64Decode(const QString &input) {
    return QString::fromUtf8(QByteArray::fromBase64(input.toUtf8()));
}

QString MimeTools::base64DecodeStrict(const QString &input, bool *ok) {
    if (const auto result =
            QByteArray::fromBase64Encoding(input.toUtf8().trimmed(), QByteArray::AbortOnBase64DecodingErrors)) {
        if (ok) {
            *ok = true;
        }
        return QString::fromUtf8(*result);
    }
    if (ok) {
        *ok = false;
    }
    return QCoreApplication::translate("MimeTools", "[Error: Invalid Base64 Data or Padding]");
}

QString MimeTools::base64DecodeByLine(const QString &input) {
    return processByLine(input, &MimeTools::base64Decode);
}

QString MimeTools::quotedPrintableEncode(const QString &input) {
    const QByteArray utf8 = input.toUtf8();
    QByteArray encoded;
    encoded.reserve(utf8.size() * 3);

    int lineLen = 0;
    for (int i = 0; i < utf8.size(); ++i) {
        if (const auto byte = static_cast<unsigned char>(utf8.at(i));
            (byte >= 33 && byte <= 60) || (byte >= 62 && byte <= 126)) {
            encoded.append(static_cast<char>(byte));
            lineLen++;
        } else if (byte == ' ' || byte == '\t') {
            if (i + 1 < utf8.size() && (utf8.at(i + 1) == '\r' || utf8.at(i + 1) == '\n')) {
                encoded.append(QByteArray::fromHex(QByteArray::number(byte, 16).toUpper()));
                encoded.prepend('=');
                lineLen += 3;
            } else {
                encoded.append(static_cast<char>(byte));
                lineLen++;
            }
        } else if (byte == '\r' || byte == '\n') {
            encoded.append(static_cast<char>(byte));
            lineLen = 0;
        } else {
            encoded.append('=');
            const QString hex = QStringLiteral("%1").arg(byte, 2, 16, QLatin1Char('0')).toUpper();
            encoded.append(hex.toLatin1());
            lineLen += 3;
        }

        if (lineLen >= 73 && i + 1 < utf8.size() && utf8.at(i) != '\n') {
            encoded.append("=\r\n");
            lineLen = 0;
        }
    }

    return QString::fromUtf8(encoded);
}

static bool decodeEscape(const QByteArray &in, int &i, QByteArray &out) {
    if (in.at(i) != '=') {
        return false;
    }
    if (i + 2 < in.size() && in.at(i + 1) == '\r' && in.at(i + 2) == '\n') {
        i += 3;
        return true;
    }
    if (i + 1 < in.size() && (in.at(i + 1) == '\r' || in.at(i + 1) == '\n')) {
        i += 2;
        return true;
    }
    if (i + 2 < in.size()) {
        bool ok = false;
        if (const int val = in.mid(i + 1, 2).toInt(&ok, 16); ok) {
            out.append(static_cast<char>(val));
            i += 3;
            return true;
        }
    }
    return false;
}

QString MimeTools::quotedPrintableDecode(const QString &input) {
    const QByteArray in = input.toUtf8();
    QByteArray out;
    out.reserve(in.size());

    int i = 0;
    while (i < in.size()) {
        if (decodeEscape(in, i, out)) {
            continue;
        }
        out.append(in.at(i));
        i++;
    }

    return QString::fromUtf8(out);
}

QString MimeTools::urlEncodeRfc1738(const QString &input) {
    return QString::fromUtf8(QUrl::toPercentEncoding(input, ".-_~"));
}

QString MimeTools::urlEncodeRfc1738ByLine(const QString &input) {
    return processByLine(input, &MimeTools::urlEncodeRfc1738);
}

QString MimeTools::urlEncodeExtended(const QString &input) {
    return QString::fromUtf8(QUrl::toPercentEncoding(input, ".-_~/:?#[]@!$&'()*+,;="));
}

QString MimeTools::urlEncodeExtendedByLine(const QString &input) {
    return processByLine(input, &MimeTools::urlEncodeExtended);
}

QString MimeTools::urlEncodeFull(const QString &input) {
    return QString::fromUtf8(QUrl::toPercentEncoding(input));
}

QString MimeTools::urlEncodeFullByLine(const QString &input) {
    return processByLine(input, &MimeTools::urlEncodeFull);
}

QString MimeTools::urlDecode(const QString &input) {
    return QUrl::fromPercentEncoding(input.toUtf8());
}
