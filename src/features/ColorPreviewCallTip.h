#pragma once
/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef COLORPREVIEWCALLTIP_H
#define COLORPREVIEWCALLTIP_H

#include <QColor>
#include <QString>
#include "../decorators/EditorDecorator.h"

class ScintillaNext;

class ColorPreviewCallTip : public EditorDecorator {
    Q_OBJECT

public:
    enum ColorFormat {
        Hex3,   // #rgb
        Hex4,   // #rgba
        Hex6,   // #rrggbb
        Hex8,   // #rrggbbaa
        Hex0x,  // 0x...
        Rgb,    // rgb(r, g, b)
        Rgba,   // rgba(r, g, b, a)
        Hsl,    // hsl(h, s, l)
        Hsla,   // hsla(h, s, l, a)
        Named   // named css color
    };

    struct ColorMatch {
        bool found{false};
        int startPos{0};
        int endPos{0};
        QString text;
        QColor color;
        ColorFormat format{Hex6};
    };

    explicit ColorPreviewCallTip(ScintillaNext *editor);
    ~ColorPreviewCallTip() override;

    bool triggerColorPreviewOrPick();
    bool showPreviewAt(int pos);
    void dismissPreview();
    bool openColorDialogAndReplace();

    static ColorMatch findColorAt(ScintillaNext *editor, int pos);
    static QString formatColor(const QColor &color, ColorFormat format);

public slots:
    void notify(const Scintilla::NotificationData *pscn) override;

private slots:
    void onDwellStart(int x, int y);
    void onDwellEnd(int x, int y) const;
    void onCallTipClick(int position);

private:
    ColorMatch m_currentMatch;
};

#endif  // COLORPREVIEWCALLTIP_H
