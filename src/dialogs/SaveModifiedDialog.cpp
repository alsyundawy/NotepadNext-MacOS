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

#include "SaveModifiedDialog.h"
#include "ScintillaNext.h"
#include "features/ThemeManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QKeyEvent>
#include <QEvent>
#include <QFileInfo>
#include <QPixmap>
#include <QApplication>

SaveModifiedDialog::SaveModifiedDialog(const QVector<ScintillaNext *> &editors, QWidget *parent)
    : QDialog(parent)
{
    for (auto *e : editors) {
        if (e) {
            m_editors.append(e);
        }
    }
    setupUi();
    updateThemeColors();

    connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &SaveModifiedDialog::updateThemeColors);
    connect(ThemeManager::instance(), &ThemeManager::paletteChanged, this, &SaveModifiedDialog::updateThemeColors);
}

void SaveModifiedDialog::setupUi()
{
    setWindowTitle(tr("Save Changes"));
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setWindowModality(Qt::WindowModal);
    setMinimumWidth(500);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 16);
    mainLayout->setSpacing(16);

    // Top section: Icon + Text
    auto *topLayout = new QHBoxLayout();
    topLayout->setSpacing(16);
    topLayout->setAlignment(Qt::AlignTop);

    m_iconLabel = new QLabel(this);
    QPixmap iconPixmap(QStringLiteral(":/icons/NotepadNext.png"));
    if (iconPixmap.isNull()) {
        iconPixmap = windowIcon().pixmap(64, 64);
    }
    if (!iconPixmap.isNull()) {
        m_iconLabel->setPixmap(iconPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    m_iconLabel->setFixedSize(64, 64);
    topLayout->addWidget(m_iconLabel);

    auto *contentLayout = new QVBoxLayout();
    contentLayout->setSpacing(6);

    const auto count = static_cast<int>(m_editors.count());

    m_headingLabel = new QLabel(this);
    m_headingLabel->setWordWrap(true);
    QFont headingFont = m_headingLabel->font();
    headingFont.setPointSize(headingFont.pointSize() + 2);
    headingFont.setBold(true);
    m_headingLabel->setFont(headingFont);

    if (count == 1) {
        QString docName = (m_editors.first() && !m_editors.first().isNull()) ? m_editors.first()->getName() : tr("Untitled");
        m_headingLabel->setText(tr("Do you want to save the changes made to the document \"%1\"?").arg(docName));
    } else {
        m_headingLabel->setText(tr("There are %n documents with unsaved changes. Save them before closing?", "", count));
    }
    contentLayout->addWidget(m_headingLabel);

    m_subLabel = new QLabel(tr("Your changes will be permanently lost if you don't save them."), this);
    m_subLabel->setWordWrap(true);
    contentLayout->addWidget(m_subLabel);

    // Document detail card
    if (count == 1) {
        m_cardWidget = new QWidget(this);
        auto *cardLayout = new QVBoxLayout(m_cardWidget);
        cardLayout->setContentsMargins(12, 10, 12, 10);
        cardLayout->setSpacing(4);

        QString filePath;
        if (m_editors.first() && !m_editors.first().isNull() && m_editors.first()->isFile()) {
            filePath = m_editors.first()->getFilePath();
        }
        if (filePath.isEmpty()) {
            filePath = tr("New unsaved document");
        }

        m_singleFileLabel = new QLabel(QStringLiteral("📄 %1").arg(filePath), m_cardWidget);
        m_singleFileLabel->setWordWrap(true);
        cardLayout->addWidget(m_singleFileLabel);

        contentLayout->addWidget(m_cardWidget);
    } else {
        m_fileListWidget = new QListWidget(this);
        m_fileListWidget->setFixedHeight(120);
        m_fileListWidget->setSelectionMode(QAbstractItemView::NoSelection);

        for (const auto &editor : m_editors) {
            if (!editor || editor.isNull()) {
                continue;
            }
            QString name = editor->getName();
            QString path = editor->isFile() ? editor->getFilePath() : QString();
            QString labelText = path.isEmpty() ? QStringLiteral("📄 %1  [New]").arg(name)
                                               : QStringLiteral("📄 %1  (%2)").arg(name, path);
            auto *item = new QListWidgetItem(labelText, m_fileListWidget);
            m_fileListWidget->addItem(item);
        }
        contentLayout->addWidget(m_fileListWidget);
    }

    topLayout->addLayout(contentLayout);
    mainLayout->addLayout(topLayout);

    // Button Row (macOS HIG Layout: Destructive on Left, Cancel + Save on Right)
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    m_discardBtn = new QPushButton(count > 1 ? tr("Don't Save All") : tr("Don't Save"), this);
    m_discardBtn->setFocusPolicy(Qt::NoFocus);
    m_discardBtn->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(m_discardBtn);

    buttonLayout->addStretch();

    m_cancelBtn = new QPushButton(tr("Cancel"), this);
    m_cancelBtn->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(m_cancelBtn);

    m_saveBtn = new QPushButton(count > 1 ? tr("Save All") : tr("Save"), this);
    m_saveBtn->setDefault(true);
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(m_saveBtn);

    mainLayout->addLayout(buttonLayout);

    connect(m_saveBtn, &QPushButton::clicked, this, &SaveModifiedDialog::onSaveClicked);
    connect(m_discardBtn, &QPushButton::clicked, this, &SaveModifiedDialog::onDiscardClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &SaveModifiedDialog::onCancelClicked);
}

void SaveModifiedDialog::changeEvent(QEvent *event)
{
    QDialog::changeEvent(event);
    if ((event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) && !m_isUpdatingTheme) {
        updateThemeColors();
    }
}

void SaveModifiedDialog::keyPressEvent(QKeyEvent *event)
{
    const Qt::KeyboardModifiers mods = event->modifiers();
    const bool isMacCmd = (mods & Qt::ControlModifier) || (mods & Qt::MetaModifier);

    if (isMacCmd && event->key() == Qt::Key_D) {
        onDiscardClicked();
        return;
    }
    if (isMacCmd && event->key() == Qt::Key_S) {
        onSaveClicked();
        return;
    }
    if (event->key() == Qt::Key_Escape || (isMacCmd && event->key() == Qt::Key_Period)) {
        onCancelClicked();
        return;
    }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        onSaveClicked();
        return;
    }

    QDialog::keyPressEvent(event);
}

