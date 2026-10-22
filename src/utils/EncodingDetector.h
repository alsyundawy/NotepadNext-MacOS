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

#pragma once
/*
 * This file is part of Notepad Next.
 *
 * Adapted from Notepad4 (zufuliu/notepad4, Encoding.c) and Notepad3 (rizonesoft/Notepad3)
 * with Mozilla Universal Charset Detector (uchardet).
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ENCODINGDETECTOR_H
#define ENCODINGDETECTOR_H

#include <QString>
#include <QByteArray>
#include <QList>
#include "ScintillaNext.h"

class EncodingDetector {
public:
    struct DetectionResult {
        QString encoding{
            "UTF-8"};  // Canonical codec name (e.g. "UTF-8", "windows-1252", "Shift-JIS", "GB18030", "UTF-16LE")
        QString friendlyName{"UTF-8"};  // Human-readable name
        ScintillaNext::BomType bomType{ScintillaNext::BomType::None};
        int bomLength{0};
        int confidence{100};      // 0 - 100
        QString detectionMethod;  // "BOM", "XML/HTML Declaration", "Coding Cookie", "Strict UTF-8 Validator", "UTF-16
                                  // Interleaved NULs", "Mozilla uchardet", "ANSI Fallback"
        bool isBinary{false};
    };

    struct EncodingInfo {
        QString name;         // Codec name for lookup
        QString category;     // e.g. "Unicode", "Western European", "East Asian", "Cyrillic", etc.
        QString description;  // e.g. "UTF-8", "Windows-1252 (Western European)", "Shift-JIS (Japanese)"
    };

    // Detects encoding and BOM from raw data sample
    static DetectionResult detect(const QByteArray &data);

    // Decodes raw data from given encoding into UTF-8 suitable for Scintilla
    static QByteArray decodeToUtf8(const QByteArray &data, const QString &encodingName, ScintillaNext::BomType bom,
                                   bool *ok = nullptr);

    // Encodes UTF-8 data from Scintilla into target encoding, prepending BOM if specified
    static QByteArray encodeFromUtf8(const QByteArray &utf8Data, const QString &encodingName,
                                     ScintillaNext::BomType bom, bool *ok = nullptr);

    // List of supported encodings grouped by category
    static QList<EncodingInfo> supportedEncodings();

    // Canonicalize encoding name (maps aliases like "cp1252", "latin1", "utf8", "sjis", "gb2312" to standard names)
    static QString canonicalizeEncodingName(const QString &rawName);

    // Friendly display string for status bar or dialog
    static QString formatDisplayName(const QString &encodingName, ScintillaNext::BomType bom);

    // Check if an encoding is supported by Qt's codec system
    static bool isSupportedEncoding(const QString &encodingName);

private:
    static bool detectBom(const QByteArray &data, DetectionResult &result);
    static bool detectInterleavedUtf16(const QByteArray &data, DetectionResult &result);
    static bool detectHeaderDeclaration(const QByteArray &data, DetectionResult &result);
    static bool detectStrictUtf8(const QByteArray &data, DetectionResult &result);
    static bool detectUchardet(const QByteArray &data, DetectionResult &result);
};

#endif  // ENCODINGDETECTOR_H
