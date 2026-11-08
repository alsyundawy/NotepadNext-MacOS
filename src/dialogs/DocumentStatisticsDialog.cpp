/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "DocumentStatisticsDialog.h"
#include "../utils/TokenForensicUtils.h"

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

DocumentStatisticsDialog::DocumentStatisticsDialog(const QString &documentName,
                                                   const TokenForensicUtils::DocumentStats &stats, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(tr("Document Statistics & Forensics"));
    setMinimumWidth(460);
    setMinimumHeight(380);

    setupUi(documentName, stats);
}

void DocumentStatisticsDialog::setupUi(const QString &documentName, const TokenForensicUtils::DocumentStats &stats) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(10);

    auto *headerLabel = new QLabel(tr("Statistics for: <b>%1</b>").arg(documentName), this);
    headerLabel->setTextFormat(Qt::RichText);
    layout->addWidget(headerLabel);

    m_tableWidget = new QTableWidget(8, 2, this);
    m_tableWidget->setHorizontalHeaderLabels({tr("Metric"), tr("Value")});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setAlternatingRowColors(true);

    auto setRow = [this](int row, const QString &metric, const QString &val) {
        auto *itemMetric = new QTableWidgetItem(metric);
        auto *itemVal = new QTableWidgetItem(val);
        m_tableWidget->setItem(row, 0, itemMetric);
        m_tableWidget->setItem(row, 1, itemVal);
    };

    setRow(0, tr("Lines (Total / Non-empty)"), tr("%1 / %2").arg(stats.lines).arg(stats.nonEmptyLines));
    setRow(1, tr("Words Count"), QString::number(stats.words));
    setRow(2, tr("Characters (with spaces)"), QString::number(stats.totalChars));
    setRow(3, tr("Characters (without spaces)"), QString::number(stats.charsNoSpaces));
    setRow(4, tr("Size (UTF-8 Bytes)"), tr("%1 bytes").arg(stats.bytesUtf8));

    int readMin = static_cast<int>(stats.readingTimeMinutes);
    int readSec = static_cast<int>((stats.readingTimeMinutes - readMin) * 60);
    setRow(5, tr("Estimated Reading Time"), tr("%1 min %2 sec (at 200 wpm)").arg(readMin).arg(readSec));

    int speakMin = static_cast<int>(stats.speakingTimeMinutes);
    int speakSec = static_cast<int>((stats.speakingTimeMinutes - speakMin) * 60);
    setRow(6, tr("Estimated Speaking Time"), tr("%1 min %2 sec (at 130 wpm)").arg(speakMin).arg(speakSec));

    QString entropyStr = tr("%1 / 8.00 bits/byte").arg(QString::number(stats.shannonEntropy, 'f', 2));
    if (stats.isHighEntropy) {
        entropyStr += tr(" — High (Possible Encrypted/Obfuscated Payload)");
    } else {
        entropyStr += tr(" — Normal text/source code");
    }
    setRow(7, tr("Shannon Entropy"), entropyStr);

    layout->addWidget(m_tableWidget);

    m_statsSummaryText = tr("=== Document Statistics for %1 ===\n"
                            "Lines: %2 (Non-empty: %3)\n"
                            "Words: %4\n"
                            "Characters (with spaces): %5\n"
                            "Characters (without spaces): %6\n"
                            "Bytes (UTF-8): %7\n"
                            "Estimated Reading Time: %8 min %9 sec\n"
                            "Estimated Speaking Time: %10 min %11 sec\n"
                            "Shannon Entropy: %12\n")
                             .arg(documentName)
                             .arg(stats.lines)
                             .arg(stats.nonEmptyLines)
                             .arg(stats.words)
                             .arg(stats.totalChars)
                             .arg(stats.charsNoSpaces)
                             .arg(stats.bytesUtf8)
                             .arg(readMin)
                             .arg(readSec)
                             .arg(speakMin)
                             .arg(speakSec)
                             .arg(entropyStr);

    auto *btnLayout = new QHBoxLayout();
    m_copyButton = new QPushButton(tr("Copy to Clipboard"), this);
    connect(m_copyButton, &QPushButton::clicked, this, &DocumentStatisticsDialog::onCopyStatsClicked);
    btnLayout->addWidget(m_copyButton);

    btnLayout->addStretch();

    m_closeButton = new QPushButton(tr("Close"), this);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(m_closeButton);

    layout->addLayout(btnLayout);
}

void DocumentStatisticsDialog::onCopyStatsClicked() {
    QApplication::clipboard()->setText(m_statsSummaryText);
    m_copyButton->setText(tr("Copied!"));
}
