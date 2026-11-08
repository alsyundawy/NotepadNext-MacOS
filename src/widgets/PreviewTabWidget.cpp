/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUSTION | https://www.alsyundawy.com
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

#include "PreviewTabWidget.h"
#include "../ScintillaNext.h"
#include "../features/ThemeManager.h"
#include "../features/ThemePaletteData.h"

#include <algorithm>
#include <array>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTemporaryFile>
#include <QTextBrowser>
#include <QTextDocument>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

namespace {
QString getEditorContent(const ScintillaNext *editor) {
    if (!editor) {
        return {};
    }
    const auto len = static_cast<int>(editor->length());
    return QString::fromUtf8(editor->get_text_range(0, len));
}
}

PreviewTabWidget::PreviewTabWidget(ScintillaNext *editor, bool isHtml, QWidget *parent)
    : QWidget(parent), m_editor(editor), m_isHtml(isHtml) {
    setupUi();

    m_debounceTimer = new QTimer(this);
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(200);
    connect(m_debounceTimer, &QTimer::timeout, this, &PreviewTabWidget::renderPreview);

    if (m_editor) {
        connect(m_editor, &ScintillaNext::notifyChange, this, &PreviewTabWidget::scheduleRender);
        connect(m_editor, &ScintillaNext::renamed, this, &PreviewTabWidget::onEditorRenamed);
        connect(m_editor, &QObject::destroyed, this, &PreviewTabWidget::onEditorDestroyed);
    }

    connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &PreviewTabWidget::updateThemeColors);
    connect(ThemeManager::instance(), &ThemeManager::paletteChanged, this, &PreviewTabWidget::updateThemeColors);

    applyTheme();
    renderPreview();
}

QString PreviewTabWidget::documentTitle() const {
    if (m_editor) {
        const QString name = m_editor->getName();
        return m_isHtml ? tr("Preview HTML - %1").arg(name) : tr("Preview Markdown - %1").arg(name);
    }
    return m_isHtml ? tr("Preview HTML") : tr("Preview Markdown");
}

