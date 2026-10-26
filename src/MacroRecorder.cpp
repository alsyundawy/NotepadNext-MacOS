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

#include "MacroRecorder.h"
#include "ScintillaNext.h"
#include <QMetaEnum>

using namespace Scintilla;


MacroRecorder::MacroRecorder(QObject *parent) :
    QObject(parent)
{
}

void MacroRecorder::startRecording(ScintillaNext *targetEditor)
{
    this->editor = targetEditor;
    connect(targetEditor, &ScintillaNext::macroRecord, this, &MacroRecorder::recordMacroStep);

    // We don't "own" the macro so don't delete it if it is a valid pointer
    macro = new Macro();

    targetEditor->startRecord();
}

Macro *MacroRecorder::stopRecording()
{
    disconnect(editor, &ScintillaNext::macroRecord, this, &MacroRecorder::recordMacroStep);

    editor->stopRecord();

    Macro *m = macro;
    macro = Q_NULLPTR;
    return m;
}

void MacroRecorder::recordMacroStep(Message message, uptr_t wParam, sptr_t lParam)
{
    qInfo(Q_FUNC_INFO);

    macro->addMacroStep(message, wParam, lParam);
}
