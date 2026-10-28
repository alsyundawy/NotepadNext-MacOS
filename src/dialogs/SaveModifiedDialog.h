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

#ifndef SAVEMODIFIEDDIALOG_H
#define SAVEMODIFIEDDIALOG_H

#include <QDialog>
#include <QVector>
#include <QPointer>

class ScintillaNext;
class QLabel;
class QPushButton;
class QListWidget;

class SaveModifiedDialog : public QDialog {
    Q_OBJECT

public:
    enum class UserAction {
        Save,
        Discard,
        Cancel
    };

    explicit SaveModifiedDialog(const QVector<ScintillaNext *> &editors, QWidget *parent = nullptr);
    ~SaveModifiedDialog() override = default;

    UserAction userAction() const { return m_action; }

protected:
    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onSaveClicked();
    void onDiscardClicked();
    void onCancelClicked();
    void updateThemeColors();

private:
    void setupUi();

    QVector<QPointer<ScintillaNext>> m_editors;
    UserAction m_action = UserAction::Cancel;
    bool m_isUpdatingTheme = false;

    QLabel *m_iconLabel = nullptr;
    QLabel *m_headingLabel = nullptr;
    QLabel *m_subLabel = nullptr;
    QWidget *m_cardWidget = nullptr;
    QLabel *m_singleFileLabel = nullptr;
    QListWidget *m_fileListWidget = nullptr;

    QPushButton *m_discardBtn = nullptr;
    QPushButton *m_cancelBtn = nullptr;
    QPushButton *m_saveBtn = nullptr;
};

#endif  // SAVEMODIFIEDDIALOG_H
