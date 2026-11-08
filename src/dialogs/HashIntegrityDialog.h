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

#ifndef HASHINTEGRITYDIALOG_H
#define HASHINTEGRITYDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QCryptographicHash>
#include "../utils/HashIntegrityManager.h"

class ScintillaNext;

class HashIntegrityDialog : public QDialog {
    Q_OBJECT

public:
    explicit HashIntegrityDialog(QWidget *parent = nullptr, ScintillaNext *currentEditor = nullptr);
    ~HashIntegrityDialog() override = default;

private slots:
    void onBrowseFileClicked();
    void onComputeHashClicked();
    void onExpectedHashChanged(const QString &text);
    void onCopyComputedClicked() const;

private:
    void updateComparisonStatus();
    [[nodiscard]] QCryptographicHash::Algorithm selectedAlgorithm() const;

    QRadioButton *radioFile = nullptr;
    QRadioButton *radioCurrentEditor = nullptr;
    QLineEdit *editFilePath = nullptr;
    QPushButton *btnBrowse = nullptr;
    QComboBox *comboAlgorithm = nullptr;
    QPushButton *btnCompute = nullptr;
    QLineEdit *editComputedHash = nullptr;
    QPushButton *btnCopy = nullptr;
    QLineEdit *editExpectedHash = nullptr;
    QLabel *labelComparisonResult = nullptr;

    ScintillaNext *editor = nullptr;
};

#endif  // HASHINTEGRITYDIALOG_H
