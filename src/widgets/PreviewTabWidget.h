#pragma once
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

#ifndef PREVIEWTABWIDGET_H
#define PREVIEWTABWIDGET_H

#include <QWidget>
#include <QPointer>
#include <QUrl>

#include "ScintillaNext.h"
class QTextBrowser;
class QLabel;
class QPushButton;
class QTimer;

class PreviewTabWidget : public QWidget {
    Q_OBJECT

public:
    explicit PreviewTabWidget(ScintillaNext *editor, bool isHtml, QWidget *parent = nullptr);
    ~PreviewTabWidget() override = default;

    [[nodiscard]] bool isHtml() const noexcept { return m_isHtml; }
    [[nodiscard]] ScintillaNext *sourceEditor() const noexcept { return m_editor.data(); }
    [[nodiscard]] QString documentTitle() const;

public slots:
    void renderPreview();
    void scheduleRender();
    void openInBrowser();
    void copyHtml();
    void exportHtml();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void updateThemeColors();

signals:
    void titleChanged(const QString &newTitle);
    void closeRequested();

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void handleAnchorClicked(const QUrl &url);
    void onEditorRenamed();
    void onEditorDestroyed();

private:
    void setupUi();
    void applyTheme();
    [[nodiscard]] QString buildDevDocsStyleSheet(bool isDark) const;
    [[nodiscard]] QString wrapHtmlDocument(const QString &rawBody, bool isDark) const;
    [[nodiscard]] static QString enhanceCallouts(const QString &html, bool isDark);

    QPointer<ScintillaNext> m_editor;
    bool m_isHtml{false};
    int m_baseFontSize{13};

    QTextBrowser *m_browser{nullptr};
    QTimer *m_debounceTimer{nullptr};

    QLabel *m_badgeLabel{nullptr};
    QLabel *m_titleLabel{nullptr};
    QLabel *m_statsLabel{nullptr};
    QPushButton *m_btnRefresh{nullptr};
    QPushButton *m_btnZoomOut{nullptr};
    QPushButton *m_btnResetZoom{nullptr};
    QPushButton *m_btnZoomIn{nullptr};
    QPushButton *m_btnCopyHtml{nullptr};
    QPushButton *m_btnExportHtml{nullptr};
    QPushButton *m_btnOpenBrowser{nullptr};
    bool m_isUpdatingTheme{false};
};

#endif  // PREVIEWTABWIDGET_H
