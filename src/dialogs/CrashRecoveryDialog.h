#pragma once
/*
 * This file is part of Notepad Next (macOS Edition).
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <QDialog>
#include <QList>
#include "../features/AutosaveRecoveryManager.h"

class QTableWidget;
class QPushButton;

class CrashRecoveryDialog : public QDialog {
    Q_OBJECT

public:
    explicit CrashRecoveryDialog(const QList<RecoverySnapshot> &snapshots, QWidget *parent = nullptr);
    ~CrashRecoveryDialog() override = default;

    [[nodiscard]] const QList<RecoverySnapshot> &selectedSnapshots() const;
    [[nodiscard]] bool discardRequested() const { return m_discardRequested; }

private slots:
    void onRecoverClicked();
    void onDiscardClicked();

private:
    void setupUi();

    QList<RecoverySnapshot> m_snapshots;
    QList<RecoverySnapshot> m_selectedSnapshots;
    QTableWidget *m_table = nullptr;
    QPushButton *btnRecover = nullptr;
    QPushButton *btnDiscard = nullptr;
    QPushButton *btnCancel = nullptr;
    bool m_discardRequested{false};
};
