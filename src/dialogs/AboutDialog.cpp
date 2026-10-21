/*
 * This file is part of Notepad Next.
 * Copyright 2026 Harry DS Alsyundawy (Notepad Next macOS Maintainer)
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "AboutDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QGuiApplication>
#include <QClipboard>
#include <QSysInfo>
#include <QScreen>
#include <QPixmap>
#include <QEvent>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent) {
    setupUi();
    updateThemeColors();
}

void AboutDialog::setupUi() {
    setWindowTitle(tr("About Notepad Next"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setModal(true);
    resize(740, 580);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(14);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(createAboutTab(), tr("&About"));
    m_tabWidget->addTab(createMaintainerTab(), tr("&Maintainer"));
    m_tabWidget->addTab(createCreditsTab(), tr("&Credits && Attribution"));
    m_tabWidget->addTab(createEnvironmentTab(), tr("&Diagnostics"));
    m_tabWidget->addTab(createShortcutsTab(), tr("&Shortcuts"));
    m_tabWidget->addTab(createLicenseTab(), tr("&License"));

    mainLayout->addWidget(m_tabWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::accept);
    mainLayout->addWidget(buttonBox);
}

void AboutDialog::changeEvent(QEvent *event) {
    QDialog::changeEvent(event);
    if ((event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) && !m_isUpdatingTheme) {
        updateThemeColors();
    }
}

void AboutDialog::updateThemeColors() {
    if (m_isUpdatingTheme) {
        return;
    }
    m_isUpdatingTheme = true;

    // Dynamically derive styling directly from the active system palette (macOS light/dark appearance)
    const QPalette pal = this->palette();
    const QColor winColor = pal.color(QPalette::Window);
    const bool dark = (winColor.lightness() < 128);

    const QString winBg = winColor.name();
    const QString cardBg = dark ? pal.color(QPalette::Base).name() : QStringLiteral("#ffffff");
    const QString borderCol = dark ? QStringLiteral("#3e3e42") : QStringLiteral("#cbd5e1");
    const QString accentCol = pal.color(QPalette::Highlight).isValid() && pal.color(QPalette::Highlight) != winColor
                                ? pal.color(QPalette::Highlight).name()
                                : (dark ? QStringLiteral("#38bdf8") : QStringLiteral("#0078d4"));
    const QString txtPri = pal.color(QPalette::WindowText).name();
    const QString txtSec = dark ? QStringLiteral("#94a3b8") : QStringLiteral("#475569");
    const QString badgeBg = dark ? QStringLiteral("rgba(255,255,255,0.08)") : QStringLiteral("rgba(0,0,0,0.05)");

    const QString cardStyle =
        QString(
            "QLabel {"
            "  padding: 14px;"
            "  background-color: %1;"
            "  border: 1px solid %2;"
            "  border-left: 5px solid %3;"
            "  border-radius: 8px;"
            "  color: %4;"
            "  font-family: -apple-system, BlinkMacSystemFont, 'SF Pro Text', 'Segoe UI', Roboto, sans-serif;"
            "  font-size: 13px;"
            "  line-height: 1.6;"
            "}")
            .arg(cardBg, borderCol, accentCol, txtPri);

    const QString dialogStyle =
        QString(
            "QDialog {"
            "  background-color: %1;"
            "  color: %4;"
            "  font-family: -apple-system, BlinkMacSystemFont, 'SF Pro Text', 'Segoe UI', Roboto, sans-serif;"
            "  font-size: 13px;"
            "}"
            "QTabWidget::pane {"
            "  background-color: %2;"
            "  border: 1px solid %3;"
            "  border-radius: 8px;"
            "}"
            "QTabBar::tab {"
            "  background-color: %1;"
            "  color: %5;"
            "  padding: 8px 18px;"
            "  border: 1px solid %3;"
            "  border-bottom: none;"
            "  border-top-left-radius: 6px;"
            "  border-top-right-radius: 6px;"
            "  margin-right: 2px;"
            "  font-weight: 500;"
            "}"
            "QTabBar::tab:selected {"
            "  background-color: %2;"
            "  color: %4;"
            "  font-weight: bold;"
            "  border-top: 3px solid %6;"
            "}"
            "QTabBar::tab:hover:!selected {"
            "  background-color: %3;"
            "  color: %4;"
            "}"
            "QLabel {"
            "  color: %4;"
            "}"
            "QTextBrowser {"
            "  background-color: %2;"
            "  color: %4;"
            "  border: 1px solid %3;"
            "  border-radius: 8px;"
            "  padding: 14px;"
            "  font-family: -apple-system, BlinkMacSystemFont, 'SF Pro Text', 'Segoe UI', Roboto, sans-serif;"
            "  font-size: 13px;"
            "  line-height: 1.6;"
            "}"
            "QPushButton {"
            "  background-color: %2;"
            "  color: %4;"
            "  border: 1px solid %3;"
            "  border-radius: 6px;"
            "  padding: 7px 18px;"
            "  font-weight: 600;"
            "}"
            "QPushButton:hover {"
            "  border-color: %6;"
            "  color: %6;"
            "}"
            "QPushButton:pressed {"
            "  background-color: %6;"
            "  color: #ffffff;"
            "}")
            .arg(winBg, cardBg, borderCol, txtPri, txtSec, accentCol);

    setStyleSheet(dialogStyle);

    if (m_titleLabel) {
        m_titleLabel->setText(
            QString(
                "<h2 style='color: %1; margin: 0 0 4px 0;'>Notepad Next <span style='font-size: 14px; font-weight: normal; color: %4;'>for macOS</span> v%2 "
                "<span style='font-size: 11px; padding: 2px 8px; background-color: %3; color: #ffffff; border-radius: 4px; font-weight: bold;'>STABLE</span></h2>"
                "<p style='margin: 0; font-weight: 600; font-size: 13px; color: %1;'>"
                "A fast, modern, and cross-platform reimplementation of Notepad++ for macOS</p>"
                "<p style='margin: 2px 0 0 0; font-style: italic; font-size: 12px; color: %4;'>"
                "Powered natively by Scintilla 5.6.x &amp; Lexilla 5.5.x with Qt 6 &amp; uchardet</p>")
                .arg(txtPri, APP_VERSION, accentCol, txtSec));
    }

    if (m_badgesLabel) {
        m_badgesLabel->setText(
            QString("<p style='margin-top: 8px; font-size: 11px; line-height: 1.8;'>"
                    "<span style='background: %1; color: %2; border: 1px solid %3; border-radius: 10px; padding: 3px 9px; font-weight: 500;'>⚡ Scintilla 5.6.x</span> "
                    "<span style='background: %1; color: %2; border: 1px solid %3; border-radius: 10px; padding: 3px 9px; font-weight: 500;'>🔤 Lexilla 5.5.x</span> "
                    "<span style='background: %1; color: %2; border: 1px solid %3; border-radius: 10px; padding: 3px 9px; font-weight: 500;'>🌐 uchardet Encodings</span> "
                    "<span style='background: %1; color: %2; border: 1px solid %3; border-radius: 10px; padding: 3px 9px; font-weight: 500;'>🍏 Apple Silicon ARM64 &amp; x86_64</span> "
                    "<span style='background: %1; color: %2; border: 1px solid %3; border-radius: 10px; padding: 3px 9px; font-weight: 500;'>🛡️ ISO C++20</span>"
                    "</p>")
                .arg(badgeBg, txtPri, borderCol));
    }

    if (m_noticeBox) {
        m_noticeBox->setStyleSheet(cardStyle);
        m_noticeBox->setText(
            tr("<b style='color: %1;'>Important Notice &amp; Attribution:</b><br/>"
               "<span style='color: %1;'>Notepad Next for macOS is an independent open-source desktop text editor "
               "based on Justin Dailey's original NotepadNext, hardened and maintained for macOS and Apple Silicon "
               "by Harry DS Alsyundawy.</span>")
                .arg(txtPri));
    }

    if (m_metaLabel) {
        m_metaLabel->setStyleSheet(cardStyle);
        m_metaLabel->setText(
            QString(
                "<div style='color: %1; line-height: 1.6;'>"
                "<h4 style='margin: 0 0 6px 0; color: %1;'>Official Repository &amp; Distribution Information</h4>"
                "<table border='0' cellspacing='4' cellpadding='2' style='color: %1; font-size: 12px; width: 100%;'>"
                "<tr>"
                "  <td style='width: 150px; font-weight: bold;'>macOS Repository:</td>"
                "  <td><a style='color: %2; text-decoration: none; font-weight: 600;' href='https://github.com/alsyundawy/NotepadNext-MacOS'>"
                "📦 https://github.com/alsyundawy/NotepadNext-MacOS</a></td>"
                "</tr>"
                "<tr>"
                "  <td style='font-weight: bold;'>Upstream Project:</td>"
                "  <td><a style='color: %2; text-decoration: none;' href='https://github.com/dail8859/NotepadNext'>"
                "https://github.com/dail8859/NotepadNext</a></td>"
                "</tr>"
                "<tr>"
                "  <td style='font-weight: bold;'>Release Tag:</td>"
                "  <td><a style='text-decoration: none;' href='https://github.com/alsyundawy/NotepadNext-MacOS/releases/tag/v%3'>"
                "<span style='background-color: #238636; color: #ffffff; padding: 2px 10px; border-radius: 12px; font-weight: bold; font-size: 11px;'>"
                "🏷️ v%3 Official macOS Release</span></a></td>"
                "</tr>"
                "<tr>"
                "  <td style='font-weight: bold;'>Original Author:</td>"
                "  <td>Justin Dailey (<code>@dail8859</code>)</td>"
                "</tr>"
                "<tr>"
                "  <td style='font-weight: bold;'>macOS Maintainer:</td>"
                "  <td><b>Harry Dertin Sutisna Alsyundawy</b> (<a style='color: %2; text-decoration: none;' href='https://github.com/alsyundawy'>@alsyundawy</a>) — <b>ALSYUNDAWY IT SOLUTION</b></td>"
                "</tr>"
                "<tr>"
                "  <td style='font-weight: bold;'>License:</td>"
                "  <td>GNU General Public License v3.0 (GPL-3.0-or-later)</td>"
                "</tr>"
                "<tr>"
                "  <td style='font-weight: bold;'>Core Architecture:</td>"
                "  <td>Universal 2 (Apple Silicon ARM64 &amp; Intel x86_64) &bull; Qt 6 &bull; ISO C++20</td>"
                "</tr>"
                "</table>"
                "</div>")
                .arg(txtPri, accentCol, APP_VERSION));
    }

    if (m_diagnosticsCard) {
        m_diagnosticsCard->setStyleSheet(cardStyle);
        QScreen *primary = QGuiApplication::primaryScreen();
        qreal dpr = primary ? primary->devicePixelRatio() : 1.0;

        QString infoText = QString(
                               "<div style='color: %1; line-height: 1.6;'>"
                               "<b>Operating System:</b> %2 (%3 %4)<br/>"
                               "<b>Architecture:</b> %5 (Build Target: %6)<br/>"
                               "<b>Qt Version:</b> %7 (Built with %8)<br/>"
                               "<b>Display Scale Ratio:</b> %9x (Retina / High-DPI)<br/>"
                               "<b>Notepad Next Version:</b> v" APP_VERSION "<br/>"
                               "</div>")
                               .arg(txtPri)
                               .arg(QSysInfo::prettyProductName())
                               .arg(QSysInfo::kernelType())
                               .arg(QSysInfo::kernelVersion())
                               .arg(QSysInfo::currentCpuArchitecture())
                               .arg(QSysInfo::buildCpuArchitecture())
                               .arg(qVersion())
                               .arg(QT_VERSION_STR)
                               .arg(QString::number(dpr, 'f', 1));

        m_diagnosticsCard->setText(infoText);
    }

    if (m_maintainerBrowser) {
        m_maintainerBrowser->setHtml(
            tr("<div style='color: %1; line-height: 1.6;'>"
               "<h3 style='color: %1; margin-top: 0;'>macOS Maintainer &amp; Support Channels</h3>"
               "<p><b>Lead Architect &amp; Maintainer:</b> HARRY DERTIN SUTISNA ALSYUNDAWY "
               "(<code>@alsyundawy</code>)<br/>"
               "<b>Organization:</b> ALSYUNDAWY IT SOLUTION<br/>"
               "<b>Official Website:</b> <a style='color: %2;' href='https://alsyundawy.com'>https://alsyundawy.com</a><br/>"
               "<b>Official Email:</b> <a style='color: %2;' href='mailto:alsyundawy@gmail.com'>alsyundawy@gmail.com</a><br/>"
               "<b>GitHub Profile:</b> <a style='color: %2;' href='https://github.com/alsyundawy'>@alsyundawy</a><br/>"
               "<b>Telegram Channel:</b> <a style='color: %2;' href='https://t.me/alsyundawy'>@alsyundawy</a></p>"
               "<hr style='border: 0; border-top: 1px solid %3; margin: 16px 0;'/>"
               "<h4 style='color: %1;'>Sponsorship &amp; Donation Channels</h4>"
               "<p>If you find Notepad Next for macOS useful for your development and daily workflow, consider supporting our ongoing development:</p>"
               "<ul>"
               "<li>💳 <b>PayPal:</b> <a style='color: %2;' href='https://www.paypal.me/alsyundawy'>paypal.me/alsyundawy</a></li>"
               "<li>🇮🇩 <b>QRIS (NMID: ID1020021153676):</b> <a style='color: %2;' href='https://github.com/user-attachments/assets/a0126f28-6dde-43da-ba14-d7c9a27de0df'>View Official QRIS Barcode</a></li>"
               "</ul>"
               "</div>")
                .arg(txtPri, accentCol, borderCol));
    }

    if (m_creditsBrowser) {
        m_creditsBrowser->setHtml(
            tr("<div style='color: %1; line-height: 1.6;'>"
               "<h3 style='color: %1; margin-top: 0;'>Credits &amp; Upstream Attribution</h3>"
               "<p>Notepad Next for macOS stands on the shoulders of giants. We express our deepest gratitude to the following projects and authors:</p>"
               "<ul>"
               "<li><b>Justin Dailey (dail8859):</b> Original creator and lead developer of NotepadNext. Provided the cross-platform Qt scaffolding, docking framework, and core application design. (<a style='color: %2;' href='https://github.com/dail8859/NotepadNext'>github.com/dail8859/NotepadNext</a>)</li>"
               "<li><b>Scintilla:</b> Created by Neil Hodgson and contributors. The legendary core editing component providing high-performance text rendering, multi-caret editing, and fold margins. (<a style='color: %2;' href='https://www.scintilla.org'>scintilla.org</a>)</li>"
               "<li><b>Lexilla:</b> Created by Neil Hodgson and contributors. Provides syntax highlighting and styling lexers for over 100 programming languages. (<a style='color: %2;' href='https://www.scintilla.org'>scintilla.org/Lexilla.html</a>)</li>"
               "<li><b>uchardet:</b> Maintained by Freedesktop. Enables fast and robust universal character encoding detection.</li>"
               "<li><b>Qt Project:</b> Developed by The Qt Company and the Qt Open Source Community. (<a style='color: %2;' href='https://www.qt.io'>qt.io</a>)</li>"
               "<li><b>Qt Advanced Docking System (QADS):</b> Advanced dock widget management for Qt.</li>"
               "<li><b>NoteSciPad:</b> Contributed modern Scintilla 5.6.7 / Lexilla 5.5.4 integration and multi-instance UI patterns.</li>"
               "</ul>"
               "</div>")
                .arg(txtPri, accentCol));
    }

    if (m_shortcutsBrowser) {
        m_shortcutsBrowser->setHtml(
            tr("<div style='color: %1; line-height: 1.6;'>"
               "<h3 style='color: %1; margin-top: 0;'>Key Productivity Shortcuts</h3>"
               "<table border='0' cellspacing='6' cellpadding='4' width='100%' style='color: %1;'>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + N</td><td>Create New File</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + Shift + N</td><td>Open New Window</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + O</td><td>Open File</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + S</td><td>Save File</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + Shift + S</td><td>Save As...</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + F</td><td>Find / Quick Find</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + H</td><td>Find &amp; Replace</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + Shift + F</td><td>Find in Files / Workspace</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + D</td><td>Duplicate Current Line or Selection</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + /</td><td>Toggle Single-Line Comment</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + Shift + /</td><td>Toggle Block / Stream Comment</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + Shift + U</td><td>Convert Selection to UPPERCASE</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl / Cmd + U</td><td>Convert Selection to lowercase</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl+Alt+Shift+E</td><td>Encoding Information &amp; Conversion Dialog</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl+Alt+U</td><td>Convert Document Encoding to UTF-8</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Ctrl+Alt+A</td><td>Convert Document Encoding to ANSI</td></tr>"
               "<tr><td style='color: %2; font-weight: bold;'>Alt + Click &amp; Drag</td><td>Column / Rectangular Box Selection</td></tr>"
               "</table>"
               "</div>")
                .arg(txtPri, accentCol));
    }

    if (m_licenseBrowser) {
        m_licenseBrowser->setHtml(
            tr("<div style='color: %1; line-height: 1.6;'>"
               "<h3 style='color: %1; margin-top: 0;'>GNU General Public License v3.0</h3>"
               "<p>Notepad Next is free software: you can redistribute it and/or modify "
               "it under the terms of the GNU General Public License as published by "
               "the Free Software Foundation, either version 3 of the License, or "
               "(at your option) any later version.</p>"
               "<p>This program is distributed in the hope that it will be useful, "
               "but WITHOUT ANY WARRANTY; without even the implied warranty of "
               "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
               "GNU General Public License for more details.</p>"
               "<p>For complete license details, visit: <a style='color: %2;' href='https://www.gnu.org/licenses/'>https://www.gnu.org/licenses/</a></p>"
               "</div>")
                .arg(txtPri, accentCol));
    }

    m_isUpdatingTheme = false;
}

QWidget *AboutDialog::createAboutTab() {
    QWidget *tab = new QWidget(m_tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(14);

    // Header layout with icon and title
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(16);

    QLabel *iconLabel = new QLabel(tab);
    QPixmap iconPixmap(QStringLiteral(":/icons/NotepadNext.png"));
    if (iconPixmap.isNull()) {
        iconPixmap = windowIcon().pixmap(80, 80);
    }
    if (!iconPixmap.isNull()) {
        iconLabel->setPixmap(iconPixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    iconLabel->setFixedSize(80, 80);
    headerLayout->addWidget(iconLabel);

    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(4);

    m_titleLabel = new QLabel(tab);
    titleLayout->addWidget(m_titleLabel);

    m_badgesLabel = new QLabel(tab);
    titleLayout->addWidget(m_badgesLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    layout->addLayout(headerLayout);

    // Legal notice box with dynamic theme card
    m_noticeBox = new QLabel(tab);
    m_noticeBox->setWordWrap(true);
    layout->addWidget(m_noticeBox);

    m_metaLabel = new QLabel(tab);
    m_metaLabel->setOpenExternalLinks(true);
    layout->addWidget(m_metaLabel);

    layout->addStretch();
    return tab;
}

QWidget *AboutDialog::createMaintainerTab() {
    QWidget *tab = new QWidget(m_tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    m_maintainerBrowser = new QTextBrowser(tab);
    m_maintainerBrowser->setOpenExternalLinks(true);
    layout->addWidget(m_maintainerBrowser);
    return tab;
}

QWidget *AboutDialog::createCreditsTab() {
    QWidget *tab = new QWidget(m_tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);

    m_creditsBrowser = new QTextBrowser(tab);
    m_creditsBrowser->setOpenExternalLinks(true);
    layout->addWidget(m_creditsBrowser);
    return tab;
}

QWidget *AboutDialog::createEnvironmentTab() {
    QWidget *tab = new QWidget(m_tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(14);

    m_diagnosticsCard = new QLabel(tab);
    m_diagnosticsCard->setTextFormat(Qt::RichText);
    layout->addWidget(m_diagnosticsCard);

    QPushButton *copyBtn = new QPushButton(tr("Copy Diagnostics to Clipboard"), tab);
    copyBtn->setCursor(Qt::PointingHandCursor);
    connect(copyBtn, &QPushButton::clicked, this, &AboutDialog::copySystemInfoToClipboard);
    layout->addWidget(copyBtn);

    layout->addStretch();
    return tab;
}

QWidget *AboutDialog::createShortcutsTab() {
    QWidget *tab = new QWidget(m_tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);

    m_shortcutsBrowser = new QTextBrowser(tab);
    m_shortcutsBrowser->setOpenExternalLinks(false);
    layout->addWidget(m_shortcutsBrowser);
    return tab;
}

QWidget *AboutDialog::createLicenseTab() {
    QWidget *tab = new QWidget(m_tabWidget);
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);

    m_licenseBrowser = new QTextBrowser(tab);
    m_licenseBrowser->setOpenExternalLinks(true);
    layout->addWidget(m_licenseBrowser);
    return tab;
}

void AboutDialog::copySystemInfoToClipboard() {
    QScreen *primary = QGuiApplication::primaryScreen();
    qreal dpr = primary ? primary->devicePixelRatio() : 1.0;

    QString plainInfo = QString(
                            "Notepad Next for macOS: v%1\n"
                            "OS: %2 (%3)\n"
                            "Kernel: %4\n"
                            "CPU: %5 (Build Target: %6)\n"
                            "Qt: %7 (Built with %8)\n"
                            "Display Scale: %9x\n")
                            .arg(APP_VERSION)
                            .arg(QSysInfo::prettyProductName())
                            .arg(QSysInfo::kernelType())
                            .arg(QSysInfo::kernelVersion())
                            .arg(QSysInfo::currentCpuArchitecture())
                            .arg(QSysInfo::buildCpuArchitecture())
                            .arg(qVersion())
                            .arg(QT_VERSION_STR)
                            .arg(QString::number(dpr, 'f', 1));

    QClipboard *clipboard = QGuiApplication::clipboard();
    if (clipboard) {
        clipboard->setText(plainInfo);
    }
}
