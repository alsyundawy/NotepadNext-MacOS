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

#ifndef DOCUMENTEXPORTMANAGER_H
#define DOCUMENTEXPORTMANAGER_H

#include <QMarginsF>
#include <QPageLayout>
#include <QPageSize>
#include <QString>

struct DocumentExportOptions {
    QPageSize::PageSizeId pageSize = QPageSize::A4;
    QPageLayout::Orientation orientation = QPageLayout::Orientation::Portrait;
    QMarginsF margins = QMarginsF(15.0, 15.0, 15.0, 15.0);  // mm
    QString title;
    bool includeCss = true;
    bool darkTheme = false;
    bool usePandocIfAvailable = false;
};

class DocumentExportManager {
public:
    using ExportOptions = DocumentExportOptions;

    static bool isPandocAvailable();

    static QString markdownToHtml(const QString &markdown, const ExportOptions &options = ExportOptions());

    static bool exportMarkdownToHtml(const QString &markdown, const QString &outputPath,
                                     const ExportOptions &options = ExportOptions());

    static bool exportToPdf(const QString &content, bool isMarkdown, const QString &outputPath,
                            const ExportOptions &options = ExportOptions());

    static QString defaultHtmlTemplate(const QString &title, const QString &bodyHtml, bool darkTheme);

    static QString generateCss(bool darkTheme);
};

#endif  // DOCUMENTEXPORTMANAGER_H
