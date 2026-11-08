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

#ifndef EXPORTOPTIONSDIALOG_H
#define EXPORTOPTIONSDIALOG_H

#include <QDialog>
#include "../features/DocumentExportManager.h"

class QComboBox;
class QCheckBox;
class QLineEdit;
class QDialogButtonBox;

class ExportOptionsDialog : public QDialog {
    Q_OBJECT

public:
    enum class TargetFormat { Html, Pdf };

    explicit ExportOptionsDialog(TargetFormat format, const QString &defaultTitle = QString(),
                                 QWidget *parent = nullptr);
    ~ExportOptionsDialog() override = default;

    [[nodiscard]] DocumentExportManager::ExportOptions options() const;

private:
    void setupUi();

    TargetFormat m_format;
    QString m_initialTitle;

    QLineEdit *editTitle = nullptr;
    QComboBox *comboPageSize = nullptr;
    QComboBox *comboOrientation = nullptr;
    QComboBox *comboMargins = nullptr;
    QCheckBox *chkIncludeCss = nullptr;
    QCheckBox *chkDarkTheme = nullptr;
    QCheckBox *chkUsePandoc = nullptr;
    QDialogButtonBox *buttonBox = nullptr;
};

#endif  // EXPORTOPTIONSDIALOG_H
