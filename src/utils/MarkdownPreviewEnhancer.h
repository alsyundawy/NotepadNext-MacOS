#pragma once
/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2019-2026 Justin Dailey
 * Copyright (C) 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUTION | https://www.alsyundawy.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef MARKDOWNPREVIEWENHANCER_H
#define MARKDOWNPREVIEWENHANCER_H

#include <QString>
#include <QStringList>
#include <vector>

class MarkdownPreviewEnhancer {
public:
    struct ThemeInfo {
        QString id;
        QString name;
        bool isDark{false};
        QString bg;
        QString text;
        QString link;
        QString border;
        QString codeBg;
        QString blockBg;
        QString headingColor;
        QString fontFamily;
    };

    // 30 Curated Themes matching cleanrmd / markdown-viewer specification
    static const std::vector<ThemeInfo> &themes();
    static const ThemeInfo &theme(const QString &themeId);
    static QString defaultThemeId(bool isDark);
    static QStringList themeNames();
    static bool isDarkTheme(const QString &themeId);
    static bool hasTheme(const QString &themeId);

    // File type detection regexes matching markdown-viewer specifications
    static bool isMarkdownFile(const QString &fileNameOrPath);
    static bool isHtmlFile(const QString &fileNameOrPath);

    // Markdown-it compilation pipeline (CommonMark + GFM + Extensions)
    static QString compileMarkdownIt(const QString &rawMd, bool isDark = false, const QString &themeId = QStringLiteral("github"));
    static QString wrapStandaloneHtml(const QString &bodyHtml, const QString &themeId, const QString &title = QString());

    // Component enhancers
    static QString processMermaidBlocks(const QString &content, bool isDark);
    static QString processMathExpressions(const QString &content);
    static QString processTaskLists(const QString &content);
    static QString processStrikethrough(const QString &content);
    static QString processMarkAndInsert(const QString &content);
    static QString processSubAndSup(const QString &content);
    static QString processFootnotes(const QString &content);
    static QString processHeadingCustomIds(const QString &content);
    static QString enhanceCallouts(const QString &html, bool isDark);

    static QString sanitizeForPreview(const QString &text);
    static QString themeCss(const QString &themeId, int fontSize = 13);
    static QString mathAndMermaidCss(bool isDark);
};

#endif  // MARKDOWNPREVIEWENHANCER_H
