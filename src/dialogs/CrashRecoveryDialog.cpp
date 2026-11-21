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

#include "CrashRecoveryDialog.h"
#include <QDateTime>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

CrashRecoveryDialog::CrashRecoveryDialog(const QList<RecoverySnapshot> &snapshots, QWidget *parent)
    : QDialog(parent), m_snapshots(snapshots) {
    setWindowTitle(tr("Document Crash Recovery"));
    resize(640, 360);
    setupUi();
}

void CrashRecoveryDialog::setupUi() {
    auto *const layout = new QVBoxLayout(this);

    auto *const lblInfo = new QLabel(tr("Notepad Next detected unsaved draft documents from a previous session.\n"
                                        "Select which documents you would like to recover:"),
                                     this);
    lblInfo->setWordWrap(true);
    layout->addWidget(lblInfo);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels(
        {tr("Recover"), tr("Document Name"), tr("Original Path"), tr("Auto-Saved Time")});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setRowCount(static_cast<int>(m_snapshots.size()));

    for (qsizetype i = 0; i < m_snapshots.size(); ++i) {
        const auto &snap = m_snapshots.at(i);

        auto *const chkItem = new QTableWidgetItem();
        chkItem->setCheckState(Qt::Checked);
        m_table->setItem(static_cast<int>(i), 0, chkItem);

        auto *const titleItem = new QTableWidgetItem(snap.title.isEmpty() ? tr("Untitled") : snap.title);
        m_table->setItem(static_cast<int>(i), 1, titleItem);

        auto *const pathItem = new QTableWidgetItem(snap.isUntitled ? tr("[Untitled Draft]") : snap.originalPath);
        m_table->setItem(static_cast<int>(i), 2, pathItem);

        const QString timeStr =
            QDateTime::fromSecsSinceEpoch(snap.timestamp).toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
        auto *const timeItem = new QTableWidgetItem(timeStr);
        m_table->setItem(static_cast<int>(i), 3, timeItem);
    }
    layout->addWidget(m_table);

    auto *const btnLayout = new QHBoxLayout();
    btnDiscard = new QPushButton(tr("Discard All Drafts"), this);
    btnDiscard->setObjectName(QStringLiteral("btnDiscard"));
    connect(btnDiscard, &QPushButton::clicked, this, &CrashRecoveryDialog::onDiscardClicked);
    btnLayout->addWidget(btnDiscard);

    btnLayout->addStretch(1);

    btnCancel = new QPushButton(tr("Cancel"), this);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(btnCancel);

    btnRecover = new QPushButton(tr("Recover Selected"), this);
    btnRecover->setDefault(true);
    connect(btnRecover, &QPushButton::clicked, this, &CrashRecoveryDialog::onRecoverClicked);
    btnLayout->addWidget(btnRecover);

    layout->addLayout(btnLayout);
}

void CrashRecoveryDialog::onRecoverClicked() {
    m_selectedSnapshots.clear();
    for (qsizetype i = 0; i < m_snapshots.size(); ++i) {
        if (m_table->item(static_cast<int>(i), 0)->checkState() == Qt::Checked) {
            m_selectedSnapshots.append(m_snapshots.at(i));
        }
    }
    accept();
}

void CrashRecoveryDialog::onDiscardClicked() {
    m_discardRequested = true;
    m_selectedSnapshots.clear();
    accept();
}

const QList<RecoverySnapshot> &CrashRecoveryDialog::selectedSnapshots() const {
    return m_selectedSnapshots;
}
