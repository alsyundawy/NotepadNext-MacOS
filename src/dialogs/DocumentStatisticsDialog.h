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

#ifndef DOCUMENTSTATISTICSDIALOG_H
#define DOCUMENTSTATISTICSDIALOG_H

#include <QDialog>
#include "../utils/TokenForensicUtils.h"

class QLabel;
class QPushButton;
class QTableWidget;

class DocumentStatisticsDialog : public QDialog {
    Q_OBJECT

public:
    explicit DocumentStatisticsDialog(const QString &documentName, const TokenForensicUtils::DocumentStats &stats,
                                      QWidget *parent = nullptr);
    ~DocumentStatisticsDialog() override = default;

private slots:
    void onCopyStatsClicked();

private:
    void setupUi(const QString &documentName, const TokenForensicUtils::DocumentStats &stats);

    QTableWidget *m_tableWidget{nullptr};
    QPushButton *m_copyButton{nullptr};
    QPushButton *m_closeButton{nullptr};
    QString m_statsSummaryText;
};

#endif  // DOCUMENTSTATISTICSDIALOG_H
