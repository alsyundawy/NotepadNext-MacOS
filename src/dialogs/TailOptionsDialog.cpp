/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Implements Tail Monitoring Options Dialog.
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "TailOptionsDialog.h"
#include "ui_TailOptionsDialog.h"
#include "TailMonitoringManager.h"
#include "ScintillaNext.h"
#include <QWidget>

TailOptionsDialog::TailOptionsDialog(QWidget *parent, ScintillaNext *currentEditor)
    : QDialog(parent), ui(std::make_unique<Ui::TailOptionsDialog>()), m_editor(currentEditor) {
    ui->setupUi(this);

    loadSettings();

    connect(ui->btnBox, &QDialogButtonBox::accepted, this, &TailOptionsDialog::onApplyClicked);
    connect(ui->btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

TailOptionsDialog::~TailOptionsDialog() = default;

void TailOptionsDialog::loadSettings() {
    const auto *mon = TailMonitoringManager::instance();

    // Poll interval
    const int currentInterval = mon->pollIntervalMs();
    int intervalIndex = 1;  // default 750
    if (currentInterval <= 250)
        intervalIndex = 0;
    else if (currentInterval <= 500)
        intervalIndex = 1;
    else if (currentInterval <= 750)
        intervalIndex = 2;
    else if (currentInterval <= 1000)
        intervalIndex = 3;
    else if (currentInterval <= 2000)
        intervalIndex = 4;
    else
        intervalIndex = 5;
    ui->comboPollInterval->setCurrentIndex(intervalIndex);

    // Read only lock
    ui->chkReadOnly->setChecked(mon->readOnlyLock());

    // Auto scroll for current editor
    if (m_editor) {
        ui->chkAutoScroll->setChecked(mon->autoScrollToEnd(m_editor));
    } else {
        ui->chkAutoScroll->setChecked(true);
    }

    // Max chunk
    const qint64 maxChunk = mon->maxChunkBytes();
    if (maxChunk <= 1024 * 1024)
        ui->comboMaxChunk->setCurrentIndex(0);
    else if (maxChunk <= 5 * 1024 * 1024)
        ui->comboMaxChunk->setCurrentIndex(1);
    else if (maxChunk <= 10 * 1024 * 1024)
        ui->comboMaxChunk->setCurrentIndex(2);
    else
        ui->comboMaxChunk->setCurrentIndex(3);
}

void TailOptionsDialog::onApplyClicked() {
    auto *mon = TailMonitoringManager::instance();

    int intervalMs = 750;
    switch (ui->comboPollInterval->currentIndex()) {
        case 0:
            intervalMs = 250;
            break;
        case 1:
            intervalMs = 500;
            break;
        case 2:
            intervalMs = 750;
            break;
        case 3:
            intervalMs = 1000;
            break;
        case 4:
            intervalMs = 2000;
            break;
        case 5:
            intervalMs = 5000;
            break;
        default:
            intervalMs = 750;
            break;
    }
    mon->setPollIntervalMs(intervalMs);

    mon->setReadOnlyLock(ui->chkReadOnly->isChecked());

    if (m_editor) {
        mon->setAutoScrollToEnd(m_editor, ui->chkAutoScroll->isChecked());
    }

    qint64 maxChunk = 10 * 1024 * 1024;
    switch (ui->comboMaxChunk->currentIndex()) {
        case 0:
            maxChunk = 1024 * 1024;
            break;
        case 1:
            maxChunk = 5 * 1024 * 1024;
            break;
        case 2:
            maxChunk = 10 * 1024 * 1024;
            break;
        case 3:
            maxChunk = 25 * 1024 * 1024;
            break;
        default:
            maxChunk = 10 * 1024 * 1024;
            break;
    }
    mon->setMaxChunkBytes(maxChunk);

    accept();
}
