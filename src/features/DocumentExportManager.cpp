/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "DocumentExportManager.h"
#include <QFile>
#include <QFileInfo>
#include <QPrinter>
#include <QProcess>
#include <QStandardPaths>
#include <QStringConverter>
#include <QTextDocument>
#include <QTextStream>

bool DocumentExportManager::isPandocAvailable() {
    return !QStandardPaths::findExecutable(QStringLiteral("pandoc")).isEmpty();
}

QString DocumentExportManager::generateCss(bool darkTheme) {
    if (darkTheme) {
        return QStringLiteral(
            "body {\n"
            "    background-color: #0d1117;\n"
            "    color: #c9d1d9;\n"
            "    font-family: -apple-system, BlinkMacSystemFont, \"Segoe UI\", Helvetica, Arial, sans-serif;\n"
            "    font-size: 14px;\n"
            "    line-height: 1.6;\n"
            "    padding: 24px;\n"
            "    max-width: 900px;\n"
            "    margin: 0 auto;\n"
            "}\n"
            "h1, h2, h3, h4, h5, h6 { color: #f0f6fc; margin-top: 24px; margin-bottom: 16px; font-weight: 600; }\n"
            "h1 { border-bottom: 1px solid #30363d; padding-bottom: 0.3em; }\n"
            "h2 { border-bottom: 1px solid #30363d; padding-bottom: 0.3em; }\n"
            "a { color: #58a6ff; text-decoration: none; }\n"
            "a:hover { text-decoration: underline; }\n"
            "pre, code {\n"
            "    font-family: SFMono-Regular, Consolas, \"Liberation Mono\", Menlo, monospace;\n"
            "    font-size: 85%;\n"
            "}\n"
            "code {\n"
            "    background-color: rgba(110,118,129,0.4);\n"
            "    padding: 0.2em 0.4em;\n"
            "    border-radius: 6px;\n"
            "}\n"
            "pre {\n"
            "    background-color: #161b22;\n"
            "    padding: 16px;\n"
            "    border-radius: 6px;\n"
            "    overflow: auto;\n"
            "}\n"
            "pre code { background: transparent; padding: 0; }\n"
            "blockquote {\n"
            "    border-left: 0.25em solid #3b434b;\n"
            "    color: #8b949e;\n"
            "    padding: 0 1em;\n"
            "    margin: 0 0 16px 0;\n"
            "}\n"
            "table {\n"
            "    border-collapse: collapse;\n"
            "    width: 100%;\n"
            "    margin-bottom: 16px;\n"
            "}\n"
            "th, td {\n"
            "    border: 1px solid #30363d;\n"
            "    padding: 6px 13px;\n"
            "}\n"
            "tr:nth-child(2n) { background-color: #161b22; }\n"
            "hr { height: 0.25em; padding: 0; margin: 24px 0; background-color: #30363d; border: 0; }\n");
    }

    return QStringLiteral(
        "body {\n"
        "    background-color: #ffffff;\n"
        "    color: #24292e;\n"
        "    font-family: -apple-system, BlinkMacSystemFont, \"Segoe UI\", Helvetica, Arial, sans-serif;\n"
        "    font-size: 14px;\n"
        "    line-height: 1.6;\n"
        "    padding: 24px;\n"
        "    max-width: 900px;\n"
        "    margin: 0 auto;\n"
        "}\n"
        "h1, h2, h3, h4, h5, h6 { color: #1f2328; margin-top: 24px; margin-bottom: 16px; font-weight: 600; }\n"
        "h1 { border-bottom: 1px solid #d0d7de; padding-bottom: 0.3em; }\n"
        "h2 { border-bottom: 1px solid #d0d7de; padding-bottom: 0.3em; }\n"
        "a { color: #0969da; text-decoration: none; }\n"
        "a:hover { text-decoration: underline; }\n"
        "pre, code {\n"
        "    font-family: SFMono-Regular, Consolas, \"Liberation Mono\", Menlo, monospace;\n"
        "    font-size: 85%;\n"
        "}\n"
        "code {\n"
        "    background-color: rgba(175,184,193,0.2);\n"
        "    padding: 0.2em 0.4em;\n"
        "    border-radius: 6px;\n"
        "}\n"
        "pre {\n"
        "    background-color: #f6f8fa;\n"
        "    padding: 16px;\n"
        "    border-radius: 6px;\n"
        "    overflow: auto;\n"
        "}\n"
        "pre code { background: transparent; padding: 0; }\n"
        "blockquote {\n"
        "    border-left: 0.25em solid #d0d7de;\n"
        "    color: #57606a;\n"
        "    padding: 0 1em;\n"
        "    margin: 0 0 16px 0;\n"
        "}\n"
        "table {\n"
        "    border-collapse: collapse;\n"
        "    width: 100%;\n"
        "    margin-bottom: 16px;\n"
        "}\n"
        "th, td {\n"
        "    border: 1px solid #d0d7de;\n"
        "    padding: 6px 13px;\n"
        "}\n"
        "tr:nth-child(2n) { background-color: #f6f8fa; }\n"
        "hr { height: 0.25em; padding: 0; margin: 24px 0; background-color: #d0d7de; border: 0; }\n");
}

