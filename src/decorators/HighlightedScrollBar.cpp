/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry DS Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
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
    : EditorDecorator(editor), scrollBar(new HighlightedScrollBar(editor, Qt::Vertical, editor))
{
    connect(scrollBar, &QScrollBar::valueChanged, editor, &ScintillaEditBase::scrollVertical);

    editor->setVerticalScrollBar(scrollBar);
}

HighlightedScrollBarDecorator::~HighlightedScrollBarDecorator() = default;

void HighlightedScrollBarDecorator::notify(const NotificationData *pscn)
{
    if (pscn->nmhdr.code == Notification::UpdateUI && (FlagSet(pscn->updated, Update::Text) || FlagSet(pscn->updated, Update::Selection))) {
        scrollBar->update();
    }
    else if (pscn->nmhdr.code == Notification::Modified && FlagSet(pscn->modificationType, ModificationFlags::ChangeMarker)) {
        scrollBar->update();
    }
}




HighlightedScrollBar::HighlightedScrollBar(ScintillaNext *editor, Qt::Orientation orientation, QWidget *parent)
    : QScrollBar(orientation, parent), editor(editor)
{
    smartHighlighterIndicator = editor->allocateIndicator("smart_highlighter");
}

void HighlightedScrollBar::paintEvent(QPaintEvent *event)
{
    // Paint the default scrollbar first
    QScrollBar::paintEvent(event);
    QPainter p(this);

    drawMarker(p, 24);
    drawIndicator(p, smartHighlighterIndicator);
    drawCursors(p);
}

void HighlightedScrollBar::drawMarker(QPainter &p, int marker) const
{
    // NOTE: SCI_MARKERGETBACK doesn't exist...so can't use the marker color
    int curLine = 0;

    while ((curLine = static_cast<int>(editor->markerNext(curLine, 1 << marker))) != -1) {
        drawTickMark(p, lineToScrollBarY(curLine), DEFAULT_TICK_HEIGHT, QColor(100, 100, 255));

        curLine++;
    }
}

void HighlightedScrollBar::drawIndicator(QPainter &p, int indicator) const
{
    auto curPos = static_cast<int>(editor->indicatorEnd(indicator, 0));
    const auto color = static_cast<int>(editor->indicFore(indicator));

    if (curPos > 0) {
        while ((curPos = static_cast<int>(editor->indicatorEnd(indicator, curPos))) < editor->length()) {
            drawTickMark(p, posToScrollBarY(curPos), DEFAULT_TICK_HEIGHT, color);

            curPos = static_cast<int>(editor->indicatorEnd(indicator, curPos));
        }
    }
}

void HighlightedScrollBar::drawCursors(QPainter &p) const
{
    for (int i = 0; i < editor->selections(); i++) {
        int startCaretY = posToScrollBarY(static_cast<int>(editor->selectionNCaret(i)));

        if (int startAnchorY = posToScrollBarY(static_cast<int>(editor->selectionNAnchor(i))); startCaretY != startAnchorY) {
            int minY = std::min(startCaretY, startAnchorY);
            int maxY = std::max(startCaretY, startAnchorY);
            drawTickMark(p, minY, std::max(1, maxY - minY), CURSOR_SELECTION_COLOR);
        }

        drawTickMark(p, startCaretY, DEFAULT_TICK_HEIGHT, CURSOR_CARET_COLOR);
    }
}

void HighlightedScrollBar::drawTickMark(QPainter &p, int y, int height, const QColor &color) const
{
    p.fillRect(rect().x() + DEFAULT_TICK_PADDING, y + scrollbarArrowHeight(), rect().width() - (DEFAULT_TICK_PADDING * 2), height, color);
}

int HighlightedScrollBar::posToScrollBarY(int pos) const
{
    const auto line = static_cast<int>(editor->visibleFromDocLine(editor->lineFromPosition(pos)));

    return lineToScrollBarY(line);
}

int HighlightedScrollBar::lineToScrollBarY(int line) const
{
    auto lineCount = static_cast<int>(editor->visibleFromDocLine(editor->lineCount()));

    if (!editor->endAtLastLine()) {
        lineCount += editor->linesOnScreen();
    }

    if (lineCount <= 0) {
        return 0;
    }

    int trackHeight = rect().height() - scrollbarArrowHeight() * 2;
    if (trackHeight <= 0) {
        return 0;
    }

    return static_cast<int>(static_cast<double>(line) / lineCount * trackHeight);
}

int HighlightedScrollBar::scrollbarArrowHeight() const
{
    // NOTE: There is no official way to get the height of the scrollbar arrow buttons, however for now we can
    // assume that the buttons are square, meaning the height of them will be the same as the width of
    // the scroll bar.
    return rect().width();
}
