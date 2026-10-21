#pragma once
/*
 * This file is part of Notepad Next.
 * Copyright 2026 Harry DS Alsyundawy (Notepad Next macOS Maintainer)
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef SAVEMODIFIEDDIALOG_H
#define SAVEMODIFIEDDIALOG_H

#include <QDialog>
#include <QVector>

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

    QVector<ScintillaNext *> m_editors;
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
