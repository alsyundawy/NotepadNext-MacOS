/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "MarkdownSafePreviewDock.h"
#include "../ScintillaNext.h"
#include "../dialogs/MainWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include "../features/DocumentExportManager.h"
#include "../features/ThemeManager.h"
#include "../features/ThemePaletteData.h"
#include "../dialogs/ExportOptionsDialog.h"
#include "../utils/MarkdownPreviewEnhancer.h"
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSettings>
#include <QTemporaryFile>
#include <QTextDocument>
#include <QVBoxLayout>
#include <algorithm>
#include <array>

MarkdownSafePreviewDock::MarkdownSafePreviewDock(MainWindow *parent)
    : QDockWidget(tr("HTML & Markdown Live Preview"), parent),
      browser(new QTextBrowser(this)),
      debounceTimer(new QTimer(this)),
      comboFormat(new QComboBox(this)),
      comboTheme(new QComboBox(this)),
      lblFormatStatus(new QLabel(this)),
      lblStats(new QLabel(this)),
      btnOpenBrowser(new QPushButton(this)),
      btnCopyHtml(new QPushButton(this)),
      btnExportHtml(new QPushButton(this)),
      btnExportPdf(new QPushButton(this)),
      btnRefresh(new QPushButton(this)),
      btnZoomIn(new QPushButton(this)),
      btnZoomOut(new QPushButton(this)),
      btnResetZoom(new QPushButton(QStringLiteral("100%"), this)) {
    setObjectName(QStringLiteral("MarkdownSafePreviewDock"));

    const QSettings settings;
    autoShowEnabled = settings.value(QStringLiteral("Gui/AutoShowMarkdownHtmlPreview"), false).toBool();
    if (const int savedFormat = settings.value(QStringLiteral("Gui/PreviewFormat"), 0).toInt();
        savedFormat >= 0 && savedFormat <= 2) {
        currentFormat = static_cast<PreviewFormat>(savedFormat);
    }

    const bool isDark = ThemeManager::instance()->isDarkActive();
    const QString defaultTheme = MarkdownPreviewEnhancer::defaultThemeId(isDark);
    currentTheme = settings.value(QStringLiteral("Preview/MarkdownTheme"), defaultTheme).toString();
    if (currentTheme.isEmpty() || !MarkdownPreviewEnhancer::hasTheme(currentTheme)) {
        currentTheme = defaultTheme;
    }

    setupUi();

    debounceTimer->setSingleShot(true);
    debounceTimer->setInterval(200);
    connect(debounceTimer, &QTimer::timeout, this, &MarkdownSafePreviewDock::renderPreview);

    connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &MarkdownSafePreviewDock::applyGitHubTheme);
    connect(ThemeManager::instance(), &ThemeManager::paletteChanged, this, &MarkdownSafePreviewDock::applyGitHubTheme);
    applyGitHubTheme();
}