void PreviewTabWidget::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sleek, modern header toolbar
    auto *headerWidget = new QWidget(this);
    headerWidget->setObjectName(QStringLiteral("previewHeaderBar"));
    auto *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 5, 10, 5);
    headerLayout->setSpacing(6);

    // Format badge
    m_badgeLabel = new QLabel(headerWidget);
    m_badgeLabel->setObjectName(QStringLiteral("previewBadge"));
    m_badgeLabel->setText(m_isHtml ? QStringLiteral("HTML") : QStringLiteral("MARKDOWN"));
    headerLayout->addWidget(m_badgeLabel);

    // Document title
    m_titleLabel = new QLabel(headerWidget);
    m_titleLabel->setObjectName(QStringLiteral("previewTitle"));
    const QString fileName = m_editor ? m_editor->getName() : tr("Untitled");
    m_titleLabel->setText(fileName);
    headerLayout->addWidget(m_titleLabel);

    // Stats pill badge
    m_statsLabel = new QLabel(headerWidget);
    m_statsLabel->setObjectName(QStringLiteral("previewStats"));
    m_statsLabel->setText(tr("0 words • 0 min read"));
    headerLayout->addWidget(m_statsLabel);

    headerLayout->addStretch(1);

    // Refresh button
    m_btnRefresh = new QPushButton(headerWidget);
    m_btnRefresh->setIcon(QIcon(QStringLiteral(":/icons/arrow_refresh.png")));
    m_btnRefresh->setIconSize(QSize(14, 14));
    m_btnRefresh->setToolTip(tr("Refresh Preview"));
    m_btnRefresh->setFixedSize(28, 24);
    m_btnRefresh->setCursor(Qt::PointingHandCursor);
    connect(m_btnRefresh, &QPushButton::clicked, this, &PreviewTabWidget::renderPreview);
    headerLayout->addWidget(m_btnRefresh);

    // Zoom out
    m_btnZoomOut = new QPushButton(headerWidget);
    m_btnZoomOut->setIcon(QIcon(QStringLiteral(":/icons/zoomOut.png")));
    m_btnZoomOut->setIconSize(QSize(14, 14));
    m_btnZoomOut->setToolTip(tr("Zoom Out"));
    m_btnZoomOut->setFixedSize(28, 24);
    m_btnZoomOut->setCursor(Qt::PointingHandCursor);
    connect(m_btnZoomOut, &QPushButton::clicked, this, &PreviewTabWidget::zoomOut);
    headerLayout->addWidget(m_btnZoomOut);

    // Reset zoom
    m_btnResetZoom = new QPushButton(QStringLiteral("100%"), headerWidget);
    m_btnResetZoom->setToolTip(tr("Reset Zoom (100%)"));
    m_btnResetZoom->setFixedHeight(24);
    m_btnResetZoom->setMinimumWidth(48);
    m_btnResetZoom->setCursor(Qt::PointingHandCursor);
    connect(m_btnResetZoom, &QPushButton::clicked, this, &PreviewTabWidget::resetZoom);
    headerLayout->addWidget(m_btnResetZoom);

    // Zoom in
    m_btnZoomIn = new QPushButton(headerWidget);
    m_btnZoomIn->setIcon(QIcon(QStringLiteral(":/icons/zoomIn.png")));
    m_btnZoomIn->setIconSize(QSize(14, 14));
    m_btnZoomIn->setToolTip(tr("Zoom In"));
    m_btnZoomIn->setFixedSize(28, 24);
    m_btnZoomIn->setCursor(Qt::PointingHandCursor);
    connect(m_btnZoomIn, &QPushButton::clicked, this, &PreviewTabWidget::zoomIn);
    headerLayout->addWidget(m_btnZoomIn);

    // Copy HTML button
    m_btnCopyHtml = new QPushButton(headerWidget);
    m_btnCopyHtml->setIcon(QIcon(QStringLiteral(":/icons/copy.png")));
    m_btnCopyHtml->setIconSize(QSize(14, 14));
    m_btnCopyHtml->setToolTip(tr("Copy Rendered HTML"));
    m_btnCopyHtml->setFixedSize(28, 24);
    m_btnCopyHtml->setCursor(Qt::PointingHandCursor);
    connect(m_btnCopyHtml, &QPushButton::clicked, this, &PreviewTabWidget::copyHtml);
    headerLayout->addWidget(m_btnCopyHtml);

    // Export HTML button
    m_btnExportHtml = new QPushButton(headerWidget);
    m_btnExportHtml->setIcon(QIcon(QStringLiteral(":/icons/saved.png")));
    m_btnExportHtml->setIconSize(QSize(14, 14));
    m_btnExportHtml->setToolTip(tr("Export as Standalone HTML File..."));
    m_btnExportHtml->setFixedSize(28, 24);
    m_btnExportHtml->setCursor(Qt::PointingHandCursor);
    connect(m_btnExportHtml, &QPushButton::clicked, this, &PreviewTabWidget::exportHtml);
    headerLayout->addWidget(m_btnExportHtml);

    // Open in browser button
    m_btnOpenBrowser = new QPushButton(headerWidget);
    m_btnOpenBrowser->setIcon(QIcon(QStringLiteral(":/icons/openFile.png")));
    m_btnOpenBrowser->setIconSize(QSize(14, 14));
    m_btnOpenBrowser->setToolTip(tr("Open in Default Web Browser"));
    m_btnOpenBrowser->setText(QStringLiteral("Browser"));
    m_btnOpenBrowser->setFixedHeight(24);
    m_btnOpenBrowser->setCursor(Qt::PointingHandCursor);
    connect(m_btnOpenBrowser, &QPushButton::clicked, this, &PreviewTabWidget::openInBrowser);
    headerLayout->addWidget(m_btnOpenBrowser);

    mainLayout->addWidget(headerWidget);

    // Text browser display
    m_browser = new QTextBrowser(this);
    m_browser->setObjectName(QStringLiteral("previewBrowser"));
    m_browser->setOpenExternalLinks(false);
    m_browser->setOpenLinks(false);
    connect(m_browser, &QTextBrowser::anchorClicked, this, &PreviewTabWidget::handleAnchorClicked);

    mainLayout->addWidget(m_browser, 1);

    m_baseFontSize = m_browser->font().pointSize();
    if (m_baseFontSize <= 0) {
        m_baseFontSize = 13;
    }
}

void PreviewTabWidget::updateThemeColors() {
    applyTheme();
    renderPreview();
}

void PreviewTabWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::PaletteChange) {
        applyTheme();
    }
    QWidget::changeEvent(event);
}

