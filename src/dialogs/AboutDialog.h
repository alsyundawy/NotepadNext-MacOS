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

#pragma once

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class QTabWidget;
class QLabel;
class QTextBrowser;

class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog() override = default;

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void copySystemInfoToClipboard() const;
    void updateThemeColors();

private:
    void setupUi();
    QWidget *createAboutTab();
    QWidget *createMaintainerTab();
    QWidget *createCreditsTab();
    QWidget *createEnvironmentTab();
    QWidget *createShortcutsTab();
    QWidget *createLicenseTab();

    QTabWidget *m_tabWidget = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_badgesLabel = nullptr;
    QLabel *m_metaLabel = nullptr;
    QLabel *m_noticeBox = nullptr;
    QLabel *m_diagnosticsCard = nullptr;
    QTextBrowser *m_maintainerBrowser = nullptr;
    QTextBrowser *m_creditsBrowser = nullptr;
    QTextBrowser *m_shortcutsBrowser = nullptr;
    QTextBrowser *m_licenseBrowser = nullptr;
    bool m_isUpdatingTheme = false;
};

#endif  // ABOUTDIALOG_H