void MarkdownSafePreviewDock::setupUi() {
    auto *container = new QWidget(this);
    container->setObjectName(QStringLiteral("previewContainer"));
    auto *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    auto *topBar = new QWidget(container);
    topBar->setObjectName(QStringLiteral("previewTopBar"));
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(6, 4, 6, 4);
    topLayout->setSpacing(5);

    comboFormat->addItem(tr("Auto"), static_cast<int>(PreviewFormat::Auto));
    comboFormat->addItem(tr("Markdown"), static_cast<int>(PreviewFormat::Markdown));
    comboFormat->addItem(tr("HTML"), static_cast<int>(PreviewFormat::Html));
    comboFormat->setCurrentIndex(static_cast<int>(currentFormat));
    comboFormat->setToolTip(tr("Select preview rendering format (Auto detects Markdown or HTML)"));
    connect(comboFormat, &QComboBox::currentIndexChanged, this, &MarkdownSafePreviewDock::onFormatComboChanged);
    topLayout->addWidget(comboFormat);

    const auto &themes = MarkdownPreviewEnhancer::themes();
    for (const auto &t : themes) {
        comboTheme->addItem(t.name, t.id);
    }
    const int themeIdx = comboTheme->findData(currentTheme);
    if (themeIdx >= 0) {
        comboTheme->setCurrentIndex(themeIdx);
    }
    comboTheme->setToolTip(tr("Markdown Theme (30 curated themes from markdown-viewer)"));
    connect(comboTheme, &QComboBox::currentIndexChanged, this, &MarkdownSafePreviewDock::onThemeComboChanged);
    topLayout->addWidget(comboTheme);

    lblFormatStatus->setObjectName(QStringLiteral("lblFormatStatus"));
    lblFormatStatus->setText(tr("Auto"));
    topLayout->addWidget(lblFormatStatus);

    lblStats->setObjectName(QStringLiteral("lblPreviewStats"));
    lblStats->setText(tr("0 words • 0 min read"));
    topLayout->addWidget(lblStats);

    topLayout->addStretch(1);

    btnZoomOut->setIcon(QIcon(QStringLiteral(":/icons/zoomOut.svg")));
    btnZoomOut->setIconSize(QSize(14, 14));
    btnZoomOut->setToolTip(tr("Zoom Out"));
    btnZoomOut->setFixedSize(28, 24);
    btnZoomOut->setCursor(Qt::PointingHandCursor);
    connect(btnZoomOut, &QPushButton::clicked, this, &MarkdownSafePreviewDock::zoomOut);
    topLayout->addWidget(btnZoomOut);

    btnResetZoom->setText(QStringLiteral("100%"));
    btnResetZoom->setToolTip(tr("Reset Zoom (100%)"));
    btnResetZoom->setFixedHeight(24);
    btnResetZoom->setMinimumWidth(48);
    btnResetZoom->setCursor(Qt::PointingHandCursor);
    connect(btnResetZoom, &QPushButton::clicked, this, &MarkdownSafePreviewDock::resetZoom);
    topLayout->addWidget(btnResetZoom);

    btnZoomIn->setIcon(QIcon(QStringLiteral(":/icons/zoomIn.svg")));
    btnZoomIn->setIconSize(QSize(14, 14));
    btnZoomIn->setToolTip(tr("Zoom In"));
    btnZoomIn->setFixedSize(28, 24);
    btnZoomIn->setCursor(Qt::PointingHandCursor);
    connect(btnZoomIn, &QPushButton::clicked, this, &MarkdownSafePreviewDock::zoomIn);
    topLayout->addWidget(btnZoomIn);

    btnRefresh->setIcon(QIcon(QStringLiteral(":/icons/arrow_refresh.svg")));
    btnRefresh->setIconSize(QSize(14, 14));
    btnRefresh->setToolTip(tr("Refresh Preview"));
    btnRefresh->setFixedSize(28, 24);
    btnRefresh->setCursor(Qt::PointingHandCursor);
    connect(btnRefresh, &QPushButton::clicked, this, &MarkdownSafePreviewDock::renderPreview);
    topLayout->addWidget(btnRefresh);

    btnCopyHtml->setIcon(QIcon(QStringLiteral(":/icons/copy.svg")));
    btnCopyHtml->setIconSize(QSize(14, 14));
    btnCopyHtml->setToolTip(tr("Copy Formatted HTML to Clipboard"));
    btnCopyHtml->setFixedSize(28, 24);
    btnCopyHtml->setCursor(Qt::PointingHandCursor);
    connect(btnCopyHtml, &QPushButton::clicked, this, &MarkdownSafePreviewDock::copyHtml);
    topLayout->addWidget(btnCopyHtml);

    btnExportHtml->setIcon(QIcon(QStringLiteral(":/icons/saveAll.svg")));
    btnExportHtml->setIconSize(QSize(14, 14));
    btnExportHtml->setToolTip(tr("Export as Standalone HTML File..."));
    btnExportHtml->setFixedSize(28, 24);
    btnExportHtml->setCursor(Qt::PointingHandCursor);
    connect(btnExportHtml, &QPushButton::clicked, this, &MarkdownSafePreviewDock::exportHtml);
    topLayout->addWidget(btnExportHtml);

    btnExportPdf->setText(QStringLiteral("PDF"));
    btnExportPdf->setToolTip(tr("Export Document to PDF..."));
    btnExportPdf->setFixedHeight(24);
    btnExportPdf->setCursor(Qt::PointingHandCursor);
    connect(btnExportPdf, &QPushButton::clicked, this, &MarkdownSafePreviewDock::exportPdf);
    topLayout->addWidget(btnExportPdf);

    btnOpenBrowser->setIcon(QIcon(QStringLiteral(":/icons/newfile.svg")));
    btnOpenBrowser->setIconSize(QSize(14, 14));
    btnOpenBrowser->setText(QStringLiteral("Browser"));
    btnOpenBrowser->setToolTip(tr("Open in Default Web Browser"));
    btnOpenBrowser->setFixedHeight(24);
    btnOpenBrowser->setCursor(Qt::PointingHandCursor);
    connect(btnOpenBrowser, &QPushButton::clicked, this, &MarkdownSafePreviewDock::openInBrowser);
    topLayout->addWidget(btnOpenBrowser);

    mainLayout->addWidget(topBar);

    browser->setObjectName(QStringLiteral("previewSafeBrowser"));
    browser->setOpenExternalLinks(false);
    browser->setOpenLinks(false);
    connect(browser, &QTextBrowser::anchorClicked, this, &MarkdownSafePreviewDock::handleAnchorClicked);
    connect(browser->verticalScrollBar(), &QScrollBar::valueChanged, this,
            &MarkdownSafePreviewDock::syncScrollFromBrowser);

    applyGitHubTheme();

    mainLayout->addWidget(browser);
    setWidget(container);

    baseFontSize = browser->font().pointSize();
    if (baseFontSize <= 0) {
        baseFontSize = 13;
    }
}

