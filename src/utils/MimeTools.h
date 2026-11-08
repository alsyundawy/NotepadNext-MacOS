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

#ifndef MIMETOOLS_H
#define MIMETOOLS_H

#include <QString>
#include <QStringList>
#include <QByteArray>

class MimeTools {
public:
    // Base64 Suite
    static QString base64Encode(const QString &input);
    static QString base64EncodeWithPadding(const QString &input);
    static QString base64EncodeUnixEol(const QString &input, int lineLength = 76);
    static QString base64EncodeByLine(const QString &input);
    static QString base64Decode(const QString &input);
    static QString base64DecodeStrict(const QString &input, bool *ok = nullptr);
    static QString base64DecodeByLine(const QString &input);

    // Quoted-Printable Suite
    static QString quotedPrintableEncode(const QString &input);
    static QString quotedPrintableDecode(const QString &input);

    // URL Encoding Suite
    static QString urlEncodeRfc1738(const QString &input);
    static QString urlEncodeRfc1738ByLine(const QString &input);
    static QString urlEncodeExtended(const QString &input);
    static QString urlEncodeExtendedByLine(const QString &input);
    static QString urlEncodeFull(const QString &input);
    static QString urlEncodeFullByLine(const QString &input);
    static QString urlDecode(const QString &input);

private:
    template <typename Func>
    static QString processByLine(const QString &input, Func &&func) {
        if (input.isEmpty()) {
            return {};
        }

        const bool hasCrLf = input.contains(QLatin1String("\r\n"));
        const QString eol = hasCrLf ? QStringLiteral("\r\n") : QStringLiteral("\n");

        const QStringList lines = input.split(QChar('\n'));
        QStringList processed;
        processed.reserve(lines.size());

        for (const QString &line : lines) {
            QString cleanLine = line;
            if (cleanLine.endsWith(QChar('\r'))) {
                cleanLine.chop(1);
            }
            processed.append(func(cleanLine));
        }

        return processed.join(eol);
    }
};

#endif // MIMETOOLS_H
