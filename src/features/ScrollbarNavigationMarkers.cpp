/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "ScrollbarNavigationMarkers.h"
#include "ScintillaNext.h"
#include <algorithm>

int ScrollbarNavigationMarkers::lineToY(int line, int totalLines, const QRect &trackRect, int arrowHeight) {
    if (totalLines <= 1) {
        return trackRect.top() + arrowHeight;
    }
    const int availableHeight = trackRect.height() - (2 * arrowHeight);
    const double ratio = static_cast<double>(line) / static_cast<double>(totalLines - 1);
    return trackRect.top() + arrowHeight + static_cast<int>(ratio * availableHeight);
}

int ScrollbarNavigationMarkers::posToY(qint64 pos, qint64 totalLength, const QRect &trackRect, int arrowHeight) {
    if (totalLength <= 0) {
        return trackRect.top() + arrowHeight;
    }
    const int availableHeight = trackRect.height() - (2 * arrowHeight);
    const double ratio = static_cast<double>(pos) / static_cast<double>(totalLength);
    return trackRect.top() + arrowHeight + static_cast<int>(ratio * availableHeight);
}

void ScrollbarNavigationMarkers::drawTick(QPainter &p, const QRect &trackRect, int y, int height, const QColor &color) {
    p.fillRect(trackRect.left() + 2, y, trackRect.width() - 4, std::max(2, height), color);
}

void ScrollbarNavigationMarkers::paintNavigationMarkers(QPainter &p, ScintillaNext *editor, const QRect &trackRect,
                                                        int arrowHeight) {
    if (!editor || trackRect.height() <= (2 * arrowHeight)) {
        return;
    }

    const auto totalLines = static_cast<int>(editor->lineCount());
    const auto totalLength = static_cast<qint64>(editor->length());

    // 1. Draw bookmarks (marker 24) in vibrant blue
    sptr_t curLine = 0;
    while ((curLine = editor->markerNext(curLine, 1 << 24)) != -1) {
        const int y = lineToY(static_cast<int>(curLine), totalLines, trackRect, arrowHeight);
        drawTick(p, trackRect, y, 3, QColor(56, 139, 253));
        curLine++;
    }

    // 2. Draw caret tick in bright red
    const int caretY = posToY(editor->currentPos(), totalLength, trackRect, arrowHeight);
    drawTick(p, trackRect, caretY, 2, QColor(248, 81, 73));
}