QString DocumentExportManager::defaultHtmlTemplate(const QString &title, const QString &bodyHtml, bool darkTheme) {
    const QString css = generateCss(darkTheme);
    return QStringLiteral(
               "<!DOCTYPE html>\n"
               "<html lang=\"en\">\n"
               "<head>\n"
               "    <meta charset=\"utf-8\">\n"
               "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
               "    <title>%1</title>\n"
               "    <style>\n%2\n    </style>\n"
               "</head>\n"
               "<body class=\"markdown-body\">\n"
               "%3\n"
               "</body>\n"
               "</html>\n")
        .arg(title.toHtmlEscaped(), css, bodyHtml);
}

QString DocumentExportManager::markdownToHtml(const QString &markdown, const ExportOptions &options) {
    QTextDocument doc;
    doc.setMarkdown(markdown);
    const QString bodyHtml = doc.toHtml();

    if (!options.includeCss) {
        return bodyHtml;
    }

    const QString docTitle = options.title.isEmpty() ? QStringLiteral("NotepadNext Document") : options.title;
    return defaultHtmlTemplate(docTitle, bodyHtml, options.darkTheme);
}

bool DocumentExportManager::exportMarkdownToHtml(const QString &markdown, const QString &outputPath,
                                                 const ExportOptions &options) {
    if (options.usePandocIfAvailable && isPandocAvailable()) {
        QProcess pandoc;
        pandoc.start(QStringLiteral("pandoc"),
                     {QStringLiteral("-f"), QStringLiteral("markdown"), QStringLiteral("-t"), QStringLiteral("html5"),
                      QStringLiteral("-s"), QStringLiteral("-o"), outputPath});
        if (pandoc.waitForStarted()) {
            pandoc.write(markdown.toUtf8());
            pandoc.closeWriteChannel();
            if (pandoc.waitForFinished(10000) && pandoc.exitCode() == 0) {
                return true;
            }
        }
    }

    const QString html = markdownToHtml(markdown, options);
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << html;
    return true;
}

bool DocumentExportManager::exportToPdf(const QString &content, bool isMarkdown, const QString &outputPath,
                                        const ExportOptions &options) {
    if (options.usePandocIfAvailable && isPandocAvailable()) {
        QProcess pandoc;
        pandoc.start(QStringLiteral("pandoc"),
                     {QStringLiteral("-f"), isMarkdown ? QStringLiteral("markdown") : QStringLiteral("html"),
                      QStringLiteral("-o"), outputPath});
        if (pandoc.waitForStarted()) {
            pandoc.write(content.toUtf8());
            pandoc.closeWriteChannel();
            if (pandoc.waitForFinished(15000) && pandoc.exitCode() == 0) {
                return true;
            }
        }
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(outputPath);
    printer.setPageSize(QPageSize(options.pageSize));
    printer.setPageOrientation(options.orientation);
    printer.setPageMargins(options.margins, QPageLayout::Millimeter);

    QTextDocument doc;
    if (isMarkdown) {
        doc.setMarkdown(content);
    } else {
        doc.setHtml(content);
    }
    doc.setDocumentMargin(0);
    doc.print(&printer);

    return QFile::exists(outputPath);
}