void PreviewTabWidget::applyTheme() {
    if (m_isUpdatingTheme) {
        return;
    }
    m_isUpdatingTheme = true;

    const bool isDark = ThemeManager::instance()->isDarkActive();
    const ThemePalette *pal = ThemeManager::instance()->currentPalette();

    const QString barBg = isDark ? QStringLiteral("#161b22") : QStringLiteral("#f6f8fa");
    const QString barBorder = isDark ? QStringLiteral("#30363d") : QStringLiteral("#d0d7de");
    const QString textPri = isDark ? QStringLiteral("#e6edf3") : QStringLiteral("#1f2328");
    const QString accentCol = pal ? QString::fromLatin1(pal->blue) : QStringLiteral("#0969da");
    const QString btnBg = isDark ? QStringLiteral("#21262d") : QStringLiteral("#ffffff");
    const QString btnBorder = isDark ? QStringLiteral("#363b42") : QStringLiteral("#d0d7de");

    QString badgeBg;
    QString badgeFg;
    if (m_isHtml) {
        badgeBg = isDark ? QStringLiteral("#1f4037") : QStringLiteral("#ddf4ff");
        badgeFg = isDark ? QStringLiteral("#56d364") : QStringLiteral("#0969da");
    } else {
        badgeBg = isDark ? QStringLiteral("#2e1f47") : QStringLiteral("#fbefff");
        badgeFg = isDark ? QStringLiteral("#d2a8ff") : QStringLiteral("#8250df");
    }

    const QString textSec = isDark ? QStringLiteral("#8b949e") : QStringLiteral("#57606a");

    const QString headerStyle =
        QStringLiteral(
            "QWidget#previewHeaderBar {"
            "  background-color: %1;"
            "  border-bottom: 1px solid %2;"
            "}"
            "QLabel#previewBadge {"
            "  background-color: %3;"
            "  color: %4;"
            "  font-size: 10px;"
            "  font-weight: 700;"
            "  padding: 2px 7px;"
            "  border-radius: 4px;"
            "  letter-spacing: 0.5px;"
            "}"
            "QLabel#previewTitle {"
            "  color: %5;"
            "  font-size: 11px;"
            "  font-weight: 600;"
            "}"
            "QLabel#previewStats {"
            "  color: %9;"
            "  font-size: 10px;"
            "  padding: 2px 6px;"
            "}"
            "QPushButton {"
            "  background-color: %6;"
            "  color: %5;"
            "  border: 1px solid %7;"
            "  border-radius: 4px;"
            "  font-size: 11px;"
            "  padding: 2px 7px;"
            "}"
            "QPushButton:hover {"
            "  border-color: %8;"
            "  color: %8;"
            "}"
            "QPushButton:pressed {"
            "  background-color: %8;"
            "  color: #ffffff;"
            "}")
            .arg(barBg, barBorder, badgeBg, badgeFg, textPri, btnBg, btnBorder, accentCol, textSec);

    if (auto *header = findChild<QWidget *>(QStringLiteral("previewHeaderBar"))) {
        header->setStyleSheet(headerStyle);
    }

    const QString browserBg = isDark ? QStringLiteral("#0d1117") : QStringLiteral("#ffffff");
    const QString browserFg = isDark ? QStringLiteral("#e6edf3") : QStringLiteral("#1f2328");
    const QString selBg =
        isDark ? QStringLiteral("rgba(56, 139, 253, 0.4)") : QStringLiteral("rgba(9, 105, 218, 0.25)");

    m_browser->setStyleSheet(
        QStringLiteral(
            "QTextBrowser { background-color: %1; color: %2; border: none; selection-background-color: %3; }")
            .arg(browserBg, browserFg, selBg));

    m_browser->document()->setDefaultStyleSheet(buildDevDocsStyleSheet(isDark));
    m_isUpdatingTheme = false;
}

