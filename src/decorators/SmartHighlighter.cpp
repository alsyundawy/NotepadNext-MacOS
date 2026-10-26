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

#include "SmartHighlighter.h"

using namespace Scintilla;


SmartHighlighter::SmartHighlighter(ScintillaNext *editor) :
    EditorDecorator(editor)
{
    setObjectName("SmartHighlighter");

    indicator = editor->allocateIndicator("smart_highlighter");

    editor->indicSetFore(indicator, 0x00FF00);
    editor->indicSetStyle(indicator, INDIC_ROUNDBOX);
    editor->indicSetOutlineAlpha(indicator, 150);
    editor->indicSetAlpha(indicator, 100);
    editor->indicSetUnder(indicator, true);
}

void SmartHighlighter::notify(const NotificationData *pscn)
{
    if (pscn->nmhdr.code == Notification::UpdateUI && (FlagSet(pscn->updated, Update::Text) || FlagSet(pscn->updated, Update::Selection))) {
        highlightCurrentView();
    }
}

void SmartHighlighter::highlightCurrentView()
{
    editor->setIndicatorCurrent(indicator);
    editor->indicatorClearRange(0, static_cast<int>(editor->length()));

    if (editor->selectionEmpty()) {
        return;
    }

    const auto mainSelection = static_cast<int>(editor->mainSelection());
    const auto selectionStart = static_cast<int>(editor->selectionNStart(mainSelection));
    const auto selectionEnd = static_cast<int>(editor->selectionNEnd(mainSelection));

    // Make sure the current selection is valid
    if (selectionStart == selectionEnd) {
        return;
    }

    const auto curPos = static_cast<int>(editor->currentPos());
    const auto wordStart = static_cast<int>(editor->wordStartPosition(curPos, true));
    const auto wordEnd = static_cast<int>(editor->wordEndPosition(wordStart, true));

    // Make sure the selection is on word boundaries
    if (wordStart == wordEnd || wordStart != selectionStart || wordEnd != selectionEnd) {
        return;
    }

    const QByteArray selText = editor->get_text_range(selectionStart, selectionEnd);

    // TODO: Handle large files. By default Notepad++ only monitors the text on screen. However,
    // that will not work when using a highlighted scroll bar. Testing with small files seems to
    // have minimal impact. For large files, Qt can have a timer set to 0 to do heavier processing.
    // Using threads seems to be a bit overkill and too burdensome to do it properly.

    //const int startLine = editor->firstVisibleLine();
    //const int linesOnScreen = editor->linesOnScreen();
    //const int startPos = editor->positionFromLine(startLine);
    //const int endPos = editor->lineEndPosition(startLine + linesOnScreen);

    // TODO: skip hidden or folded lines?

    Sci_TextToFind ttf {{0, static_cast<Sci_PositionCR>(editor->length())}, selText.constData(), {-1, -1}};
    const auto flags = static_cast<int>(static_cast<unsigned int>(SCFIND_MATCHCASE) | static_cast<unsigned int>(SCFIND_WHOLEWORD));

    while (editor->send(SCI_FINDTEXT, flags, reinterpret_cast<sptr_t>(&ttf)) != -1) {
        editor->indicatorFillRange(ttf.chrgText.cpMin, ttf.chrgText.cpMax - ttf.chrgText.cpMin);
        ttf.chrg.cpMin = ttf.chrgText.cpMax;
    }
}