void SaveModifiedDialog::onSaveClicked()
{
    m_action = UserAction::Save;
    accept();
}

void SaveModifiedDialog::onDiscardClicked()
{
    m_action = UserAction::Discard;
    accept();
}

void SaveModifiedDialog::onCancelClicked()
{
    m_action = UserAction::Cancel;
    reject();
}

void SaveModifiedDialog::updateThemeColors()
{
    if (m_isUpdatingTheme) {
        return;
    }
    m_isUpdatingTheme = true;

    const bool dark = ThemeManager::instance()->isDarkActive();
    const ThemePalette *pal = ThemeManager::instance()->currentPalette();

    QString winBg = dark ? QStringLiteral("#1e1e1e") : QStringLiteral("#f0f2f5");
    QString cardBg = dark ? QStringLiteral("#25282c") : QStringLiteral("#fafbfc");
    QString borderCol = dark ? QStringLiteral("#3e3e42") : QStringLiteral("#cbd5e1");
    QString txtPri = dark ? QStringLiteral("#e0e0e0") : QStringLiteral("#24292f");
    QString txtSec = dark ? QStringLiteral("#94a3b8") : QStringLiteral("#64748b");
    QString accentCol = dark ? QStringLiteral("#38bdf8") : QStringLiteral("#0078d4");

    if (pal) {
        winBg = QString::fromLatin1(pal->win);
        cardBg = QString::fromLatin1(pal->card);
        borderCol = QString::fromLatin1(pal->border);
        txtPri = QString::fromLatin1(pal->txtPri);
        txtSec = QString::fromLatin1(pal->txtSec);
        accentCol = QString::fromLatin1(pal->blue);
    }

    const QString dialogStyle =
        QString(
            "QDialog {"
            "  background-color: %1;"
            "  color: %2;"
            "  font-family: -apple-system, BlinkMacSystemFont, 'SF Pro Text', 'Segoe UI', Roboto, sans-serif;"
            "}"
            "QPushButton {"
            "  background-color: %3;"
            "  color: %2;"
            "  border: 1px solid %4;"
            "  border-radius: 6px;"
            "  padding: 6px 16px;"
            "  font-size: 13px;"
            "  font-weight: 500;"
            "}"
            "QPushButton:hover {"
            "  background-color: %4;"
            "}"
            "QPushButton:default {"
            "  background-color: %5;"
            "  color: #ffffff;"
            "  border: 1px solid %5;"
            "  font-weight: 600;"
            "}"
            "QPushButton:default:hover {"
            "  background-color: %6;"
            "}"
            "QListWidget {"
            "  background-color: %3;"
            "  color: %2;"
            "  border: 1px solid %4;"
            "  border-radius: 6px;"
            "  padding: 4px;"
            "  font-size: 12px;"
            "}")
            .arg(winBg, txtPri, cardBg, borderCol, accentCol, dark ? QStringLiteral("#0284c7") : QStringLiteral("#005a9e"));

    setStyleSheet(dialogStyle);

    if (m_headingLabel) {
        m_headingLabel->setStyleSheet(QStringLiteral("color: %1;").arg(txtPri));
    }
    if (m_subLabel) {
        m_subLabel->setStyleSheet(QStringLiteral("color: %1; font-size: 13px;").arg(txtSec));
    }
    if (m_cardWidget) {
        m_cardWidget->setStyleSheet(
            QStringLiteral("QWidget { background-color: %1; border: 1px solid %2; border-radius: 6px; }")
                .arg(cardBg, borderCol));
    }
    if (m_singleFileLabel) {
        m_singleFileLabel->setStyleSheet(QStringLiteral("color: %1; font-size: 12px; border: none;").arg(txtPri));
    }

    m_isUpdatingTheme = false;
}