QString PreviewTabWidget::buildDevDocsStyleSheet(bool isDark) const {
    const QString textColor = isDark ? QStringLiteral("#e6edf3") : QStringLiteral("#1f2328");
    const QString bgColor = isDark ? QStringLiteral("#0d1117") : QStringLiteral("#ffffff");
    const QString borderColor = isDark ? QStringLiteral("#30363d") : QStringLiteral("#d8dee4");
    const QString mutedColor = isDark ? QStringLiteral("#8b949e") : QStringLiteral("#57606a");
    const QString codeBg =
        isDark ? QStringLiteral("rgba(110, 118, 129, 0.4)") : QStringLiteral("rgba(175, 184, 193, 0.2)");
    const QString blockBg = isDark ? QStringLiteral("#161b22") : QStringLiteral("#f6f8fa");
    const QString linkColor = isDark ? QStringLiteral("#58a6ff") : QStringLiteral("#0969da");

    return QStringLiteral(
               "body { "
               "  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; "
               "  font-size: %1px; line-height: 1.6; color: %2; background-color: %3; margin: 0; padding: 18px 24px; "
               "} "
               "h1, h2, h3, h4, h5, h6 { "
               "  font-weight: 600; line-height: 1.3; margin-top: 24px; margin-bottom: 14px; color: %2; "
               "} "
               "h1 { font-size: 2.1em; padding-bottom: 0.3em; border-bottom: 1px solid %4; } "
               "h2 { font-size: 1.5em; padding-bottom: 0.3em; border-bottom: 1px solid %4; } "
               "h3 { font-size: 1.25em; } "
               "h4 { font-size: 1.05em; } "
               "h5 { font-size: 0.9em; } "
               "h6 { font-size: 0.85em; color: %5; } "
               "p { margin-top: 0; margin-bottom: 14px; line-height: 1.6; } "
               "blockquote { "
               "  margin: 0 0 16px 0; padding: 4px 14px; color: %5; border-left: 4px solid %4; background-color: %7; "
               "border-radius: 0 4px 4px 0; "
               "} "
               "ul, ol { margin-top: 0; margin-bottom: 14px; padding-left: 24px; } "
               "li { margin-top: 0.3em; } "
               "code { "
               "  font-family: 'SFMono-Regular', Menlo, Consolas, Monaco, 'Liberation Mono', monospace; "
               "  font-size: 88%; padding: 2px 6px; background-color: %6; border-radius: 4px; "
               "} "
               "pre { "
               "  margin-top: 0; margin-bottom: 16px; padding: 14px; line-height: 1.45; "
               "  background-color: %7; border: 1px solid %4; border-radius: 6px; "
               "  font-family: 'SFMono-Regular', Menlo, Consolas, Monaco, 'Liberation Mono', monospace; font-size: "
               "88%; "
               "} "
               "table { border-collapse: collapse; border-spacing: 0; margin-top: 0; margin-bottom: 16px; width: 100%; "
               "} "
               "th, td { padding: 8px 12px; border: 1px solid %4; } "
               "th { font-weight: 600; background-color: %7; text-align: left; } "
               "tr:nth-child(2n) { background-color: %7; } "
               "hr { height: 2px; padding: 0; margin: 22px 0; background-color: %4; border: 0; } "
               "a { color: %8; text-decoration: none; font-weight: 500; } "
               "kbd { "
               "  display: inline-block; padding: 2px 6px; font-size: 11px; line-height: 12px; color: %2; "
               "  background-color: %7; border: 1px solid %4; border-radius: 4px; box-shadow: inset 0 -1px 0 %4; "
               "} "
               "img { max-width: 100%; height: auto; border-radius: 4px; }")
        .arg(QString::number(m_baseFontSize), textColor, bgColor, borderColor, mutedColor, codeBg, blockBg, linkColor);
}

void PreviewTabWidget::renderPreview() {
    if (!m_editor) {
        m_browser->clear();
        m_titleLabel->setText(tr("(Editor Closed)"));
        if (m_statsLabel) {
            m_statsLabel->setText(tr("0 words • 0 min read"));
        }
        return;
    }

    m_titleLabel->setText(m_editor->getName());

    if (m_editor->isFile()) {
        const QFileInfo fi = m_editor->getFileInfo();
        if (fi.exists()) {
            m_browser->document()->setBaseUrl(QUrl::fromLocalFile(fi.absolutePath() + QStringLiteral("/")));
        }
    }

    const QString rawContent = getEditorContent(m_editor.data());

    if (m_statsLabel) {
        if (rawContent.isEmpty()) {
            m_statsLabel->setText(tr("0 words • 0 min read"));
        } else {
            const QStringList words = rawContent.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            const qsizetype wordCount = words.size();
            const qsizetype minutes = std::max(static_cast<qsizetype>(1), (wordCount + 199) / 200);
            m_statsLabel->setText(tr("%1 words • %2 min read").arg(wordCount).arg(minutes));
        }
    }

    auto *const scrollBar = m_browser->verticalScrollBar();
    const double scrollRatio = (scrollBar && scrollBar->maximum() > 0)
                                   ? static_cast<double>(scrollBar->value()) / static_cast<double>(scrollBar->maximum())
                                   : 0.0;

    const bool isDark = ThemeManager::instance()->isDarkActive();
    if (m_isHtml) {
        m_browser->setHtml(rawContent);
    } else {
        QTextDocument doc;
        doc.setMarkdown(rawContent);
        const QString enhanced = enhanceCallouts(doc.toHtml(), isDark);
        m_browser->setHtml(enhanced);
    }

    if (scrollBar && scrollBar->maximum() > 0 && scrollRatio > 0.0) {
        scrollBar->setValue(static_cast<int>(scrollRatio * scrollBar->maximum()));
    }
}

