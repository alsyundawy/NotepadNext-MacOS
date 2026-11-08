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

#include "NumberConverter.h"
#include "ScintillaNext.h"
#include "UndoAction.h"

#include <algorithm>
#include <QByteArray>

NumberConverter::Result NumberConverter::convertString(const QString &input, TargetBase targetBase, bool uppercaseHex,
                                                       bool addPrefix) {
    const QString text = input.trimmed();
    if (text.isEmpty()) {
        return {false, input, QString(), QStringLiteral("Empty number string")};
    }

    bool ok = false;
    qulonglong value = 0;

    if (text.startsWith(QLatin1String("0x"), Qt::CaseInsensitive)) {
        value = text.mid(2).toULongLong(&ok, 16);
    } else if (text.startsWith(QLatin1String("0b"), Qt::CaseInsensitive)) {
        value = text.mid(2).toULongLong(&ok, 2);
    } else if (text.startsWith(QLatin1String("0o"), Qt::CaseInsensitive)) {
        value = text.mid(2).toULongLong(&ok, 8);
    } else {
        const bool hasHexAlpha = std::any_of(text.cbegin(), text.cend(), [](const QChar &ch) {
            return (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
        });

        if (hasHexAlpha) {
            value = text.toULongLong(&ok, 16);
        } else {
            value = text.toULongLong(&ok, 10);
            if (!ok) {
                value = text.toULongLong(&ok, 16);
            }
        }
    }

    if (!ok) {
        return {false, text, QString(), QStringLiteral("Could not parse number")};
    }

    QString formatted;
    switch (targetBase) {
        case ToHex: {
            QString hex = QString::number(value, 16);
            if (uppercaseHex) {
                hex = hex.toUpper();
            }
            formatted = addPrefix ? (QStringLiteral("0x") + hex) : hex;
            break;
        }
        case ToDec: {
            formatted = QString::number(value, 10);
            break;
        }
        case ToBin: {
            QString bin = QString::number(value, 2);
            formatted = addPrefix ? (QStringLiteral("0b") + bin) : bin;
            break;
        }
        case ToOct: {
            QString oct = QString::number(value, 8);
            formatted = addPrefix ? (QStringLiteral("0o") + oct) : oct;
            break;
        }
    }

    return {true, text, formatted, QString()};
}

NumberConverter::Result NumberConverter::convert(ScintillaNext *editor, TargetBase targetBase, bool uppercaseHex,
                                                 bool addPrefix) {
    if (!editor) {
        return {false, QString(), QString(), QStringLiteral("No active editor")};
    }

    int startPos = editor->selectionStart();
    int endPos = editor->selectionEnd();

    if (startPos == endPos) {
        const int curPos = editor->currentPos();
        const int docLen = editor->length();

        startPos = curPos;
        while (startPos > 0) {
            const char ch = static_cast<char>(editor->charAt(startPos - 1));
            if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') || ch == 'x' ||
                ch == 'X' || ch == 'b' || ch == 'B' || ch == 'o' || ch == 'O') {
                --startPos;
            } else {
                break;
            }
        }

        endPos = curPos;
        while (endPos < docLen) {
            const char ch = static_cast<char>(editor->charAt(endPos));
            if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') || ch == 'x' ||
                ch == 'X' || ch == 'b' || ch == 'B' || ch == 'o' || ch == 'O') {
                ++endPos;
            } else {
                break;
            }
        }
    }

    if (startPos >= endPos) {
        return {false, QString(), QString(), QStringLiteral("No number selected or at cursor")};
    }

    const QByteArray textBytes = editor->get_text_range(startPos, endPos);
    const QString originalText = QString::fromUtf8(textBytes);

    const Result res = convertString(originalText, targetBase, uppercaseHex, addPrefix);
    if (!res.success) {
        return res;
    }

    const UndoAction ua(editor);
    editor->setTargetRange(startPos, endPos);
    const QByteArray utf8Converted = res.converted.toUtf8();
    editor->replaceTarget(utf8Converted.length(), utf8Converted.constData());
    editor->setSelection(startPos + utf8Converted.length(), startPos);

    return res;
}