void MarkdownSafePreviewDock::setActiveEditor(ScintillaNext *editor) {
    if (currentEditor == editor) {
        scheduleRender();
        return;
    }

    if (currentEditor) {
        disconnect(currentEditor, nullptr, this, nullptr);
    }

    currentEditor = editor;

    if (currentEditor) {
        connect(currentEditor, &ScintillaNext::notifyChange, this, &MarkdownSafePreviewDock::scheduleRender);
        connect(currentEditor, &ScintillaNext::updateUi, this, &MarkdownSafePreviewDock::syncScrollFromEditor);
    }

    scheduleRender();
}

void MarkdownSafePreviewDock::scheduleRender() {
    if (isVisible()) {
        debounceTimer->start();
    }
}

MarkdownSafePreviewDock::PreviewFormat MarkdownSafePreviewDock::detectFormat(const ScintillaNext *editor) {
    using enum PreviewFormat;

    if (!editor) {
        return Markdown;
    }

    const QString lang = editor->languageName.toLower();
    if (lang.contains(QStringLiteral("html")) || lang.contains(QStringLiteral("xml"))) {
        return Html;
    }
    if (lang.contains(QStringLiteral("markdown"))) {
        return Markdown;
    }

    const QString fileName = editor->getFileInfo().fileName();
    if (MarkdownPreviewEnhancer::isHtmlFile(fileName)) {
        return Html;
    }
    if (MarkdownPreviewEnhancer::isMarkdownFile(fileName)) {
        return Markdown;
    }

    if (const QString snippet = QString::fromUtf8(editor->get_text_range(0, 400)).trimmed().toLower();
        snippet.startsWith(QStringLiteral("<!doctype html")) || snippet.startsWith(QStringLiteral("<html")) ||
        snippet.contains(QStringLiteral("<head>")) || snippet.contains(QStringLiteral("<body>")) ||
        snippet.contains(QStringLiteral("</div>")) || snippet.contains(QStringLiteral("</p>"))) {
        return Html;
    }

    return Markdown;
}

void MarkdownSafePreviewDock::changeEvent(QEvent *event) {
    if (event->type() == QEvent::PaletteChange) {
        applyGitHubTheme();
        renderPreview();
    }
    QDockWidget::changeEvent(event);
}

void MarkdownSafePreviewDock::applyGitHubTheme() {
    if (m_isUpdatingTheme) {
        return;
    }
    m_isUpdatingTheme = true;

    const bool isDark = ThemeManager::instance()->isDarkActive();
    const ThemePalette *pal = ThemeManager::instance()->currentPalette();
    const QString accent = pal ? QString::fromLatin1(pal->blue) : QStringLiteral("#0969da");
    const QString dockBg = isDark ? QStringLiteral("#161b22") : QStringLiteral("#f6f8fa");
    const QString textPri = isDark ? QStringLiteral("#e6edf3") : QStringLiteral("#1f2328");
    const QString textSec = isDark ? QStringLiteral("#8b949e") : QStringLiteral("#57606a");
    const QString bgColor = isDark ? QStringLiteral("#0d1117") : QStringLiteral("#ffffff");
    const QString border = isDark ? QStringLiteral("#30363d") : QStringLiteral("#d0d7de");
    const QString btnBg = isDark ? QStringLiteral("#21262d") : QStringLiteral("#f6f8fa");
    const QString badgeBg =
        isDark ? QStringLiteral("rgba(56, 139, 253, 0.15)") : QStringLiteral("rgba(9, 105, 218, 0.1)");
    const QString selColor =
        isDark ? QStringLiteral("rgba(56, 139, 253, 0.4)") : QStringLiteral("rgba(9, 105, 218, 0.25)");

    const QString dockStyle = QStringLiteral(
                                  "QDockWidget {"
                                  "  background-color: %1;"
                                  "  color: %2;"
                                  "}"
                                  "QDockWidget::title {"
                                  "  background-color: %1;"
                                  "  color: %2;"
                                  "  padding: 6px 10px;"
                                  "  border-bottom: 2px solid %3;"
                                  "  font-weight: 600;"
                                  "  font-size: 11px;"
                                  "}"
                                  "QWidget#previewTopBar {"
                                  "  background-color: %1;"
                                  "  border: 1px solid %4;"
                                  "  border-radius: 6px;"
                                  "}"
                                  "QWidget#previewTopBar QComboBox {"
                                  "  background-color: %5;"
                                  "  color: %2;"
                                  "  border: 1px solid %4;"
                                  "  border-radius: 4px;"
                                  "  padding: 2px 6px;"
                                  "  font-size: 11px;"
                                  "}"
                                  "QWidget#previewTopBar QComboBox:hover {"
                                  "  border-color: %3;"
                                  "}"
                                  "QLabel#lblFormatStatus {"
                                  "  font-size: 10px;"
                                  "  font-weight: 600;"
                                  "  padding: 2px 8px;"
                                  "  border-radius: 10px;"
                                  "  background-color: %6;"
                                  "  color: %3;"
                                  "  border: 1px solid %3;"
                                  "}"
                                  "QLabel#lblPreviewStats {"
                                  "  font-size: 10px;"
                                  "  color: %7;"
                                  "  padding: 2px 6px;"
                                  "}"
                                  "QWidget#previewTopBar QPushButton {"
                                  "  background-color: %8;"
                                  "  color: %2;"
                                  "  border: 1px solid %4;"
                                  "  border-radius: 4px;"
                                  "  font-size: 11px;"
                                  "  padding: 2px 5px;"
                                  "  min-height: 20px;"
                                  "}"
                                  "QWidget#previewTopBar QPushButton:hover {"
                                  "  border-color: %3;"
                                  "  color: %3;"
                                  "}"
                                  "QWidget#previewTopBar QPushButton:pressed {"
                                  "  background-color: %3;"
                                  "  color: #ffffff;"
                                  "}")
                                  .arg(dockBg, textPri, accent, border, bgColor, badgeBg, textSec, btnBg);
    setStyleSheet(dockStyle);

    const auto effectiveFormat = (currentFormat == PreviewFormat::Auto) ? detectFormat(currentEditor) : currentFormat;
    QString browserBg;
    QString browserFg;
    if (effectiveFormat == PreviewFormat::Html) {
        browserBg = isDark ? QStringLiteral("#0d1117") : QStringLiteral("#ffffff");
        browserFg = isDark ? QStringLiteral("#e6edf3") : QStringLiteral("#1f2328");
    } else {
        const auto &themeInfo = MarkdownPreviewEnhancer::theme(currentTheme);
        browserBg = themeInfo.bg;
        browserFg = themeInfo.text;
    }

    browser->setStyleSheet(
        QStringLiteral(
            "#previewSafeBrowser { background-color: %1; color: %2; border: none; selection-background-color: %3; }")
            .arg(browserBg, browserFg, selColor));

    QPalette browserPal = browser->palette();
    browserPal.setColor(QPalette::Base, QColor(browserBg));
    browserPal.setColor(QPalette::Text, QColor(browserFg));
    browserPal.setColor(QPalette::Window, QColor(browserBg));
    browserPal.setColor(QPalette::WindowText, QColor(browserFg));
    if (effectiveFormat != PreviewFormat::Html) {
        const auto &themeInfo = MarkdownPreviewEnhancer::theme(currentTheme);
        browserPal.setColor(QPalette::Link, QColor(themeInfo.link));
    }
    browser->setPalette(browserPal);

    browser->document()->setDefaultStyleSheet(
        (effectiveFormat == PreviewFormat::Html)
            ? buildGitHubStyleSheet(isDark)
            : MarkdownPreviewEnhancer::themeCss(currentTheme, baseFontSize));
    m_isUpdatingTheme = false;
}