void PreviewTabWidget::scheduleRender() {
    if (m_debounceTimer) {
        m_debounceTimer->start();
    }
}

void PreviewTabWidget::zoomIn() {
    m_baseFontSize = std::min(m_baseFontSize + 1, 28);
    applyTheme();
    renderPreview();
}

void PreviewTabWidget::zoomOut() {
    m_baseFontSize = std::max(m_baseFontSize - 1, 9);
    applyTheme();
    renderPreview();
}

void PreviewTabWidget::resetZoom() {
    m_baseFontSize = 13;
    applyTheme();
    renderPreview();
}

void PreviewTabWidget::copyHtml() {
    if (!m_editor) {
        return;
    }
    const QString rawContent = getEditorContent(m_editor.data());
    const bool isDark = ThemeManager::instance()->isDarkActive();
    QString fullHtml;
    if (m_isHtml) {
        fullHtml = wrapHtmlDocument(rawContent, isDark);
    } else {
        QTextDocument doc;
        doc.setMarkdown(rawContent);
        fullHtml = wrapHtmlDocument(enhanceCallouts(doc.toHtml(), isDark), isDark);
    }
    QApplication::clipboard()->setText(fullHtml);
    if (m_badgeLabel) {
        const QString orig = m_badgeLabel->text();
        m_badgeLabel->setText(tr("COPIED!"));
        QTimer::singleShot(2000, this, [this, orig] {
            if (m_badgeLabel) {
                m_badgeLabel->setText(orig);
            }
        });
    }
}

void PreviewTabWidget::exportHtml() {
    if (!m_editor) {
        return;
    }
    QString defaultName = m_editor->getName();
    if (defaultName.isEmpty()) {
        defaultName = tr("Untitled");
    }
    if (!defaultName.endsWith(QStringLiteral(".html"), Qt::CaseInsensitive)) {
        if (const auto dot = defaultName.lastIndexOf('.'); dot > 0) {
            defaultName = defaultName.left(dot);
        }
        defaultName += QStringLiteral(".html");
    }
    const QString savePath = QFileDialog::getSaveFileName(this, tr("Export HTML Document"), defaultName,
                                                          tr("HTML Documents (*.html *.htm);;All Files (*)"));
    if (savePath.isEmpty()) {
        return;
    }
    const QString rawContent = getEditorContent(m_editor.data());
    const bool isDark = ThemeManager::instance()->isDarkActive();
    QString fullHtml;
    if (m_isHtml) {
        fullHtml = wrapHtmlDocument(rawContent, isDark);
    } else {
        QTextDocument doc;
        doc.setMarkdown(rawContent);
        fullHtml = wrapHtmlDocument(enhanceCallouts(doc.toHtml(), isDark), isDark);
    }
    QFile file(savePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        file.write(fullHtml.toUtf8());
        file.close();
    }
}

void PreviewTabWidget::openInBrowser() {
    if (!m_editor) {
        return;
    }

    if (m_isHtml && m_editor->isFile() && m_editor->getFileInfo().exists()) {
        const QString path = m_editor->getFileInfo().absoluteFilePath();
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        return;
    }

    const bool isDark = ThemeManager::instance()->isDarkActive();
    const QString content = getEditorContent(m_editor.data());

    auto *tempFile = new QTemporaryFile(QDir::tempPath() + QStringLiteral("/npn_preview_XXXXXX.html"), this);
    if (tempFile->open()) {
        QString fullHtml;
        if (m_isHtml) {
            fullHtml = wrapHtmlDocument(content, isDark);
        } else {
            QTextDocument doc;
            doc.setMarkdown(content);
            fullHtml = wrapHtmlDocument(enhanceCallouts(doc.toHtml(), isDark), isDark);
        }
        tempFile->write(fullHtml.toUtf8());
        tempFile->flush();
        const QString tempPath = tempFile->fileName();
        tempFile->close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(tempPath));
    }
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

