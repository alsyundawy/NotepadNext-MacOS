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

#include "Macro.h"

using namespace Scintilla;

Macro::Macro() : name("<Current Recorded Macro>") {}

void Macro::addMacroStep(Message message, uptr_t wParam, sptr_t lParam)
{
    qInfo(Q_FUNC_INFO);

    // Combine ReplaceSel messages into a single string
    if (message == Message::ReplaceSel && !steps.empty() && steps.constLast().message == Message::ReplaceSel) {
        steps.last().str.append(reinterpret_cast<const char*>(lParam)); // NOSONAR(cpp:S3630,cpp:S5416) // NOLINT(performance-no-int-to-ptr)
    }
    // Combine DeleteBack (backspace) with ReplaceSel
    else if (message == Message::DeleteBack && !steps.empty() && steps.constLast().message == Message::ReplaceSel) {
        if (steps.last().str.size() == 1) {
            // A single char left so just remove the action
            steps.takeLast();
        }
        else {
            steps.last().str.chop(1);
        }
    }
    else {
        steps.append(MacroStep(message, wParam, lParam));
    }

#ifdef QT_DEBUG
    for (const MacroStep &step : steps) {
        qInfo("%s", qUtf8Printable(step.toString()));
    }
#endif
}

void Macro::addMacroStep(const MacroStep &step)
{
    steps.append(step);
}

void Macro::replay(ScintillaNext *editor, int n) const
{
    qInfo(Q_FUNC_INFO);

    editor->beginUndoAction();

    while (n > 0) {
        for (const MacroStep &step : steps) {
            step.replay(editor);
        }

        --n;
    }

    editor->endUndoAction();
}

void Macro::replayTillEndOfFile(ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    editor->beginUndoAction();

    do {
        const auto length = editor->length();
        const auto curPos = editor->currentPos();

        replay(editor);

        // The doc can shrink...yay
        if (editor->length() < length) {
            continue;
        }
        // The doc can grow...
        else if (editor->length() > length) {
            // are we going to catch up?
            const auto deltaLength = editor->length() - length;
            const auto deltaPos = editor->currentPos() - curPos;
            if (deltaPos > deltaLength) {
                // Cursor position is moving forward more than document is growing
                continue;
            }
        }
        // the file can stay the same
        else {
            // is the cursor moving?
            if (editor->currentPos() != curPos) {
                continue;
            }
        }

        break;
    } while (true);

    editor->endUndoAction();
}

const QString &Macro::getName() const
{
    return name;
}

void Macro::setName(const QString &value)
{
    name = value;
}
