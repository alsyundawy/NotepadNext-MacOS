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

#ifndef MARKDOWNPREVIEWENHANCER_H
#define MARKDOWNPREVIEWENHANCER_H

#include <QString>

class MarkdownPreviewEnhancer {
public:
    static QString enhanceMarkdown(const QString &rawMd, bool isDark);

    static QString processMermaidBlocks(const QString &content, bool isDark);

    static QString processMathExpressions(const QString &content);

    static QString sanitizeForPreview(const QString &text);

    static QString mathAndMermaidCss(bool isDark);
};

#endif  // MARKDOWNPREVIEWENHANCER_H
