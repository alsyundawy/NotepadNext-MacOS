#pragma once
/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef CHARACTERINFO_H
#define CHARACTERINFO_H

#include <QString>
#include <QByteArray>
#include <cstdint>

class ScintillaNext;

class CharacterInfo {
public:
    struct Details {
        uint32_t codePoint{0};
        QString glyph;
        QString displayGlyph;
        QString name;
        QString block;
        QString categoryCode;
        QString categoryDescription;
        QString script;
        QString direction;
        QByteArray utf8Bytes;
        QString utf8Hex;
        QString utf8Dec;
        QString utf16Hex;
        QString utf16Dec;
        QString utf32Hex;
        QString htmlNamedEntity;
        QString htmlDecEntity;
        QString htmlHexEntity;
        QString cEscape;
        QString jsonEscape;
        QString cssEscape;
        QString urlPercent;
        bool isValid{false};
    };

    static Details getDetailsAt(const ScintillaNext *editor, int pos = -1);
    static Details getDetailsForCodePoint(uint32_t codePoint);
    static QString formatSummary(const Details &d);
    static QString formatCodePoint(uint32_t cp);
};

#endif  // CHARACTERINFO_H
