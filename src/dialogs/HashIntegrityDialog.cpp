/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "HashIntegrityDialog.h"
#include "../ScintillaNext.h"
#include "../utils/HashIntegrityManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QClipboard>
#include <QApplication>
#include <QFileInfo>
#include <QListView>

HashIntegrityDialog::HashIntegrityDialog(QWidget *parent, ScintillaNext *currentEditor)
    : QDialog(parent), editor(currentEditor) {
    setWindowTitle(tr("Hash & Authenticity Validator"));
    setMinimumWidth(560);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    auto *sourceLayout = new QHBoxLayout();
    radioFile = new QRadioButton(tr("File on Disk"), this);
    radioCurrentEditor = new QRadioButton(tr("Current Editor Text"), this);
    sourceLayout->addWidget(radioFile);
    sourceLayout->addWidget(radioCurrentEditor);
    sourceLayout->addStretch();
    mainLayout->addLayout(sourceLayout);

    auto *fileRowLayout = new QHBoxLayout();
    editFilePath = new QLineEdit(this);
    editFilePath->setPlaceholderText(tr("Select file to hash..."));
    btnBrowse = new QPushButton(tr("&Browse..."), this);
    fileRowLayout->addWidget(editFilePath);
    fileRowLayout->addWidget(btnBrowse);
    mainLayout->addLayout(fileRowLayout);

    auto *formLayout = new QFormLayout();
    formLayout->setSpacing(8);

    comboAlgorithm = new QComboBox(this);
    comboAlgorithm->setView(new QListView(comboAlgorithm));
    comboAlgorithm->addItem(QStringLiteral("SHA-256 (Standard / Recommended)"),
                            static_cast<int>(QCryptographicHash::Sha256));
    comboAlgorithm->addItem(QStringLiteral("SHA-384"), static_cast<int>(QCryptographicHash::Sha384));
    comboAlgorithm->addItem(QStringLiteral("SHA-512 (High Security)"), static_cast<int>(QCryptographicHash::Sha512));
    comboAlgorithm->addItem(QStringLiteral("SHA3-1 (SHA3-224)"), static_cast<int>(QCryptographicHash::Sha3_224));
    comboAlgorithm->addItem(QStringLiteral("SHA3-256"), static_cast<int>(QCryptographicHash::Sha3_256));
    comboAlgorithm->addItem(QStringLiteral("SHA3-384"), static_cast<int>(QCryptographicHash::Sha3_384));
    comboAlgorithm->addItem(QStringLiteral("SHA3-512"), static_cast<int>(QCryptographicHash::Sha3_512));
    comboAlgorithm->addItem(QStringLiteral("SHA-1"), static_cast<int>(QCryptographicHash::Sha1));
    comboAlgorithm->addItem(QStringLiteral("MD5"), static_cast<int>(QCryptographicHash::Md5));
    formLayout->addRow(tr("Hash Algorithm:"), comboAlgorithm);

    mainLayout->addLayout(formLayout);

    btnCompute = new QPushButton(tr("🔒 Compute Hash"), this);
    btnCompute->setDefault(true);
    mainLayout->addWidget(btnCompute);

    auto *outputForm = new QFormLayout();
    outputForm->setSpacing(8);

    auto *computedLayout = new QHBoxLayout();
    editComputedHash = new QLineEdit(this);
    editComputedHash->setReadOnly(true);
    editComputedHash->setPlaceholderText(tr("Computed checksum will appear here..."));
    btnCopy = new QPushButton(tr("Copy"), this);
    computedLayout->addWidget(editComputedHash);
    computedLayout->addWidget(btnCopy);
    outputForm->addRow(tr("Computed Hash:"), computedLayout);

    editExpectedHash = new QLineEdit(this);
    editExpectedHash->setPlaceholderText(tr("Paste expected release hash to verify authenticity..."));
    outputForm->addRow(tr("Expected Hash:"), editExpectedHash);

    mainLayout->addLayout(outputForm);

    labelComparisonResult = new QLabel(this);
    labelComparisonResult->setAlignment(Qt::AlignCenter);
    labelComparisonResult->setStyleSheet(QStringLiteral(
        "padding: 8px; border-radius: 4px; font-weight: bold; background: rgba(100, 116, 139, 0.1); color: #64748b;"));
    labelComparisonResult->setText(tr("Enter expected hash above to compare automatically."));
    mainLayout->addWidget(labelComparisonResult);

    auto *btnBox = new QHBoxLayout();
    btnBox->addStretch();
    auto *btnClose = new QPushButton(tr("Close"), this);
    btnBox->addWidget(btnClose);
    mainLayout->addLayout(btnBox);

    if (editor && editor->isFile()) {
        radioFile->setChecked(true);
        editFilePath->setText(editor->getFilePath());
    } else if (editor) {
        radioCurrentEditor->setChecked(true);
        editFilePath->setEnabled(false);
        btnBrowse->setEnabled(false);
    } else {
        radioFile->setChecked(true);
    }

    connect(radioFile, &QRadioButton::toggled, this, [this](bool checked) {
        editFilePath->setEnabled(checked);
        btnBrowse->setEnabled(checked);
    });
    connect(btnBrowse, &QPushButton::clicked, this, &HashIntegrityDialog::onBrowseFileClicked);
    connect(btnCompute, &QPushButton::clicked, this, &HashIntegrityDialog::onComputeHashClicked);
    connect(btnCopy, &QPushButton::clicked, this, &HashIntegrityDialog::onCopyComputedClicked);
    connect(editExpectedHash, &QLineEdit::textChanged, this, &HashIntegrityDialog::onExpectedHashChanged);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

void HashIntegrityDialog::onBrowseFileClicked() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Select File to Hash"), editFilePath->text());
    if (!path.isEmpty()) {
        editFilePath->setText(path);
    }
}