QString MarkdownSafePreviewDock::buildGitHubStyleSheet(bool isDark) const {
    const QString textColor = isDark ? QStringLiteral("#e6edf3") : QStringLiteral("#1f2328");
    const QString bgColor = isDark ? QStringLiteral("#0d1117") : QStringLiteral("#ffffff");
    const QString borderColor = isDark ? QStringLiteral("#30363d") : QStringLiteral("#d8dee4");
    const QString mutedColor = isDark ? QStringLiteral("#848d97") : QStringLiteral("#656d76");
    const QString codeBg =
        isDark ? QStringLiteral("rgba(110, 118, 129, 0.4)") : QStringLiteral("rgba(175, 184, 193, 0.2)");
    const QString blockBg = isDark ? QStringLiteral("#161b22") : QStringLiteral("#f6f8fa");
    const QString linkColor = isDark ? QStringLiteral("#2f81f7") : QStringLiteral("#0969da");

    return QStringLiteral(
               "body { "
               "  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Noto Sans', Helvetica, Arial, "
               "sans-serif; "
               "  font-size: %1px; line-height: 1.6; color: %2; background-color: %3; margin: 0; padding: 18px 24px; "
               "} "
               "h1, h2, h3, h4, h5, h6 { "
               "  font-weight: 600; line-height: 1.25; margin-top: 24px; margin-bottom: 16px; color: %2; "
               "} "
               "h1 { font-size: 2em; padding-bottom: 0.3em; border-bottom: 1px solid %4; } "
               "h2 { font-size: 1.5em; padding-bottom: 0.3em; border-bottom: 1px solid %4; } "
               "h3 { font-size: 1.25em; } "
               "h4 { font-size: 1em; } "
               "h5 { font-size: 0.875em; } "
               "h6 { font-size: 0.85em; color: %5; } "
               "p { margin-top: 0; margin-bottom: 16px; line-height: 1.6; } "
               "blockquote { "
               "  margin: 0 0 16px 0; padding: 6px 16px; color: %5; border-left: 4px solid %4; background-color: %7; "
               "border-radius: 0 4px 4px 0; "
               "} "
               "ul, ol { margin-top: 0; margin-bottom: 16px; padding-left: 2em; } "
               "li { margin-top: 0.3em; } "
               "code { "
               "  font-family: 'JetBrains Mono', 'SFMono-Regular', Consolas, 'Liberation Mono', Menlo, monospace; "
               "  font-size: 85%; padding: 0.2em 0.4em; background-color: %6; border-radius: 6px; "
               "} "
               "pre { "
               "  margin-top: 0; margin-bottom: 16px; padding: 16px; line-height: 1.45; "
               "  background-color: %7; border: 1px solid %4; border-radius: 8px; "
               "  font-family: 'JetBrains Mono', 'SFMono-Regular', Consolas, 'Liberation Mono', Menlo, monospace; "
               "font-size: 85%; "
               "} "
               "table { border-collapse: collapse; border-spacing: 0; margin-top: 0; margin-bottom: 16px; width: 100%; "
               "} "
               "th, td { padding: 8px 14px; border: 1px solid %4; } "
               "th { font-weight: 600; background-color: %7; } "
               "tr:nth-child(2n) { background-color: %7; } "
               "hr { height: 2px; padding: 0; margin: 24px 0; background-color: %4; border: 0; } "
               "a { color: %8; text-decoration: none; font-weight: 500; } "
               "kbd { "
               "  display: inline-block; padding: 3px 6px; font-size: 11px; line-height: 10px; color: %2; "
               "  background-color: %7; border: 1px solid %4; border-radius: 6px; box-shadow: inset 0 -1px 0 %4; "
               "} "
               "img { max-width: 100%; height: auto; border-radius: 6px; }\n")
               .arg(QString::number(baseFontSize), textColor, bgColor, borderColor, mutedColor, codeBg, blockBg,
                    linkColor) +
           MarkdownPreviewEnhancer::mathAndMermaidCss(isDark);
}

