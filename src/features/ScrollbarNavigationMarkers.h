#pragma once
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

#include <QColor>
#include <QPainter>
#include <QRect>

class ScintillaNext;

class ScrollbarNavigationMarkers {
public:
    static void paintNavigationMarkers(QPainter &p, ScintillaNext *editor, const QRect &trackRect,
                                       int arrowHeight = 15);

    static int lineToY(int line, int totalLines, const QRect &trackRect, int arrowHeight = 15);

    static int posToY(qint64 pos, qint64 totalLength, const QRect &trackRect, int arrowHeight = 15);

    static void drawTick(QPainter &p, const QRect &trackRect, int y, int height, const QColor &color);
};
