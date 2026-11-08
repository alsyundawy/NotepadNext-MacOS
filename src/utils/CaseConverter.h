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

#ifndef CASECONVERTER_H
#define CASECONVERTER_H

#include <cstdint>
#include <QString>

class ScintillaNext;

class CaseConverter {
public:
    enum class CaseType : std::uint8_t {
        UpperCase,
        LowerCase,
        InvertCase,
        TitleCase,
        SentenceCase,
        SnakeCase,
        CamelCase,
        PascalCase,
        KebabCase,
        ConstantCase
    };

    struct ConvertResult {
        bool success = false;
        int selectionsConverted = 0;
        int charactersChanged = 0;
        QString message;
        QString errorMessage;
    };

    // Pure string transformation methods (Unicode-aware)
    static QString toUpper(const QString &text);
    static QString toLower(const QString &text);
    static QString toInvertCase(const QString &text);
    static QString toTitleCase(const QString &text);
    static QString toSentenceCase(const QString &text);
    static QString toSnakeCase(const QString &text);
    static QString toCamelCase(const QString &text);
    static QString toPascalCase(const QString &text);
    static QString toKebabCase(const QString &text);
    static QString toConstantCase(const QString &text);

    static QString convertText(const QString &text, CaseType caseType);

    static ConvertResult convertEditor(ScintillaNext *editor, CaseType caseType);
    static ConvertResult upperCase(ScintillaNext *editor);
    static ConvertResult lowerCase(ScintillaNext *editor);
    static ConvertResult invertCase(ScintillaNext *editor);
    static ConvertResult titleCase(ScintillaNext *editor);
    static ConvertResult sentenceCase(ScintillaNext *editor);
    static ConvertResult snakeCase(ScintillaNext *editor);
    static ConvertResult camelCase(ScintillaNext *editor);
    static ConvertResult pascalCase(ScintillaNext *editor);
    static ConvertResult kebabCase(ScintillaNext *editor);
    static ConvertResult constantCase(ScintillaNext *editor);
};

#endif // CASECONVERTER_H