QString MarkdownSafePreviewDock::generateFullHtmlDocument(const QString &bodyHtml, bool isDark) const {
    const PreviewFormat effectiveFormat =
        (currentFormat == PreviewFormat::Auto) ? detectFormat(currentEditor) : currentFormat;
    const QString title = currentEditor ? currentEditor->getFileInfo().fileName() : tr("NotepadNext Preview");

    if (effectiveFormat == PreviewFormat::Html) {
        const QString css = buildGitHubStyleSheet(isDark);
        return QStringLiteral(
                   "<!DOCTYPE html>\n"
                   "<html>\n"
                   "<head>\n"
                   "<meta charset=\"utf-8\">\n"
                   "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
                   "<title>%1</title>\n"
                   "<style>\n"
                   "%2\n"
                   "</style>\n"
                   "</head>\n"
                   "<body>\n"
                   "%3\n"
                   "</body>\n"
                   "</html>\n")
            .arg(title.toHtmlEscaped(), css, bodyHtml);
    }

    return MarkdownPreviewEnhancer::wrapStandaloneHtml(bodyHtml, currentTheme, title);
}

namespace {
struct CalloutDef {
    const char *tag;
    const char *icon;
    const char *title;
    const char *colorLight;
    const char *colorDark;
    const char *bgLight;
    const char *bgDark;
};

constexpr std::array<CalloutDef, 5> s_dockCallouts = {{
    {"[!NOTE]", "ℹ️", "Note", "#0969da", "#2f81f7", "rgba(9, 105, 218, 0.08)", "rgba(47, 129, 247, 0.12)"},
    {"[!TIP]", "💡", "Tip", "#1a7f37", "#3fb950", "rgba(26, 127, 55, 0.08)", "rgba(63, 185, 80, 0.12)"},
    {"[!IMPORTANT]", "🟣", "Important", "#8250df", "#a371f7", "rgba(130, 80, 223, 0.08)", "rgba(163, 113, 247, 0.12)"},
    {"[!WARNING]", "⚠️", "Warning", "#9a6700", "#d29922", "rgba(154, 103, 0, 0.08)", "rgba(210, 153, 34, 0.12)"},
    {"[!CAUTION]", "🛑", "Caution", "#cf222e", "#f85149", "rgba(207, 34, 46, 0.08)", "rgba(248, 81, 73, 0.12)"},
}};

QString cleanDockCalloutInner(const QString &raw) {
    QString inner = raw.trimmed();
    if (inner.startsWith(QStringLiteral("<br />"))) {
        return inner.mid(6).trimmed();
    }
    if (inner.startsWith(QStringLiteral("<br>"))) {
        return inner.mid(4).trimmed();
    }
    if (inner.startsWith(QStringLiteral("<br/>"))) {
        return inner.mid(5).trimmed();
    }
    return inner;
}

void replaceSingleDockCallout(QString &result, const CalloutDef &c, qsizetype &idx, bool isDark) {
    const QString marker = QString::fromUtf8(c.tag);
    const qsizetype bqStart = result.lastIndexOf(QStringLiteral("<blockquote"), idx);
    if (bqStart == -1 || bqStart < idx - 100) {
        idx += marker.length();
        return;
    }

    const qsizetype bqEnd = result.indexOf(QStringLiteral("</blockquote>"), idx);
    if (bqEnd == -1) {
        idx += marker.length();
        return;
    }

    const qsizetype contentStart = idx + marker.length();
    const QString inner = cleanDockCalloutInner(result.mid(contentStart, bqEnd - contentStart));
    const QString color = isDark ? QString::fromLatin1(c.colorDark) : QString::fromLatin1(c.colorLight);
    const QString bg = isDark ? QString::fromLatin1(c.bgDark) : QString::fromLatin1(c.bgLight);
    const QString replacement = QStringLiteral(
                                    "<div style=\"margin: 12px 0; padding: 10px 14px; border-left: 4px solid %1; "
                                    "background-color: %2; border-radius: 4px;\">"
                                    "<div style=\"font-weight: 600; color: %1; margin-bottom: 4px;\">%3 %4</div>"
                                    "<div>%5</div></div>")
                                    .arg(color, bg, QString::fromUtf8(c.icon),
                                         QCoreApplication::translate("MarkdownSafePreviewDock", c.title), inner);

    result.replace(bqStart, (bqEnd + 13) - bqStart, replacement);
    idx = bqStart + replacement.length();
}
}  // namespace

