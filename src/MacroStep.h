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

#ifndef MACROSTEP_H
#define MACROSTEP_H

#include "ScintillaNext.h"
#include <QDataStream>

class MacroStep {
public:
    MacroStep() = default;
    MacroStep(Scintilla::Message message, uptr_t wParam, sptr_t lParamValue);
    ~MacroStep() = default;

    QString toString() const;
    QString getName() const;

    void replay(const ScintillaNext *editor) const;

    friend QDataStream &operator<<(QDataStream& stream, const MacroStep &macroStep)
    {
        stream << macroStep.message << static_cast<quint32>(macroStep.wParam);

        if (MacroStep::MessageHasString(macroStep.message)) {
            return stream << macroStep.str;
        } else {
            return stream << static_cast<qint32>(macroStep.lParam);
        }
    }

    friend QDataStream &operator>>(QDataStream& stream, MacroStep &macroStep)
    {
        quint32 w = 0;
        stream >> macroStep.message >> w;
        macroStep.wParam = w;

        if (MacroStep::MessageHasString(macroStep.message)) {
            return stream >> macroStep.str;
        } else {
            qint32 l = 0;
            stream >> l;
            macroStep.lParam = l;
            return stream;
        }
    }

    static bool MessageHasString(Scintilla::Message message);
    static QString NameOfMessage(Scintilla::Message message);
    static QList<Scintilla::Message> RecordableMacroMessages();

    Scintilla::Message message;
    uptr_t wParam;
    sptr_t lParam;
    QByteArray str;
};
Q_DECLARE_METATYPE(MacroStep)

#endif // MACROSTEP_H