QCryptographicHash::Algorithm HashIntegrityDialog::selectedAlgorithm() const {
    return static_cast<QCryptographicHash::Algorithm>(comboAlgorithm->currentData().toInt());
}

void HashIntegrityDialog::onComputeHashClicked() {
    const auto algo = selectedAlgorithm();
    QString hash;

    if (radioFile->isChecked()) {
        const QString path = editFilePath->text().trimmed();
        if (path.isEmpty() || !QFileInfo::exists(path)) {
            labelComparisonResult->setText(tr("Error: Specified file does not exist."));
            labelComparisonResult->setStyleSheet(
                QStringLiteral("padding: 8px; border-radius: 4px; font-weight: bold; background: rgba(239, 68, 68, "
                               "0.15); color: #ef4444;"));
            return;
        }
        bool ok = false;
        hash = HashIntegrityManager::computeFileHash(path, algo, &ok);
        if (!ok) {
            labelComparisonResult->setText(tr("Error: Failed to read file for hash computation."));
            labelComparisonResult->setStyleSheet(
                QStringLiteral("padding: 8px; border-radius: 4px; font-weight: bold; background: rgba(239, 68, 68, "
                               "0.15); color: #ef4444;"));
            return;
        }
    } else if (editor) {
        hash = HashIntegrityManager::computeTextHash(
            QString::fromUtf8(editor->get_text_range(0, editor->length())), algo);
    }

    editComputedHash->setText(hash);
    updateComparisonStatus();
}

void HashIntegrityDialog::onCopyComputedClicked() const {
    const QString hash = editComputedHash->text();
    if (!hash.isEmpty()) {
        QApplication::clipboard()->setText(hash);
    }
}

void HashIntegrityDialog::onExpectedHashChanged(const QString & /*text*/) {
    updateComparisonStatus();
}

void HashIntegrityDialog::updateComparisonStatus() {
    const QString computed = editComputedHash->text().trimmed();
    const QString expected = editExpectedHash->text().trimmed();

    if (computed.isEmpty()) {
        labelComparisonResult->setText(tr("Click 'Compute Hash' to generate checksum."));
        labelComparisonResult->setStyleSheet(
            QStringLiteral("padding: 8px; border-radius: 4px; font-weight: bold; background: rgba(100, 116, 139, 0.1); "
                           "color: #64748b;"));
        return;
    }

    if (expected.isEmpty()) {
        labelComparisonResult->setText(tr("Enter expected hash above to compare automatically."));
        labelComparisonResult->setStyleSheet(
            QStringLiteral("padding: 8px; border-radius: 4px; font-weight: bold; background: rgba(100, 116, 139, 0.1); "
                           "color: #64748b;"));
        return;
    }

    if (HashIntegrityManager::verifyHashMatch(computed, expected)) {
        labelComparisonResult->setText(tr("✓ MATCH: Checksums match perfectly. Authenticity and integrity verified!"));
        labelComparisonResult->setStyleSheet(
            QStringLiteral("padding: 8px; border-radius: 4px; font-weight: bold; background: rgba(16, 185, 129, 0.2); "
                           "color: #10b981; border: 1px solid #10b981;"));
    } else {
        labelComparisonResult->setText(tr("✗ MISMATCH: Checksum mismatch! Possible corruption or tampering detected."));
        labelComparisonResult->setStyleSheet(
            QStringLiteral("padding: 8px; border-radius: 4px; font-weight: bold; background: rgba(239, 68, 68, 0.2); "
                           "color: #ef4444; border: 1px solid #ef4444;"));
    }
}