QString MarkdownSafePreviewDock::enhanceCallouts(const QString &html, bool isDark) {
    QString result = html;
    for (const auto &c : s_dockCallouts) {
        const QString marker = QString::fromUtf8(c.tag);
        qsizetype idx = 0;
        while ((idx = result.indexOf(marker, idx, Qt::CaseInsensitive)) != -1) {
            replaceSingleDockCallout(result, c, idx, isDark);
        }
    }
    return result;
}

void MarkdownSafePreviewDock::renderPreview() {
    if (!currentEditor) {
        browser->clear();
        if (lblFormatStatus) {
            lblFormatStatus->setText(tr("No Document"));
        }
        if (lblStats) {
            lblStats->setText(tr("0 words • 0 min read"));
        }
        return;
    }

    applyGitHubTheme();

    const QString content = QString::fromUtf8(currentEditor->get_text_range(0, static_cast<int>(currentEditor->length())));
    const PreviewFormat effectiveFormat =
        (currentFormat == PreviewFormat::Auto) ? detectFormat(currentEditor) : currentFormat;

    updateFormatLabel(effectiveFormat);
    updateStatsLabel(content);

    auto *const scrollBar = browser->verticalScrollBar();
    const double scrollRatio = (scrollBar && scrollBar->maximum() > 0)
                                   ? static_cast<double>(scrollBar->value()) / static_cast<double>(scrollBar->maximum())
                                   : 0.0;

    if (effectiveFormat == PreviewFormat::Html) {
        browser->setHtml(content);
        if (comboTheme) {
            comboTheme->setEnabled(false);
        }
    } else {
        if (comboTheme) {
            comboTheme->setEnabled(true);
        }
        const bool mdIsDark = MarkdownPreviewEnhancer::isDarkTheme(currentTheme);
        const QString rendered = MarkdownPreviewEnhancer::compileMarkdownIt(content, mdIsDark, currentTheme);
        browser->setHtml(rendered);
    }

    if (scrollBar && scrollBar->maximum() > 0 && scrollRatio > 0.0) {
        scrollBar->setValue(static_cast<int>(scrollRatio * scrollBar->maximum()));
    }
}

void MarkdownSafePreviewDock::updateFormatLabel(PreviewFormat effectiveFormat) {
    if (!lblFormatStatus) {
        return;
    }

    using enum PreviewFormat;
    if (currentFormat == Auto) {
        if (effectiveFormat == Html) {
            lblFormatStatus->setText(tr("Auto (HTML)"));
        } else {
            lblFormatStatus->setText(tr("Auto (Markdown)"));
        }
    } else if (currentFormat == Html) {
        lblFormatStatus->setText(tr("HTML"));
    } else {
        lblFormatStatus->setText(tr("Markdown"));
    }
}

void MarkdownSafePreviewDock::updateStatsLabel(const QString &text) {
    if (!lblStats) {
        return;
    }
    if (text.isEmpty()) {
        lblStats->setText(tr("0 words • 0 min read"));
        return;
    }
    const QStringList words = text.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
    const qsizetype wordCount = words.size();
    const qsizetype minutes = std::max(static_cast<qsizetype>(1), (wordCount + 199) / 200);
    lblStats->setText(tr("%1 words • %2 min read").arg(wordCount).arg(minutes));
}

void MarkdownSafePreviewDock::setPreviewFormat(PreviewFormat newFormat) {
    if (currentFormat == newFormat) {
        return;
    }
    currentFormat = newFormat;
    if (comboFormat && comboFormat->currentIndex() != static_cast<int>(newFormat)) {
        comboFormat->setCurrentIndex(static_cast<int>(newFormat));
    }
    QSettings settings;
    settings.setValue(QStringLiteral("Gui/PreviewFormat"), static_cast<int>(newFormat));
    emit formatChanged(newFormat);
    renderPreview();
}

