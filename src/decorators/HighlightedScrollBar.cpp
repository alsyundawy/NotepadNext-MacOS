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

#include <QPainter>

#include "HighlightedScrollBar.h"

using namespace Scintilla;

const int DEFAULT_TICK_HEIGHT = 3;
const int DEFAULT_TICK_PADDING = 3;
const QColor CURSOR_SELECTION_COLOR = QColor(0, 0, 0, 25);
const QColor CURSOR_CARET_COLOR = QColor(0, 0, 0, 100);

HighlightedScrollBarDecorator::HighlightedScrollBarDecorator(ScintillaNext *editor)
    : EditorDecorator(editor), scrollBar(new HighlightedScrollBar(editor, Qt::Vertical, editor)) {
    connect(scrollBar, &QScrollBar::valueChanged, editor, &ScintillaEditBase::scrollVertical);

    editor->setVerticalScrollBar(scrollBar);
}

HighlightedScrollBarDecorator::~HighlightedScrollBarDecorator() = default;

void HighlightedScrollBarDecorator::notify(const NotificationData *pscn) {
    if ((pscn->nmhdr.code == Notification::UpdateUI &&
         (FlagSet(pscn->updated, Update::Text) || FlagSet(pscn->updated, Update::Selection))) ||
        (pscn->nmhdr.code == Notification::Modified &&
         FlagSet(pscn->modificationType, ModificationFlags::ChangeMarker))) {
        scrollBar->update();
    }
}

HighlightedScrollBar::HighlightedScrollBar(ScintillaNext *editor, Qt::Orientation orientation, QWidget *parent)
    : QScrollBar(orientation, parent), editor(editor) {
    smartHighlighterIndicator = editor->allocateIndicator("smart_highlighter");
}

void HighlightedScrollBar::paintEvent(QPaintEvent *event) {
    // Paint the default scrollbar first
    QScrollBar::paintEvent(event);
    if (!editor || editor->length() <= 0) {
        return;
    }

    sptr_t totalLines = editor->visibleFromDocLine(editor->lineCount());
    if (!editor->endAtLastLine()) {
        totalLines += editor->linesOnScreen();
    }
    if (totalLines <= 0) {
        return;
    }

    const int arrowH = scrollbarArrowHeight();
    const int trackH = rect().height() - (arrowH * 2);
    if (trackH <= 0) {
        return;
    }

    QPainter p(this);
    drawMarker(p, 24, totalLines, trackH, arrowH);
    if (smartHighlighterIndicator >= 0) {
        drawIndicator(p, smartHighlighterIndicator, totalLines, trackH, arrowH);
    }
    drawCursors(p, totalLines, trackH, arrowH);
}

void HighlightedScrollBar::drawMarker(QPainter &p, int marker, sptr_t totalLines, int trackH, int arrowH) const {
    sptr_t curLine = 0;

    while ((curLine = editor->markerNext(curLine, 1 << marker)) != -1) {
        drawTickMark(p, lineToScrollBarY(curLine, totalLines, trackH), DEFAULT_TICK_HEIGHT, QColor(100, 100, 255),
                     arrowH);
        curLine++;
    }
}

void HighlightedScrollBar::drawIndicator(QPainter &p, int indicator, sptr_t totalLines, int trackH, int arrowH) const {
    const sptr_t docLength = editor->length();
    if (docLength <= 0 || docLength > 10L * 1024 * 1024) {
        return;
    }

    sptr_t curPos = 0;
    const auto color = static_cast<int>(editor->indicFore(indicator));
    int safetyCount = 0;

    while (curPos < docLength && safetyCount < 1500) {
        safetyCount++;
        const sptr_t nextPos = editor->indicatorEnd(indicator, curPos);
        if (nextPos <= curPos) {
            break;
        }
        if (editor->indicatorValueAt(indicator, curPos)) {
            drawTickMark(p, posToScrollBarY(curPos, totalLines, trackH), DEFAULT_TICK_HEIGHT, color, arrowH);
        }
        curPos = nextPos;
    }
}

void HighlightedScrollBar::drawCursors(QPainter &p, sptr_t totalLines, int trackH, int arrowH) const {
    const sptr_t numSelections = editor->selections();
    for (sptr_t i = 0; i < numSelections; i++) {
        const int startCaretY = posToScrollBarY(editor->selectionNCaret(i), totalLines, trackH);
        if (const int startAnchorY = posToScrollBarY(editor->selectionNAnchor(i), totalLines, trackH);
            startCaretY != startAnchorY) {
            const int topY = qMin(startCaretY, startAnchorY);
            const int h = qMax(1, qAbs(startCaretY - startAnchorY));
            drawTickMark(p, topY, h, CURSOR_SELECTION_COLOR, arrowH);
        }

        drawTickMark(p, startCaretY, DEFAULT_TICK_HEIGHT, CURSOR_CARET_COLOR, arrowH);
    }
}

void HighlightedScrollBar::drawTickMark(QPainter &p, int y, int height, const QColor &color, int arrowH) const {
    p.fillRect(rect().x() + DEFAULT_TICK_PADDING, y + arrowH, rect().width() - (DEFAULT_TICK_PADDING * 2), height,
               color);
}

int HighlightedScrollBar::posToScrollBarY(sptr_t pos, sptr_t totalLines, int trackH) const {
    const sptr_t line = editor->visibleFromDocLine(editor->lineFromPosition(pos));

    return lineToScrollBarY(line, totalLines, trackH);
}

int HighlightedScrollBar::lineToScrollBarY(sptr_t line, sptr_t totalLines, int trackH) {
    if (totalLines <= 0) {
        return 0;
    }

    return static_cast<int>(static_cast<double>(line) / static_cast<double>(totalLines) * static_cast<double>(trackH));
}

int HighlightedScrollBar::scrollbarArrowHeight() const {
    // NOTE: There is no official way to get the height of the scrollbar arrow buttons, however for now we can
    // assume that the buttons are square, meaning the height of them will be the same as the width of
    // the scroll bar.
    return rect().width();
}
