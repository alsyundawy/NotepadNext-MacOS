/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUTION | https://www.alsyundawy.com
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

#include <QTimer>
#include <algorithm>

using namespace Scintilla;

SmartHighlighter::SmartHighlighter(ScintillaNext *editor) : EditorDecorator(editor), debounceTimer(new QTimer(this)) {
    setObjectName("SmartHighlighter");

    indicator = editor->allocateIndicator("smart_highlighter");

    editor->indicSetFore(indicator, 0x00FF00);
    editor->indicSetStyle(indicator, INDIC_ROUNDBOX);
    editor->indicSetOutlineAlpha(indicator, 150);
    editor->indicSetAlpha(indicator, 100);
    editor->indicSetUnder(indicator, true);

    debounceTimer->setSingleShot(true);
    debounceTimer->setInterval(100);
    connect(debounceTimer, &QTimer::timeout, this, &SmartHighlighter::highlightCurrentView);
}

void SmartHighlighter::notify(const NotificationData *pscn) {
    if (pscn->nmhdr.code == Notification::UpdateUI &&
        (FlagSet(pscn->updated, Update::Text) || FlagSet(pscn->updated, Update::Selection))) {
        debounceTimer->start();
    }
}

void SmartHighlighter::highlightCurrentView() {
    if (!editor) {
        return;
    }

    const sptr_t docLen = editor->length();
    if (docLen <= 0 || editor->selectionEmpty()) {
        if (m_hasHighlights) {
            editor->setIndicatorCurrent(indicator);
            editor->indicatorClearRange(0, docLen);
            m_hasHighlights = false;
        }
        return;
    }

    const sptr_t mainSelection = editor->mainSelection();
    const sptr_t selectionStart = editor->selectionNStart(mainSelection);
    const sptr_t selectionEnd = editor->selectionNEnd(mainSelection);
    const sptr_t selLen = selectionEnd - selectionStart;

    // Fast reject: if selection is empty or longer than a single reasonable word (e.g. multi-word or Select All)
    if (selLen <= 0 || selLen > 128) {
        if (m_hasHighlights) {
            editor->setIndicatorCurrent(indicator);
            editor->indicatorClearRange(0, docLen);
            m_hasHighlights = false;
        }
        return;
    }

    const sptr_t curPos = editor->currentPos();
    const sptr_t wordStart = editor->wordStartPosition(curPos, true);
    const sptr_t wordEnd = editor->wordEndPosition(wordStart, true);

    // Make sure the selection is strictly on word boundaries
    if (wordStart == wordEnd || wordStart != selectionStart || wordEnd != selectionEnd) {
        if (m_hasHighlights) {
            editor->setIndicatorCurrent(indicator);
            editor->indicatorClearRange(0, docLen);
            m_hasHighlights = false;
        }
        return;
    }

    const QByteArray selText = editor->get_text_range(selectionStart, selectionEnd);
    if (selText.trimmed().isEmpty()) {
        if (m_hasHighlights) {
            editor->setIndicatorCurrent(indicator);
            editor->indicatorClearRange(0, docLen);
            m_hasHighlights = false;
        }
        return;
    }

    // Always clear previous highlights before filling new ones
    editor->setIndicatorCurrent(indicator);
    editor->indicatorClearRange(0, docLen);
    m_hasHighlights = false;

    // For large files (> 1MB or > 5000 lines), constrain search to viewport +/- 150 lines
    Sci_PositionCR searchStart = 0;
    Sci_PositionCR searchEnd = docLen;
    if (docLen > 1024 * 1024 || editor->lineCount() > 5000) {
        const int firstLine = qMax(0, static_cast<int>(editor->firstVisibleLine()) - 150);
        const int lastLine = qMin(static_cast<int>(editor->lineCount()),
                                  static_cast<int>(editor->firstVisibleLine() + editor->linesOnScreen() + 150));
        searchStart = static_cast<Sci_PositionCR>(editor->positionFromLine(firstLine));
        searchEnd = static_cast<Sci_PositionCR>(editor->lineEndPosition(lastLine));
    }

    Sci_TextToFind ttf{{searchStart, searchEnd}, selText.constData(), {-1, -1}};
    const int flags = SCFIND_MATCHCASE | SCFIND_WHOLEWORD;
    int matchCount = 0;

    while (editor->send(SCI_FINDTEXT, flags, reinterpret_cast<sptr_t>(&ttf)) != -1) {
        if (ttf.chrgText.cpMax <= ttf.chrgText.cpMin) {
            break;
        }
        editor->indicatorFillRange(ttf.chrgText.cpMin, ttf.chrgText.cpMax - ttf.chrgText.cpMin);
        ttf.chrg.cpMin = ttf.chrgText.cpMax;
        m_hasHighlights = true;
        if (++matchCount > 1500) {
            break;  // Safety cap to prevent UI saturation
        }
    }
}
