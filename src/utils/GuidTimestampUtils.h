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

#ifndef GUIDTIMESTAMPUTILS_H
#define GUIDTIMESTAMPUTILS_H

#include <cstdint>
#include <QString>

class ScintillaNext;

class GuidTimestampUtils {
public:
    enum GuidFormat : std::uint8_t {
        GuidHyphenatedLower, // xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (RFC 4122)
        GuidHyphenatedUpper, // XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
        GuidWithBracesUpper, // {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
        GuidWithBracesLower, // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
        GuidCompactUpper,    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        GuidCompactLower     // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    };

    enum TimestampFormat : std::uint8_t {
        DateTimeShort,      // yyyy-MM-dd HH:mm:ss
        DateTimeLong,       // Locale long date & time
        Iso8601Local,       // yyyy-MM-ddTHH:mm:ss±HH:mm
        Iso8601Utc,         // yyyy-MM-ddTHH:mm:ssZ
        DateOnly,           // yyyy-MM-dd
        TimeOnly,           // HH:mm:ss
        UnixTimestampSecs,  // 1792352700
        UnixTimestampMillis // 1792352700000
    };

    struct Result {
        bool success{false};
        QString insertedText;
        QString errorMessage;
    };

    static QString generateGuid(GuidFormat format = GuidHyphenatedLower);
    static QString generateTimestamp(TimestampFormat format = DateTimeShort);

    static Result insertGuid(ScintillaNext *editor, GuidFormat format = GuidHyphenatedLower);
    static Result insertTimestamp(ScintillaNext *editor, TimestampFormat format = DateTimeShort);
    static Result insertText(ScintillaNext *editor, const QString &text);
};

#endif // GUIDTIMESTAMPUTILS_H
