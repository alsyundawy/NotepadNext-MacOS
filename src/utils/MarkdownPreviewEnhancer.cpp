/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "MarkdownPreviewEnhancer.h"
#include <QRegularExpression>

QString MarkdownPreviewEnhancer::sanitizeForPreview(const QString &text) {
    return text.toHtmlEscaped();
}

QString MarkdownPreviewEnhancer::mathAndMermaidCss(bool isDark) {
    if (isDark) {
        return QStringLiteral(
            ".mermaid-diagram {\n"
            "    background-color: #161b22;\n"
            "    border: 1px solid #30363d;\n"
            "    border-radius: 6px;\n"
            "    margin: 16px 0;\n"
            "    padding: 12px;\n"
            "}\n"
            ".mermaid-header {\n"
            "    font-weight: 600;\n"
            "    color: #58a6ff;\n"
            "    font-size: 12px;\n"
            "    margin-bottom: 8px;\n"
            "    letter-spacing: 0.5px;\n"
            "}\n"
            ".mermaid-code {\n"
            "    background: transparent;\n"
            "    color: #e6edf3;\n"
            "    font-family: SFMono-Regular, Consolas, Menlo, monospace;\n"
            "    font-size: 13px;\n"
            "    margin: 0;\n"
            "    padding: 0;\n"
            "}\n"
            ".math-block {\n"
            "    background-color: #161b22;\n"
            "    border: 1px solid #30363d;\n"
            "    border-radius: 6px;\n"
            "    padding: 12px 16px;\n"
            "    margin: 16px 0;\n"
            "    text-align: center;\n"
            "    font-family: KaTeX_Math, \"Cambria Math\", STIXGeneral, serif;\n"
            "    font-size: 15px;\n"
            "    color: #79c0ff;\n"
            "    overflow-x: auto;\n"
            "}\n"
            ".math-inline {\n"
            "    background-color: rgba(110, 118, 129, 0.2);\n"
            "    padding: 2px 6px;\n"
            "    border-radius: 4px;\n"
            "    font-family: KaTeX_Math, \"Cambria Math\", STIXGeneral, serif;\n"
            "    font-size: 13px;\n"
            "    color: #79c0ff;\n"
            "}\n");
    }

    return QStringLiteral(
        ".mermaid-diagram {\n"
        "    background-color: #f6f8fa;\n"
        "    border: 1px solid #d0d7de;\n"
        "    border-radius: 6px;\n"
        "    margin: 16px 0;\n"
        "    padding: 12px;\n"
        "}\n"
        ".mermaid-header {\n"
        "    font-weight: 600;\n"
        "    color: #0969da;\n"
        "    font-size: 12px;\n"
        "    margin-bottom: 8px;\n"
        "    letter-spacing: 0.5px;\n"
        "}\n"
        ".mermaid-code {\n"
        "    background: transparent;\n"
        "    color: #1f2328;\n"
        "    font-family: SFMono-Regular, Consolas, Menlo, monospace;\n"
        "    font-size: 13px;\n"
        "    margin: 0;\n"
        "    padding: 0;\n"
        "}\n"
        ".math-block {\n"
        "    background-color: #f6f8fa;\n"
        "    border: 1px solid #d0d7de;\n"
        "    border-radius: 6px;\n"
        "    padding: 12px 16px;\n"
        "    margin: 16px 0;\n"
        "    text-align: center;\n"
        "    font-family: KaTeX_Math, \"Cambria Math\", STIXGeneral, serif;\n"
        "    font-size: 15px;\n"
        "    color: #0550ae;\n"
        "    overflow-x: auto;\n"
        "}\n"
        ".math-inline {\n"
        "    background-color: rgba(175, 184, 193, 0.2);\n"
        "    padding: 2px 6px;\n"
        "    border-radius: 4px;\n"
        "    font-family: KaTeX_Math, \"Cambria Math\", STIXGeneral, serif;\n"
        "    font-size: 13px;\n"
        "    color: #0550ae;\n"
        "}\n");
}

QString MarkdownPreviewEnhancer::processMermaidBlocks(const QString &content, bool isDark) {
    (void)isDark;
    static const QRegularExpression mermaidRegex(QStringLiteral("```mermaid\\s*\\n([\\s\\S]*?)\\n```"),
                                                 QRegularExpression::CaseInsensitiveOption);

    QString result = content;
    auto matchIterator = mermaidRegex.globalMatch(content);
    qsizetype offset = 0;

    while (matchIterator.hasNext()) {
        const QRegularExpressionMatch match = matchIterator.next();
        const QString diagramSource = match.captured(1).trimmed();
        const QString escapedSource = sanitizeForPreview(diagramSource);

        const QString replacement = QStringLiteral(
                                        "<div class=\"mermaid-diagram\">\n"
                                        "<div class=\"mermaid-header\">📊 Mermaid Diagram</div>\n"
                                        "<pre class=\"mermaid-code\">%1</pre>\n"
                                        "</div>\n")
                                        .arg(escapedSource);

        const qsizetype start = match.capturedStart() + offset;
        const qsizetype length = match.capturedLength();
        result.replace(start, length, replacement);
        offset += replacement.length() - length;
    }

    return result;
}

QString MarkdownPreviewEnhancer::processMathExpressions(const QString &content) {
    QString result = content;

    static const QRegularExpression blockMathRegex(QStringLiteral("\\$\\$\\s*\\n?([\\s\\S]*?)\\n?\\s*\\$\\$"));

    auto blockIt = blockMathRegex.globalMatch(result);
    qsizetype offset = 0;
    while (blockIt.hasNext()) {
        const QRegularExpressionMatch match = blockIt.next();
        const QString mathSource = match.captured(1).trimmed();
        const QString escaped = sanitizeForPreview(mathSource);

        const QString replacement =
            QStringLiteral("<div class=\"math-block\"><span class=\"math-tex\">%1</span></div>").arg(escaped);

        const qsizetype start = match.capturedStart() + offset;
        const qsizetype length = match.capturedLength();
        result.replace(start, length, replacement);
        offset += replacement.length() - length;
    }

    static const QRegularExpression inlineMathRegex(
        QStringLiteral("(?<![\\\\a-zA-Z0-9])\\$(?![0-9\\s\\$])([^\\$\\n]+?)(?<![\\\\\\s])\\$(?![a-zA-Z0-9])"));

    auto inlineIt = inlineMathRegex.globalMatch(result);
    offset = 0;
    while (inlineIt.hasNext()) {
        const QRegularExpressionMatch match = inlineIt.next();
        const QString mathSource = match.captured(1).trimmed();

        bool isNumber = false;
        (void)mathSource.toDouble(&isNumber);
        if (isNumber) {
            continue;
        }

        const QString escaped = sanitizeForPreview(mathSource);
        const QString replacement = QStringLiteral("<span class=\"math-inline\">%1</span>").arg(escaped);

        const qsizetype start = match.capturedStart() + offset;
        const qsizetype length = match.capturedLength();
        result.replace(start, length, replacement);
        offset += replacement.length() - length;
    }

    return result;
}

QString MarkdownPreviewEnhancer::enhanceMarkdown(const QString &rawMd, bool isDark) {
    QString processed = processMermaidBlocks(rawMd, isDark);
    processed = processMathExpressions(processed);
    return processed;
}