constexpr std::array<CalloutDef, 5> s_callouts = {{
    {"[!NOTE]", "ℹ️", "Note", "#0969da", "#2f81f7", "rgba(9, 105, 218, 0.08)", "rgba(47, 129, 247, 0.12)"},
    {"[!TIP]", "💡", "Tip", "#1a7f37", "#3fb950", "rgba(26, 127, 55, 0.08)", "rgba(63, 185, 80, 0.12)"},
    {"[!IMPORTANT]", "🟣", "Important", "#8250df", "#a371f7", "rgba(130, 80, 223, 0.08)", "rgba(163, 113, 247, 0.12)"},
    {"[!WARNING]", "⚠️", "Warning", "#9a6700", "#d29922", "rgba(154, 103, 0, 0.08)", "rgba(210, 153, 34, 0.12)"},
    {"[!CAUTION]", "🛑", "Caution", "#cf222e", "#f85149", "rgba(207, 34, 46, 0.08)", "rgba(248, 81, 73, 0.12)"},
}};

QString cleanCalloutInner(const QString &raw) {
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

void replaceSingleCallout(QString &result, const CalloutDef &c, qsizetype &idx, bool isDark) {
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
    const QString inner = cleanCalloutInner(result.mid(contentStart, bqEnd - contentStart));
    const QString color = isDark ? QString::fromLatin1(c.colorDark) : QString::fromLatin1(c.colorLight);
    const QString bg = isDark ? QString::fromLatin1(c.bgDark) : QString::fromLatin1(c.bgLight);
    const QString replacement =
        QStringLiteral(
            "<div style=\"margin: 12px 0; padding: 10px 14px; border-left: 4px solid %1; "
            "background-color: %2; border-radius: 4px;\">"
            "<div style=\"font-weight: 600; color: %1; margin-bottom: 4px;\">%3 %4</div>"
            "<div>%5</div></div>")
            .arg(color, bg, QString::fromUtf8(c.icon), QCoreApplication::translate("PreviewTabWidget", c.title), inner);

    result.replace(bqStart, (bqEnd + 13) - bqStart, replacement);
    idx = bqStart + replacement.length();
}
}  // namespace

QString PreviewTabWidget::enhanceCallouts(const QString &html, bool isDark) {
    QString result = html;
    for (const auto &c : s_callouts) {
        const QString marker = QString::fromUtf8(c.tag);
        qsizetype idx = 0;
        while ((idx = result.indexOf(marker, idx, Qt::CaseInsensitive)) != -1) {
            replaceSingleCallout(result, c, idx, isDark);
        }
    }
    return result;
}

QString PreviewTabWidget::wrapHtmlDocument(const QString &rawBody, bool isDark) const {
    const QString css = buildDevDocsStyleSheet(isDark);
    const QString title = m_editor ? m_editor->getName() : tr("NotepadNext Preview");

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
               "<body>\n"
               "%3\n"
               "</body>\n"
               "</html>\n")
        .arg(title.toHtmlEscaped(), css, rawBody);
}

void PreviewTabWidget::handleAnchorClicked(const QUrl &url) {
    if (url.scheme() == QStringLiteral("http") || url.scheme() == QStringLiteral("https") ||
        url.scheme() == QStringLiteral("mailto")) {
        QDesktopServices::openUrl(url);
    } else if (url.hasFragment() && (url.scheme().isEmpty() || url.scheme() == QStringLiteral("file"))) {
        m_browser->scrollToAnchor(url.fragment());
    }
}

void PreviewTabWidget::onEditorRenamed() {
    if (m_editor) {
        m_titleLabel->setText(m_editor->getName());
        emit titleChanged(documentTitle());
    }
}

void PreviewTabWidget::onEditorDestroyed() {
    m_editor = nullptr;
    m_titleLabel->setText(tr("(Editor Closed)"));
}