void MarkdownSafePreviewDock::onFormatComboChanged(int index) {
    if (index >= 0 && index <= 2) {
        setPreviewFormat(static_cast<PreviewFormat>(index));
    }
}

void MarkdownSafePreviewDock::onThemeComboChanged(int index) {
    if (index >= 0 && comboTheme && index < comboTheme->count()) {
        setMarkdownTheme(comboTheme->itemData(index).toString());
    }
}

void MarkdownSafePreviewDock::setMarkdownTheme(const QString &themeId) {
    if (currentTheme == themeId) {
        return;
    }
    currentTheme = themeId;
    QSettings settings;
    settings.setValue(QStringLiteral("Preview/MarkdownTheme"), currentTheme);
    if (comboTheme) {
        const int idx = comboTheme->findData(currentTheme);
        if (idx >= 0 && comboTheme->currentIndex() != idx) {
            comboTheme->setCurrentIndex(idx);
        }
    }
    applyGitHubTheme();
    renderPreview();
}

void MarkdownSafePreviewDock::setAutoShowEnabled(bool enabled) {
    if (autoShowEnabled == enabled) {
        return;
    }
    autoShowEnabled = enabled;
    QSettings settings;
    settings.setValue(QStringLiteral("Gui/AutoShowMarkdownHtmlPreview"), enabled);
    emit autoShowChanged(enabled);
}

void MarkdownSafePreviewDock::copyHtml() {
    if (!currentEditor) {
        return;
    }
    const QString rawContent = QString::fromUtf8(currentEditor->get_text_range(0, static_cast<int>(currentEditor->length())));
    const PreviewFormat effectiveFormat =
        (currentFormat == PreviewFormat::Auto) ? detectFormat(currentEditor) : currentFormat;
    QString bodyHtml;
    if (effectiveFormat == PreviewFormat::Html) {
        bodyHtml = rawContent;
    } else {
        const bool mdIsDark = MarkdownPreviewEnhancer::isDarkTheme(currentTheme);
        bodyHtml = MarkdownPreviewEnhancer::compileMarkdownIt(rawContent, mdIsDark, currentTheme);
    }
    const bool isDark = ThemeManager::instance()->isDarkActive();
    const QString fullHtml = generateFullHtmlDocument(bodyHtml, isDark);
    QApplication::clipboard()->setText(fullHtml);
    if (lblFormatStatus) {
        const QString orig = lblFormatStatus->text();
        lblFormatStatus->setText(tr("HTML Copied!"));
        QTimer::singleShot(2000, this, [this, orig] {
            if (lblFormatStatus) {
                lblFormatStatus->setText(orig);
            }
        });
    }
}

void MarkdownSafePreviewDock::exportHtml() {
    if (!currentEditor) {
        return;
    }
    QString defaultName = currentEditor->getName();
    if (defaultName.isEmpty()) {
        defaultName = tr("Untitled");
    }
    if (!defaultName.endsWith(QStringLiteral(".html"), Qt::CaseInsensitive)) {
        if (const auto dot = defaultName.lastIndexOf('.'); dot > 0) {
            defaultName = defaultName.left(dot);
        }
        defaultName += QStringLiteral(".html");
    }

    ExportOptionsDialog dlg(ExportOptionsDialog::TargetFormat::Html, defaultName, this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    const QString savePath = QFileDialog::getSaveFileName(this, tr("Export HTML Document"), defaultName,
                                                          tr("HTML Documents (*.html *.htm);;All Files (*)"));
    if (savePath.isEmpty()) {
        return;
    }
    const QString rawContent = QString::fromUtf8(currentEditor->get_text_range(0, static_cast<int>(currentEditor->length())));
    const PreviewFormat effectiveFormat =
        (currentFormat == PreviewFormat::Auto) ? detectFormat(currentEditor) : currentFormat;

    if (effectiveFormat == PreviewFormat::Html) {
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            file.write(rawContent.toUtf8());
            file.close();
        }
    } else {
        DocumentExportManager::exportMarkdownToHtml(rawContent, savePath, dlg.options());
    }
}

