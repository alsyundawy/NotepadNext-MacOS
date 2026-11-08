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

#ifndef BASE64UTILS_H
#define BASE64UTILS_H

#include <QString>
#include <QByteArray>

class ScintillaNext;

class Base64Utils {
public:
    struct Result {
        bool success{false};
        bool wholeDocument{false};
        QString errorMessage;
    };

    static Result encode(ScintillaNext *editor);
    static Result decode(ScintillaNext *editor);

    static QString encode(const QString &plainText);
    static QString decode(const QString &base64Text, bool *ok = nullptr);
    static QString encodeUrlSafe(const QString &plainText);
    static QString decodeUrlSafe(const QString &base64Text, bool *ok = nullptr);
};

#endif // BASE64UTILS_H
