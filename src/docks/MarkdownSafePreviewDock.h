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

#ifndef MARKDOWNSAFEPREVIEWDOCK_H
#define MARKDOWNSAFEPREVIEWDOCK_H

#include <QDockWidget>
#include <QPointer>
#include <QTextBrowser>
#include <QTimer>
#include <QUrl>
#include <cstdint>
#include "ScintillaTypes.h"

class ScintillaNext;
class MainWindow;
class QComboBox;
class QLabel;
class QPushButton;

class MarkdownSafePreviewDock : public QDockWidget {
    Q_OBJECT

public:
    enum class PreviewFormat : std::uint8_t {
        Auto = 0,
        Markdown = 1,
        Html = 2,
    };
    Q_ENUM(PreviewFormat)

    explicit MarkdownSafePreviewDock(MainWindow *parent = nullptr);
    ~MarkdownSafePreviewDock() override = default;

    [[nodiscard]] PreviewFormat format() const { return currentFormat; }
    static PreviewFormat detectFormat(const ScintillaNext *editor);
    [[nodiscard]] bool isAutoShowEnabled() const { return autoShowEnabled; }

public slots:
    void setActiveEditor(ScintillaNext *editor);
    void scheduleRender();
    void renderPreview();
    void setPreviewFormat(PreviewFormat newFormat);
    void setAutoShowEnabled(bool enabled);
    void openInBrowser() const;
    void copyHtml();
    void exportHtml();
    void exportPdf();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void syncScrollFromEditor(Scintilla::Update updated);
    void syncScrollFromBrowser();

signals:
    void formatChanged(PreviewFormat newFormat);
    void autoShowChanged(bool enabled);

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void handleAnchorClicked(const QUrl &url);
    void onFormatComboChanged(int index);

private:
    void setupUi();
    void updateFormatLabel(PreviewFormat effectiveFormat);
    void updateStatsLabel(const QString &text);
    void applyGitHubTheme();
    [[nodiscard]] QString buildGitHubStyleSheet(bool isDark) const;
    [[nodiscard]] QString generateFullHtmlDocument(const QString &bodyHtml, bool isDark) const;
    [[nodiscard]] static QString enhanceCallouts(const QString &html, bool isDark);

    QTextBrowser *browser = nullptr;
    QTimer *debounceTimer = nullptr;
    QPointer<ScintillaNext> currentEditor;

    QComboBox *comboFormat = nullptr;
    QLabel *lblFormatStatus = nullptr;
    QLabel *lblStats = nullptr;
    QPushButton *btnOpenBrowser = nullptr;
    QPushButton *btnCopyHtml = nullptr;
    QPushButton *btnExportHtml = nullptr;
    QPushButton *btnExportPdf = nullptr;
    QPushButton *btnRefresh = nullptr;
    QPushButton *btnZoomIn = nullptr;
    QPushButton *btnZoomOut = nullptr;
    QPushButton *btnResetZoom = nullptr;

    PreviewFormat currentFormat = PreviewFormat::Auto;
    bool autoShowEnabled = false;
    int baseFontSize = 13;
    bool m_isUpdatingTheme{false};
    bool m_syncingScroll{false};
};

#endif  // MARKDOWNSAFEPREVIEWDOCK_H