void MarkdownSafePreviewDock::exportPdf() {
    if (!currentEditor) {
        return;
    }
    QString defaultName = currentEditor->getName();
    if (defaultName.isEmpty()) {
        defaultName = tr("Untitled");
    }
    if (!defaultName.endsWith(QStringLiteral(".pdf"), Qt::CaseInsensitive)) {
        if (const auto dot = defaultName.lastIndexOf('.'); dot > 0) {
            defaultName = defaultName.left(dot);
        }
        defaultName += QStringLiteral(".pdf");
    }

    ExportOptionsDialog dlg(ExportOptionsDialog::TargetFormat::Pdf, defaultName, this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    const QString savePath = QFileDialog::getSaveFileName(this, tr("Export PDF Document"), defaultName,
                                                          tr("PDF Documents (*.pdf);;All Files (*)"));
    if (savePath.isEmpty()) {
        return;
    }

    const QString rawContent = QString::fromUtf8(currentEditor->get_text_range(0, static_cast<int>(currentEditor->length())));
    const PreviewFormat effectiveFormat =
        (currentFormat == PreviewFormat::Auto) ? detectFormat(currentEditor) : currentFormat;
    const bool isMarkdown = (effectiveFormat != PreviewFormat::Html);

    DocumentExportManager::exportToPdf(rawContent, isMarkdown, savePath, dlg.options());
}

void MarkdownSafePreviewDock::openInBrowser() const {
    if (!currentEditor) {
        return;
    }

    const PreviewFormat effectiveFormat =
        (currentFormat == PreviewFormat::Auto) ? detectFormat(currentEditor) : currentFormat;

    if (effectiveFormat == PreviewFormat::Html && currentEditor->isFile() && currentEditor->getFileInfo().exists()) {
        const QString path = currentEditor->getFileInfo().absoluteFilePath();
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        return;
    }

    const bool isDark = palette().color(QPalette::Window).lightness() < 128;
    const QString rawContent = QString::fromUtf8(currentEditor->get_text_range(0, static_cast<int>(currentEditor->length())));
    QString bodyHtml;
    if (effectiveFormat == PreviewFormat::Html) {
        bodyHtml = rawContent;
    } else {
        const bool mdIsDark = MarkdownPreviewEnhancer::isDarkTheme(currentTheme);
        bodyHtml = MarkdownPreviewEnhancer::compileMarkdownIt(rawContent, mdIsDark, currentTheme);
    }

    QTemporaryFile tempFile(QDir::tempPath() + QStringLiteral("/NotepadNext_preview_XXXXXX.html"));
    tempFile.setAutoRemove(false);
    if (tempFile.open()) {
        const QString fullHtml = generateFullHtmlDocument(bodyHtml, isDark);
        tempFile.write(fullHtml.toUtf8());
        const QString tempPath = tempFile.fileName();
        tempFile.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(tempPath));
    }
}

void MarkdownSafePreviewDock::zoomIn() {
    browser->zoomIn(1);
}

void MarkdownSafePreviewDock::zoomOut() {
    browser->zoomOut(1);
}

void MarkdownSafePreviewDock::resetZoom() {
    QFont f = browser->font();
    f.setPointSize(baseFontSize);
    browser->setFont(f);
}

void MarkdownSafePreviewDock::handleAnchorClicked(const QUrl &url) {
    const QString scheme = url.scheme().toLower();

    if (scheme == QStringLiteral("http") || scheme == QStringLiteral("https") || scheme == QStringLiteral("mailto")) {
        QDesktopServices::openUrl(url);
        return;
    }

    qWarning("Blocked unsafe Markdown/HTML preview link navigation: %s", qUtf8Printable(url.toString()));
    QMessageBox::warning(
        this, tr("Blocked Untrusted Link"),
        tr("Navigation to link '%1' with scheme '%2' is blocked for security.\n\n"
           "Only http, https, and mailto URLs are permitted in HTML & Markdown Preview (mitigating CVE-2026-20841).")
            .arg(url.toString(), scheme));
}

void MarkdownSafePreviewDock::syncScrollFromEditor(Scintilla::Update updated) {
    if (m_syncingScroll || !currentEditor || !isVisible()) {
        return;
    }
    if (!FlagSet(updated, Scintilla::Update::VScroll) && !FlagSet(updated, Scintilla::Update::Content)) {
        return;
    }
    const auto totalLines = static_cast<int>(currentEditor->lineCount());
    const auto linesOnScreen = static_cast<int>(currentEditor->linesOnScreen());
    const int maxScrollLine = std::max(1, totalLines - linesOnScreen);
    const auto currentFirstLine = static_cast<int>(currentEditor->firstVisibleLine());
    const double ratio =
        std::clamp(static_cast<double>(currentFirstLine) / static_cast<double>(maxScrollLine), 0.0, 1.0);

    auto *const scrollBar = browser->verticalScrollBar();
    if (scrollBar && scrollBar->maximum() > 0) {
        m_syncingScroll = true;
        scrollBar->setValue(static_cast<int>(ratio * scrollBar->maximum()));
        m_syncingScroll = false;
    }
}

void MarkdownSafePreviewDock::syncScrollFromBrowser() {
    if (m_syncingScroll || !currentEditor || !isVisible()) {
        return;
    }
    const QScrollBar *const scrollBar = browser->verticalScrollBar();
    if (!scrollBar || scrollBar->maximum() <= 0) {
        return;
    }
    const double ratio =
        std::clamp(static_cast<double>(scrollBar->value()) / static_cast<double>(scrollBar->maximum()), 0.0, 1.0);
    const auto totalLines = static_cast<int>(currentEditor->lineCount());
    const auto linesOnScreen = static_cast<int>(currentEditor->linesOnScreen());
    const int maxScrollLine = std::max(1, totalLines - linesOnScreen);
    const auto targetLine = static_cast<int>(ratio * static_cast<double>(maxScrollLine));

    m_syncingScroll = true;
    currentEditor->setFirstVisibleLine(targetLine);
    m_syncingScroll = false;
}
