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

#include "MarkdownPreviewEnhancer.h"

#include <QCoreApplication>
#include <QMap>
#include <QRegularExpression>
#include <QTextDocument>
#include <algorithm>
#include <array>

namespace {
// 30 Cleanrmd / Markdown-Viewer classless themes
const std::vector<MarkdownPreviewEnhancer::ThemeInfo> &getThemesList() {
    static const std::vector<MarkdownPreviewEnhancer::ThemeInfo> s_themes = {
    {QStringLiteral("github"), QStringLiteral("GITHUB"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#1f2328"), QStringLiteral("#0969da"),
     QStringLiteral("#d0d7de"), QStringLiteral("rgba(175,184,193,0.2)"), QStringLiteral("#f6f8fa"),
     QStringLiteral("#1f2328"),
     QStringLiteral("-apple-system, BlinkMacSystemFont, 'Segoe UI', 'Noto Sans', Helvetica, Arial, sans-serif")},

    {QStringLiteral("github-dark"), QStringLiteral("GITHUB-DARK"), true,
     QStringLiteral("#0d1117"), QStringLiteral("#e6edf3"), QStringLiteral("#58a6ff"),
     QStringLiteral("#30363d"), QStringLiteral("rgba(110,118,129,0.4)"), QStringLiteral("#161b22"),
     QStringLiteral("#e6edf3"),
     QStringLiteral("-apple-system, BlinkMacSystemFont, 'Segoe UI', 'Noto Sans', Helvetica, Arial, sans-serif")},

    {QStringLiteral("almond"), QStringLiteral("ALMOND"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#222222"), QStringLiteral("#336699"),
     QStringLiteral("#e0e0e0"), QStringLiteral("#f4f4f4"), QStringLiteral("#fbfbfb"),
     QStringLiteral("#2a52be"),
     QStringLiteral("Helvetica, Arial, sans-serif")},

    {QStringLiteral("awsm"), QStringLiteral("AWSM"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#282828"), QStringLiteral("#0066cc"),
     QStringLiteral("#e0e0e0"), QStringLiteral("#f5f5f5"), QStringLiteral("#fafafa"),
     QStringLiteral("#111111"),
     QStringLiteral("-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen-Sans, Ubuntu, Cantarell, sans-serif")},

    {QStringLiteral("axist"), QStringLiteral("AXIST"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#333333"), QStringLiteral("#2572b4"),
     QStringLiteral("#dddddd"), QStringLiteral("#f7f7f7"), QStringLiteral("#fbfbfb"),
     QStringLiteral("#222222"),
     QStringLiteral("Arial, Helvetica, sans-serif")},

    {QStringLiteral("bamboo"), QStringLiteral("BAMBOO"), false,
     QStringLiteral("#fcfbfa"), QStringLiteral("#37352f"), QStringLiteral("#2e7b54"),
     QStringLiteral("#e3e2de"), QStringLiteral("#f1f1ef"), QStringLiteral("#f7f6f3"),
     QStringLiteral("#1f4e38"),
     QStringLiteral("-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif")},

    {QStringLiteral("bullframe"), QStringLiteral("BULLFRAME"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#2c3e50"), QStringLiteral("#3498db"),
     QStringLiteral("#ecf0f1"), QStringLiteral("#f8f9fa"), QStringLiteral("#f4f6f7"),
     QStringLiteral("#2980b9"),
     QStringLiteral("system-ui, -apple-system, 'Segoe UI', Roboto, sans-serif")},

    {QStringLiteral("holiday"), QStringLiteral("HOLIDAY"), false,
     QStringLiteral("#fffdfa"), QStringLiteral("#40302b"), QStringLiteral("#b54d35"),
     QStringLiteral("#ebdcd6"), QStringLiteral("#f7efe9"), QStringLiteral("#fdf6f2"),
     QStringLiteral("#8f351f"),
     QStringLiteral("Georgia, Palatino, serif")},

    {QStringLiteral("kacit"), QStringLiteral("KACIT"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#222222"), QStringLiteral("#1a5fb4"),
     QStringLiteral("#dedede"), QStringLiteral("#f6f6f6"), QStringLiteral("#f9f9f9"),
     QStringLiteral("#1c2833"),
     QStringLiteral("system-ui, sans-serif")},

    {QStringLiteral("latex"), QStringLiteral("LATEX"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#111111"), QStringLiteral("#002a80"),
     QStringLiteral("#cccccc"), QStringLiteral("#f4f4f4"), QStringLiteral("#f8f8f8"),
     QStringLiteral("#000000"),
     QStringLiteral("'Latin Modern Roman', 'Computer Modern', 'Times New Roman', Times, serif")},

    {QStringLiteral("marx"), QStringLiteral("MARX"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#333333"), QStringLiteral("#0074d9"),
     QStringLiteral("#e5e5e5"), QStringLiteral("#f4f4f4"), QStringLiteral("#fafafa"),
     QStringLiteral("#111111"),
     QStringLiteral("-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif")},

    {QStringLiteral("mini"), QStringLiteral("MINI"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#212121"), QStringLiteral("#0277bd"),
     QStringLiteral("#e0e0e0"), QStringLiteral("#f5f5f5"), QStringLiteral("#f8f9fa"),
     QStringLiteral("#01579b"),
     QStringLiteral("-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif")},

    {QStringLiteral("modest"), QStringLiteral("MODEST"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#444444"), QStringLiteral("#0066ee"),
     QStringLiteral("#eaeaea"), QStringLiteral("#f8f8f8"), QStringLiteral("#fafafa"),
     QStringLiteral("#222222"),
     QStringLiteral("Georgia, 'Times New Roman', serif")},

    {QStringLiteral("new"), QStringLiteral("NEW"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#1d2d35"), QStringLiteral("#0076df"),
     QStringLiteral("#e4ecf1"), QStringLiteral("#f1f5f8"), QStringLiteral("#f7fafc"),
     QStringLiteral("#0c1b22"),
     QStringLiteral("-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif")},

    {QStringLiteral("no-class"), QStringLiteral("NO-CLASS"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#2c3e50"), QStringLiteral("#3498db"),
     QStringLiteral("#e7e7e7"), QStringLiteral("#f9f9f9"), QStringLiteral("#fcfcfc"),
     QStringLiteral("#1a252f"),
     QStringLiteral("system-ui, sans-serif")},

    {QStringLiteral("pico"), QStringLiteral("PICO"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#373c44"), QStringLiteral("#1095c1"),
     QStringLiteral("#e7e9ed"), QStringLiteral("#f3f5f7"), QStringLiteral("#f8f9fa"),
     QStringLiteral("#1b2832"),
     QStringLiteral("system-ui, 'Segoe UI', Roboto, sans-serif")},

    {QStringLiteral("retro"), QStringLiteral("RETRO"), true,
     QStringLiteral("#1a1a1a"), QStringLiteral("#44ff44"), QStringLiteral("#00ffff"),
     QStringLiteral("#22aa22"), QStringLiteral("#0d1f0d"), QStringLiteral("#112611"),
     QStringLiteral("#66ff66"),
     QStringLiteral("'Courier New', Courier, monospace")},

    {QStringLiteral("sakura"), QStringLiteral("SAKURA"), false,
     QStringLiteral("#fdfaf6"), QStringLiteral("#5e504c"), QStringLiteral("#d85d77"),
     QStringLiteral("#eddcd7"), QStringLiteral("#f7eee9"), QStringLiteral("#fcf5f1"),
     QStringLiteral("#b8465e"),
     QStringLiteral("system-ui, -apple-system, BlinkMacSystemFont, sans-serif")},

    {QStringLiteral("sakura-vader"), QStringLiteral("SAKURA-VADER"), true,
     QStringLiteral("#17181c"), QStringLiteral("#dcd3cf"), QStringLiteral("#ff7b95"),
     QStringLiteral("#33353b"), QStringLiteral("#23252a"), QStringLiteral("#1d1e23"),
     QStringLiteral("#ffa3b5"),
     QStringLiteral("system-ui, -apple-system, BlinkMacSystemFont, sans-serif")},

    {QStringLiteral("semantic"), QStringLiteral("SEMANTIC"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#24292e"), QStringLiteral("#4183c4"),
     QStringLiteral("#e1e4e8"), QStringLiteral("#f6f8fa"), QStringLiteral("#fafbfc"),
     QStringLiteral("#1b1f23"),
     QStringLiteral("'Helvetica Neue', Arial, sans-serif")},

    {QStringLiteral("simple"), QStringLiteral("SIMPLE"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#212121"), QStringLiteral("#0070f3"),
     QStringLiteral("#eaeaea"), QStringLiteral("#fafafa"), QStringLiteral("#f7f7f7"),
     QStringLiteral("#000000"),
     QStringLiteral("system-ui, -apple-system, sans-serif")},

    {QStringLiteral("style-sans"), QStringLiteral("STYLE-SANS"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#1a1a1a"), QStringLiteral("#0055aa"),
     QStringLiteral("#dddddd"), QStringLiteral("#f5f5f5"), QStringLiteral("#fafafa"),
     QStringLiteral("#003366"),
     QStringLiteral("system-ui, -apple-system, 'Segoe UI', Roboto, sans-serif")},

    {QStringLiteral("style-serif"), QStringLiteral("STYLE-SERIF"), false,
     QStringLiteral("#fffff8"), QStringLiteral("#222222"), QStringLiteral("#770000"),
     QStringLiteral("#e0ded6"), QStringLiteral("#f5f4ed"), QStringLiteral("#f8f7f0"),
     QStringLiteral("#550000"),
     QStringLiteral("Charter, 'Bitstream Charter', 'Sitka Text', Cambria, Georgia, serif")},

    {QStringLiteral("stylize"), QStringLiteral("STYLIZE"), false,
     QStringLiteral("#fafafa"), QStringLiteral("#333333"), QStringLiteral("#7b1fa2"),
     QStringLiteral("#e0e0e0"), QStringLiteral("#eeeeee"), QStringLiteral("#f5f5f5"),
     QStringLiteral("#4a148c"),
     QStringLiteral("'Avenir Next', 'Segoe UI', sans-serif")},

    {QStringLiteral("superstylin"), QStringLiteral("SUPERSTYLIN"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#111111"), QStringLiteral("#ff3e00"),
     QStringLiteral("#d0d0d0"), QStringLiteral("#f0f0f0"), QStringLiteral("#f7f7f7"),
     QStringLiteral("#cc3200"),
     QStringLiteral("Impact, 'Arial Black', -apple-system, sans-serif")},

    {QStringLiteral("tacit"), QStringLiteral("TACIT"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#222222"), QStringLiteral("#1a5fb4"),
     QStringLiteral("#e0e0e0"), QStringLiteral("#f7f7f7"), QStringLiteral("#fafafa"),
     QStringLiteral("#111111"),
     QStringLiteral("system-ui, sans-serif")},

    {QStringLiteral("vanilla"), QStringLiteral("VANILLA"), false,
     QStringLiteral("#fffefc"), QStringLiteral("#2e2b29"), QStringLiteral("#3f51b5"),
     QStringLiteral("#e8e4df"), QStringLiteral("#f5f2ed"), QStringLiteral("#faf7f2"),
     QStringLiteral("#1a237e"),
     QStringLiteral("system-ui, sans-serif")},

    {QStringLiteral("water"), QStringLiteral("WATER"), false,
     QStringLiteral("#ffffff"), QStringLiteral("#202124"), QStringLiteral("#1a73e8"),
     QStringLiteral("#dadce0"), QStringLiteral("#f1f3f4"), QStringLiteral("#f8f9fa"),
     QStringLiteral("#174ea6"),
     QStringLiteral("system-ui, -apple-system, sans-serif")},

    {QStringLiteral("water-dark"), QStringLiteral("WATER-DARK"), true,
     QStringLiteral("#202124"), QStringLiteral("#e8eaed"), QStringLiteral("#8ab4f8"),
     QStringLiteral("#3c4043"), QStringLiteral("#292a2d"), QStringLiteral("#303134"),
     QStringLiteral("#aecbfa"),
     QStringLiteral("system-ui, -apple-system, sans-serif")},

    {QStringLiteral("writ"), QStringLiteral("WRIT"), false,
     QStringLiteral("#fefefe"), QStringLiteral("#2c2c2c"), QStringLiteral("#1f6feb"),
     QStringLiteral("#e6e6e6"), QStringLiteral("#f6f6f6"), QStringLiteral("#fafafa"),
     QStringLiteral("#1a1a1a"),
     QStringLiteral("Georgia, 'Times New Roman', serif")},
    };
    return s_themes;
}

struct CalloutDef {
    const char *tag;
    const char *icon;
    const char *title;
    const char *colorLight;
    const char *colorDark;
    const char *bgLight;
    const char *bgDark;
};

constexpr std::array<CalloutDef, 5> s_callouts = {{
    {"[!NOTE]", "ℹ️", "Note", "#0969da", "#2f81f7", "rgba(9, 105, 218, 0.08)", "rgba(47, 129, 247, 0.12)"},
    {"[!TIP]", "💡", "Tip", "#1a7f37", "#3fb950", "rgba(26, 127, 55, 0.08)", "rgba(63, 185, 80, 0.12)"},
    {"[!IMPORTANT]", "🟣", "Important", "#8250df", "#a371f7", "rgba(130, 80, 223, 0.08)", "rgba(163, 113, 247, 0.12)"},
    {"[!WARNING]", "⚠️", "Warning", "#9a6700", "#d29922", "rgba(154, 103, 0, 0.08)", "rgba(210, 153, 34, 0.12)"},
    {"[!CAUTION]", "🛑", "Caution", "#cf222e", "#f85149", "rgba(207, 34, 46, 0.08)", "rgba(248, 81, 73, 0.12)"},
}};
}  // namespace

const std::vector<MarkdownPreviewEnhancer::ThemeInfo> &MarkdownPreviewEnhancer::themes() {
    return getThemesList();
}

const MarkdownPreviewEnhancer::ThemeInfo &MarkdownPreviewEnhancer::theme(const QString &themeId) {
    const auto &allThemes = themes();
    const auto it = std::find_if(allThemes.begin(), allThemes.end(),
                                 [&themeId](const ThemeInfo &t) { return t.id.compare(themeId, Qt::CaseInsensitive) == 0; });
    if (it != allThemes.end()) {
        return *it;
    }
    return allThemes.front();
}

QString MarkdownPreviewEnhancer::defaultThemeId(bool isDark) {
    return isDark ? QStringLiteral("github-dark") : QStringLiteral("github");
}

QStringList MarkdownPreviewEnhancer::themeNames() {
    const auto &allThemes = themes();
    QStringList list;
    list.reserve(static_cast<qsizetype>(allThemes.size()));
    std::transform(allThemes.begin(), allThemes.end(), std::back_inserter(list),
                   [](const ThemeInfo &t) { return t.name; });
    return list;
}

bool MarkdownPreviewEnhancer::isDarkTheme(const QString &themeId) {
    return theme(themeId).isDark;
}

bool MarkdownPreviewEnhancer::hasTheme(const QString &themeId) {
    const auto &allThemes = themes();
    return std::any_of(allThemes.begin(), allThemes.end(),
                       [&themeId](const ThemeInfo &t) { return t.id.compare(themeId, Qt::CaseInsensitive) == 0; });
}

bool MarkdownPreviewEnhancer::isMarkdownFile(const QString &fileNameOrPath) {
    if (fileNameOrPath.isEmpty()) {
        return false;
    }
    static const QRegularExpression mdRegex(
        QStringLiteral(R"(\.(?:markdown|mdown|mkdn|md|mkd|mdwn|mdtxt|mdtext|text)(?:#.*|\?.*)?$)"),
        QRegularExpression::CaseInsensitiveOption);
    return mdRegex.match(fileNameOrPath).hasMatch();
}

bool MarkdownPreviewEnhancer::isHtmlFile(const QString &fileNameOrPath) {
    if (fileNameOrPath.isEmpty()) {
        return false;
    }
    static const QRegularExpression htmlRegex(
        QStringLiteral(R"(\.(?:html|htm|xhtml|xht|shtml|phtml|hta)(?:#.*|\?.*)?$)"),
        QRegularExpression::CaseInsensitiveOption);
    return htmlRegex.match(fileNameOrPath).hasMatch();
}

QString MarkdownPreviewEnhancer::sanitizeForPreview(const QString &text) {
    return text.toHtmlEscaped();
}

QString MarkdownPreviewEnhancer::mathAndMermaidCss(bool isDark) {
    const QString blockBg = isDark ? QStringLiteral("#161b22") : QStringLiteral("#f6f8fa");
    const QString border = isDark ? QStringLiteral("#30363d") : QStringLiteral("#d0d7de");
    const QString accent = isDark ? QStringLiteral("#58a6ff") : QStringLiteral("#0969da");
    const QString text = isDark ? QStringLiteral("#e6edf3") : QStringLiteral("#1f2328");
    const QString mathColor = isDark ? QStringLiteral("#79c0ff") : QStringLiteral("#0550ae");
    const QString inlineBg = isDark ? QStringLiteral("rgba(110, 118, 129, 0.2)") : QStringLiteral("rgba(175, 184, 193, 0.2)");

    return QStringLiteral(
               ".mermaid-diagram {\n"
               "    background-color: %1;\n"
               "    border: 1px solid %2;\n"
               "    border-radius: 6px;\n"
               "    margin: 16px 0;\n"
               "    padding: 12px;\n"
               "}\n"
               ".mermaid-header {\n"
               "    font-weight: 600;\n"
               "    color: %3;\n"
               "    font-size: 12px;\n"
               "    margin-bottom: 8px;\n"
               "    letter-spacing: 0.5px;\n"
               "}\n"
               ".mermaid-code {\n"
               "    background: transparent;\n"
               "    color: %4;\n"
               "    font-family: 'SFMono-Regular', Consolas, Menlo, monospace;\n"
               "    font-size: 13px;\n"
               "    margin: 0;\n"
               "    padding: 0;\n"
               "}\n"
               ".math-block {\n"
               "    background-color: %1;\n"
               "    border: 1px solid %2;\n"
               "    border-radius: 6px;\n"
               "    padding: 12px 16px;\n"
               "    margin: 16px 0;\n"
               "    text-align: center;\n"
               "    font-family: KaTeX_Math, 'Cambria Math', STIXGeneral, serif;\n"
               "    font-size: 15px;\n"
               "    color: %5;\n"
               "    overflow-x: auto;\n"
               "}\n"
               ".math-inline {\n"
               "    background-color: %6;\n"
               "    padding: 2px 6px;\n"
               "    border-radius: 4px;\n"
               "    font-family: KaTeX_Math, 'Cambria Math', STIXGeneral, serif;\n"
               "    font-size: 13px;\n"
               "    color: %5;\n"
               "}\n"
               "mark {\n"
               "    background-color: #fff8c5;\n"
               "    color: #24292f;\n"
               "    padding: 0.1em 0.3em;\n"
               "    border-radius: 3px;\n"
               "}\n"
               "ins {\n"
               "    text-decoration: underline;\n"
               "    background-color: rgba(46, 160, 67, 0.15);\n"
               "}\n"
               "del, s {\n"
               "    text-decoration: line-through;\n"
               "    opacity: 0.8;\n"
               "}\n"
               ".task-list-item {\n"
               "    list-style-type: none;\n"
               "    margin-left: -1.2em;\n"
               "}\n"
               ".footnotes {\n"
               "    font-size: 85%;\n"
               "    color: %4;\n"
               "    margin-top: 32px;\n"
               "    border-top: 1px solid %2;\n"
               "    padding-top: 12px;\n"
               "}\n")
        .arg(blockBg, border, accent, text, mathColor, inlineBg);
}

QString MarkdownPreviewEnhancer::themeCss(const QString &themeId, int fontSize) {
    const ThemeInfo &t = theme(themeId);
    const QString sizeStr = QString::number(fontSize > 0 ? fontSize : 13);
    const QString justify = (t.id == QStringLiteral("latex")) ? QStringLiteral("text-align: justify;") : QString();

    return QStringLiteral(
               "body { "
               "  font-family: %1; "
               "  font-size: %2px; line-height: 1.6; color: %3; background-color: %4; margin: 0; padding: 18px 24px; "
               "  %11 "
               "} "
               "h1, h2, h3, h4, h5, h6 { "
               "  font-weight: 600; line-height: 1.25; margin-top: 24px; margin-bottom: 14px; color: %5; "
               "} "
               "h1 { font-size: 2em; padding-bottom: 0.3em; border-bottom: 1px solid %6; } "
               "h2 { font-size: 1.5em; padding-bottom: 0.3em; border-bottom: 1px solid %6; } "
               "h3 { font-size: 1.25em; } "
               "h4 { font-size: 1em; } "
               "h5 { font-size: 0.875em; } "
               "h6 { font-size: 0.85em; opacity: 0.75; } "
               "p { margin-top: 0; margin-bottom: 14px; line-height: 1.6; } "
               "blockquote { "
               "  margin: 0 0 16px 0; padding: 6px 16px; opacity: 0.85; border-left: 4px solid %7; background-color: %8; "
               "  border-radius: 0 4px 4px 0; "
               "} "
               "ul, ol { margin-top: 0; margin-bottom: 14px; padding-left: 24px; } "
               "li { margin-top: 0.3em; } "
               "code { "
               "  font-family: 'SFMono-Regular', Consolas, Menlo, monospace; "
               "  font-size: 88%; padding: 2px 6px; background-color: %9; border-radius: 4px; "
               "} "
               "pre { "
               "  margin-top: 0; margin-bottom: 16px; padding: 14px; line-height: 1.45; "
               "  background-color: %8; border: 1px solid %6; border-radius: 6px; "
               "  font-family: 'SFMono-Regular', Consolas, Menlo, monospace; font-size: 88%; "
               "} "
               "table { border-collapse: collapse; border-spacing: 0; margin-top: 0; margin-bottom: 16px; width: 100%; } "
               "th, td { padding: 8px 14px; border: 1px solid %6; } "
               "th { font-weight: 600; background-color: %8; } "
               "tr:nth-child(2n) { background-color: %8; } "
               "hr { height: 2px; padding: 0; margin: 22px 0; background-color: %6; border: 0; } "
               "a { color: %7; text-decoration: none; font-weight: 500; } "
               "kbd { "
               "  display: inline-block; padding: 2px 6px; font-size: 11px; line-height: 12px; color: %3; "
               "  background-color: %8; border: 1px solid %6; border-radius: 4px; box-shadow: inset 0 -1px 0 %6; "
               "} "
               "img { max-width: 100%; height: auto; border-radius: 4px; }\n")
               .arg(t.fontFamily, sizeStr, t.text, t.bg, t.headingColor, t.border, t.link, t.blockBg, t.codeBg, t.id,
                    justify) +
           mathAndMermaidCss(t.isDark);
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

QString MarkdownPreviewEnhancer::processTaskLists(const QString &content) {
    static const QRegularExpression taskUnchecked(QStringLiteral(R"(^(\s*[-*+]\s+)\[ \]\s+(.*)$)"),
                                                  QRegularExpression::MultilineOption);
    static const QRegularExpression taskChecked(QStringLiteral(R"(^(\s*[-*+]\s+)\[[xX]\]\s+(.*)$)"),
                                                QRegularExpression::MultilineOption);

    QString result = content;
    result.replace(taskUnchecked, QStringLiteral(R"(\1<span style="color:#888;">&#9744;</span> \2)"));
    result.replace(taskChecked, QStringLiteral(R"(\1<span style="color:#2ea44f;">&#9745;</span> \2)"));
    return result;
}

QString MarkdownPreviewEnhancer::processStrikethrough(const QString &content) {
    static const QRegularExpression strikeRegex(QStringLiteral(R"(~~([^~\n]+)~~)"));
    QString result = content;
    result.replace(strikeRegex, QStringLiteral(R"(<del>\1</del>)"));
    return result;
}

QString MarkdownPreviewEnhancer::processMarkAndInsert(const QString &content) {
    static const QRegularExpression markRegex(QStringLiteral(R"(==([^=\n]+)==)"));
    static const QRegularExpression insRegex(QStringLiteral(R"(\+\+([^\+\n]+)\+\+)"));

    QString result = content;
    result.replace(markRegex, QStringLiteral(R"(<mark>\1</mark>)"));
    result.replace(insRegex, QStringLiteral(R"(<ins>\1</ins>)"));
    return result;
}

QString MarkdownPreviewEnhancer::processSubAndSup(const QString &content) {
    static const QRegularExpression subRegex(QStringLiteral(R"((?<![~\w])~([^~\s\n]+)~(?!~))"));
    static const QRegularExpression supRegex(QStringLiteral(R"((?<![\^\w])\^([^\^\s\n]+)\^(?!\^))"));

    QString result = content;
    result.replace(subRegex, QStringLiteral(R"(<sub>\1</sub>)"));
    result.replace(supRegex, QStringLiteral(R"(<sup>\1</sup>)"));
    return result;
}

QString MarkdownPreviewEnhancer::processFootnotes(const QString &content) {
    static const QRegularExpression fnDefRegex(QStringLiteral(R"(^\[\^([a-zA-Z0-9_\-]+)\]:\s*([^\n]+)$)"),
                                               QRegularExpression::MultilineOption);
    static const QRegularExpression fnRefRegex(QStringLiteral(R"(\[\^([a-zA-Z0-9_\-]+)\])"));

    QString result = content;
    QMap<QString, QString> definitions;

    auto it = fnDefRegex.globalMatch(result);
    while (it.hasNext()) {
        const auto match = it.next();
        definitions.insert(match.captured(1), match.captured(2));
    }
    result.remove(fnDefRegex);

    result.replace(fnRefRegex, QStringLiteral(R"(<sup><a href="#fn-\1" id="fnref-\1">[\1]</a></sup>)"));

    if (!definitions.isEmpty()) {
        result += QStringLiteral("\n\n<div class=\"footnotes\"><hr><ol>\n");
        for (auto fnIt = definitions.constBegin(); fnIt != definitions.constEnd(); ++fnIt) {
            result += QStringLiteral("<li id=\"fn-%1\">%2 <a href=\"#fnref-%1\">↩</a></li>\n")
                          .arg(fnIt.key(), fnIt.value());
        }
        result += QStringLiteral("</ol></div>\n");
    }

    return result;
}

QString MarkdownPreviewEnhancer::processHeadingCustomIds(const QString &content) {
    static const QRegularExpression headerIdRegex(
        QStringLiteral(R"(^(#{1,6}\s+.*?)\s*\{#([a-zA-Z0-9_\-]+)\}\s*$)"), QRegularExpression::MultilineOption);
    QString result = content;
    result.replace(headerIdRegex, QStringLiteral(R"(\1 <a id="\2"></a>)"));
    return result;
}

QString MarkdownPreviewEnhancer::enhanceCallouts(const QString &html, bool isDark) {
    QString result = html;
    for (const auto &c : s_callouts) {
        const QString marker = QString::fromUtf8(c.tag);
        qsizetype idx = 0;
        while ((idx = result.indexOf(marker, idx)) != -1) {
            const qsizetype bqStart = result.lastIndexOf(QStringLiteral("<blockquote"), idx);
            if (bqStart == -1 || bqStart < idx - 120) {
                idx += marker.length();
                continue;
            }
            const qsizetype bqEnd = result.indexOf(QStringLiteral("</blockquote>"), idx);
            if (bqEnd == -1) {
                idx += marker.length();
                continue;
            }

            const qsizetype contentStart = idx + marker.length();
            QString inner = result.mid(contentStart, bqEnd - contentStart).trimmed();
            while (inner.startsWith(QStringLiteral("<br />")) || inner.startsWith(QStringLiteral("<br>")) ||
                   inner.startsWith(QStringLiteral("<br/>"))) {
                inner = inner.mid(inner.indexOf('>') + 1).trimmed();
            }

            const QString color = isDark ? QString::fromLatin1(c.colorDark) : QString::fromLatin1(c.colorLight);
            const QString bg = isDark ? QString::fromLatin1(c.bgDark) : QString::fromLatin1(c.bgLight);
            const QString replacement =
                QStringLiteral(
                    "<div style=\"margin: 12px 0; padding: 10px 14px; border-left: 4px solid %1; "
                    "background-color: %2; border-radius: 4px;\">"
                    "<div style=\"font-weight: 600; color: %1; margin-bottom: 4px;\">%3 %4</div>"
                    "<div>%5</div></div>")
                    .arg(color, bg, QString::fromUtf8(c.icon),
                         QCoreApplication::translate("MarkdownPreviewEnhancer", c.title), inner);

            result.replace(bqStart, (bqEnd + 13) - bqStart, replacement);
            idx = bqStart + replacement.length();
        }
    }
    return result;
}

QString MarkdownPreviewEnhancer::compileMarkdownIt(const QString &rawMd, bool isDark, const QString &themeId) {
    // Isolate markdown document theme from application theme
    const bool effectiveDark = hasTheme(themeId) ? isDarkTheme(themeId) : isDark;

    // Step 1: Protect fenced code blocks and mermaid diagrams
    static const QRegularExpression fencedCodeRegex(QStringLiteral(R"(```[\w\-]*\n[\s\S]*?\n```)"));
    static const QRegularExpression inlineCodeRegex(QStringLiteral(R"(`[^`\n]+`)"));

    QString working = rawMd;
    QList<QString> codePlaceholders;

    // First process mermaid diagrams using markdown theme's dark/light setting
    working = processMermaidBlocks(working, effectiveDark);

    // Collect and preserve other code blocks
    auto codeIt = fencedCodeRegex.globalMatch(working);
    qsizetype offset = 0;
    while (codeIt.hasNext()) {
        const auto match = codeIt.next();
        const QString placeholder = QStringLiteral("<!--NPN_FENCE_%1-->").arg(codePlaceholders.size());
        codePlaceholders.append(match.captured(0));

        const qsizetype start = match.capturedStart() + offset;
        const qsizetype len = match.capturedLength();
        working.replace(start, len, placeholder);
        offset += placeholder.length() - len;
    }

    // Step 2: Apply markdown-it extensions
    working = processMathExpressions(working);
    working = processHeadingCustomIds(working);
    working = processTaskLists(working);
    working = processStrikethrough(working);
    working = processMarkAndInsert(working);
    working = processSubAndSup(working);
    working = processFootnotes(working);

    // Step 3: Restore code blocks
    for (qsizetype i = 0; i < codePlaceholders.size(); ++i) {
        const QString placeholder = QStringLiteral("<!--NPN_FENCE_%1-->").arg(i);
        working.replace(placeholder, codePlaceholders.at(i));
    }

    // Step 4: Render through QTextDocument CommonMark parser
    QTextDocument doc;
    doc.setMarkdown(working);
    QString html = doc.toHtml();

    // Step 5: Post-process callouts using markdown theme's dark/light setting
    html = enhanceCallouts(html, effectiveDark);

    return html;
}

QString MarkdownPreviewEnhancer::wrapStandaloneHtml(const QString &bodyHtml, const QString &themeId,
                                                    const QString &title) {
    const ThemeInfo &t = theme(themeId);
    const QString docTitle = title.isEmpty() ? QStringLiteral("NotepadNext Markdown Document") : title.toHtmlEscaped();
    const QString css = themeCss(t.id, 14);

    return QStringLiteral(
               "<!DOCTYPE html>\n"
               "<html lang=\"en\">\n"
               "<head>\n"
               "<meta charset=\"utf-8\">\n"
               "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
               "<title>%1</title>\n"
               "<style>\n"
               "%2\n"
               "</style>\n"
               "</head>\n"
               "<body class=\"_theme-%3\">\n"
               "%4\n"
               "</body>\n"
               "</html>\n")
        .arg(docTitle, css, t.id, bodyHtml);
}
