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

#ifndef HIGHLIGHTEDSCROLLBAR_H
#define HIGHLIGHTEDSCROLLBAR_H

#include <QScrollBar>
#include <QPointer>

#include "EditorDecorator.h"

class HighlightedScrollBar;

class HighlightedScrollBarDecorator : public EditorDecorator {
    Q_OBJECT

public:
    explicit HighlightedScrollBarDecorator(ScintillaNext *editor);
    ~HighlightedScrollBarDecorator() override;

public slots:
    void notify(const Scintilla::NotificationData *pscn) override;

private:
    HighlightedScrollBar *scrollBar;
};

class HighlightedScrollBar : public QScrollBar {
    Q_OBJECT

public:
    explicit HighlightedScrollBar(ScintillaNext *editor, Qt::Orientation orientation, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawMarker(QPainter &p, int marker, sptr_t totalLines, int trackH, int arrowH) const;
    void drawIndicator(QPainter &p, int indicator, sptr_t totalLines, int trackH, int arrowH) const;
    void drawCursors(QPainter &p, sptr_t totalLines, int trackH, int arrowH) const;

    void drawTickMark(QPainter &p, int y, int height, const QColor &color, int arrowH) const;

    [[nodiscard]] int posToScrollBarY(sptr_t pos, sptr_t totalLines, int trackH) const;
    [[nodiscard]] static int lineToScrollBarY(sptr_t line, sptr_t totalLines, int trackH);
    [[nodiscard]] int scrollbarArrowHeight() const;

    ScintillaNext *editor;
    int smartHighlighterIndicator;
};

#endif  // HIGHLIGHTEDSCROLLBAR_H
