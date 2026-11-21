/*
 * This file is part of NotepadNext.
 *
 * Implements Workspace Transfer & Portable Bundle Dialog (.npnbundle).
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 */

#include "TransferBundleDialog.h"
#include "../features/NotepadNextTransferManager.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

TransferBundleDialog::TransferBundleDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Workspace Transfer & Portable Bundle (.npnbundle)"));
    resize(560, 480);
    setupUi();
}

void TransferBundleDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    m_tabWidget = new QTabWidget(this);

    // ==========================================
    // TAB 1: EXPORT WORKSPACE BUNDLE
    // ==========================================
    auto *exportTab = new QWidget(this);
    auto *exportLayout = new QVBoxLayout(exportTab);

    auto *exportIntro = new QLabel(
        tr("<b>Export Workspace Configuration Bundle</b><br>"
           "Package your preferences, custom themes, sessions, and scripts into a portable, "
           "cross-platform <code>.npnbundle</code> archive for macOS, FreeBSD, and Linux."),
        exportTab);
    exportIntro->setWordWrap(true);
    exportLayout->addWidget(exportIntro);

    auto *itemsGroup = new QGroupBox(tr("Select Components to Include"), exportTab);
    auto *itemsLayout = new QVBoxLayout(itemsGroup);
    m_chkPreferences = new QCheckBox(tr("Preferences & App Settings (INI/JSON)"), itemsGroup);
    m_chkThemes = new QCheckBox(tr("Custom Visual Themes & Style Sheets (QSS)"), itemsGroup);
    m_chkSessions = new QCheckBox(tr("Workspace Sessions & Tab Layouts"), itemsGroup);
    m_chkScripts = new QCheckBox(tr("User Lua & Python Scripts"), itemsGroup);

    m_chkPreferences->setChecked(true);
    m_chkThemes->setChecked(true);
    m_chkSessions->setChecked(true);
    m_chkScripts->setChecked(true);

    itemsLayout->addWidget(m_chkPreferences);
    itemsLayout->addWidget(m_chkThemes);
    itemsLayout->addWidget(m_chkSessions);
    itemsLayout->addWidget(m_chkScripts);
    exportLayout->addWidget(itemsGroup);

    auto *securityGroup = new QGroupBox(tr("Optional Encryption (AES-256-CBC)"), exportTab);
    auto *secLayout = new QVBoxLayout(securityGroup);
    m_exportPassEdit = new QLineEdit(securityGroup);
    m_exportPassEdit->setEchoMode(QLineEdit::Password);
    m_exportPassEdit->setPlaceholderText(tr("Enter encryption passphrase (leave blank for unencrypted)..."));
    secLayout->addWidget(m_exportPassEdit);

    auto *strengthLayout = new QHBoxLayout();
    m_exportStrengthBar = new QProgressBar(securityGroup);
    m_exportStrengthBar->setRange(0, 100);
    m_exportStrengthBar->setValue(0);
    m_exportStrengthBar->setTextVisible(false);
    m_exportStrengthBar->setFixedHeight(6);

    m_exportStrengthLabel = new QLabel(tr("No passphrase (unencrypted)"), securityGroup);
    strengthLayout->addWidget(m_exportStrengthBar);
    strengthLayout->addWidget(m_exportStrengthLabel);
    secLayout->addLayout(strengthLayout);
    exportLayout->addWidget(securityGroup);

    connect(m_exportPassEdit, &QLineEdit::textChanged, this, &TransferBundleDialog::onPasswordChanged);

    auto *exportBtnLayout = new QHBoxLayout();
    exportBtnLayout->addStretch();
    m_btnExport = new QPushButton(tr("Export Bundle..."), exportTab);
    m_btnExport->setDefault(true);
    connect(m_btnExport, &QPushButton::clicked, this, &TransferBundleDialog::onExportClicked);
    exportBtnLayout->addWidget(m_btnExport);
    exportLayout->addLayout(exportBtnLayout);

    m_tabWidget->addTab(exportTab, tr("Export"));

    // ==========================================
    // TAB 2: IMPORT WORKSPACE BUNDLE
    // ==========================================
    auto *importTab = new QWidget(this);
    auto *importLayout = new QVBoxLayout(importTab);

    auto *importIntro = new QLabel(
        tr("<b>Import / Restore Workspace Bundle</b><br>"
           "Restore configuration files from an existing <code>.npnbundle</code> archive."),
        importTab);
    importIntro->setWordWrap(true);
    importLayout->addWidget(importIntro);

    auto *fileGroup = new QGroupBox(tr("Bundle Archive File"), importTab);
    auto *fileLayout = new QHBoxLayout(fileGroup);
    m_importPathEdit = new QLineEdit(fileGroup);
    m_importPathEdit->setPlaceholderText(tr("Select .npnbundle file..."));
    m_btnBrowseImport = new QPushButton(tr("Browse..."), fileGroup);
    connect(m_btnBrowseImport, &QPushButton::clicked, this, &TransferBundleDialog::onBrowseImportBundle);
    fileLayout->addWidget(m_importPathEdit);
    fileLayout->addWidget(m_btnBrowseImport);
    importLayout->addWidget(fileGroup);

    auto *importSecGroup = new QGroupBox(tr("Decryption Passphrase (if encrypted)"), importTab);
    auto *importSecLayout = new QHBoxLayout(importSecGroup);
    m_importPassEdit = new QLineEdit(importSecGroup);
    m_importPassEdit->setEchoMode(QLineEdit::Password);
    m_importPassEdit->setPlaceholderText(tr("Enter passphrase if bundle is encrypted..."));
    m_btnInspect = new QPushButton(tr("Inspect"), importSecGroup);
    connect(m_btnInspect, &QPushButton::clicked, this, &TransferBundleDialog::onInspectBundle);
    importSecLayout->addWidget(m_importPassEdit);
    importSecLayout->addWidget(m_btnInspect);
    importLayout->addWidget(importSecGroup);

    m_inspectList = new QListWidget(importTab);
    m_inspectList->setMaximumHeight(140);
    importLayout->addWidget(m_inspectList);

    m_inspectStatusLabel = new QLabel(importTab);
    m_inspectStatusLabel->setStyleSheet(QStringLiteral("font-size: 11px; color: #888888;"));
    importLayout->addWidget(m_inspectStatusLabel);

    auto *importBtnLayout = new QHBoxLayout();
    importBtnLayout->addStretch();
    m_btnImport = new QPushButton(tr("Restore Configuration"), importTab);
    connect(m_btnImport, &QPushButton::clicked, this, &TransferBundleDialog::onImportClicked);
    importBtnLayout->addWidget(m_btnImport);
    importLayout->addLayout(importBtnLayout);

    m_tabWidget->addTab(importTab, tr("Import"));

    mainLayout->addWidget(m_tabWidget);
}

void TransferBundleDialog::onPasswordChanged(const QString &pass)
{
    if (pass.isEmpty()) {
        m_exportStrengthBar->setValue(0);
        m_exportStrengthLabel->setText(tr("No passphrase (unencrypted)"));
    } else if (pass.length() < 6) {
        m_exportStrengthBar->setValue(30);
        m_exportStrengthLabel->setText(tr("<font color='#E53E3E'>Weak passphrase</font>"));
    } else if (pass.length() < 10) {
        m_exportStrengthBar->setValue(65);
        m_exportStrengthLabel->setText(tr("<font color='#E5A50A'>Moderate passphrase</font>"));
    } else {
        m_exportStrengthBar->setValue(100);
        m_exportStrengthLabel->setText(tr("<font color='#22C55E'>Strong passphrase</font>"));
    }
}

void TransferBundleDialog::onExportClicked()
{
    const QString target = QFileDialog::getSaveFileName(
        this, tr("Save Workspace Bundle"),
        QDir::homePath() + QStringLiteral("/NotepadNext_Backup.npnbundle"),
        tr("NotepadNext Bundle (*.npnbundle)"));

    if (target.isEmpty()) return;

    NotepadNextTransferManager::ExportOptions opts;
    opts.includePreferences = m_chkPreferences->isChecked();
    opts.includeThemes = m_chkThemes->isChecked();
    opts.includeSessions = m_chkSessions->isChecked();
    opts.includeScripts = m_chkScripts->isChecked();
    opts.passphrase = m_exportPassEdit->text();

    const auto res = NotepadNextTransferManager::exportBundle(target, opts);
    if (res.success) {
        QMessageBox::information(this, tr("Export Successful"),
            tr("Workspace bundle exported successfully!\n\n"
               "Files archived: %1\nTotal size: %2 KB\nSHA-256: %3\n\nDestination: %4")
                .arg(res.filesCount)
                .arg(res.totalBytes / 1024)
                .arg(res.bundleSha256.left(16) + QStringLiteral("..."))
                .arg(target));
    } else {
        QMessageBox::critical(this, tr("Export Failed"), res.message);
    }
}

void TransferBundleDialog::onBrowseImportBundle()
{
    const QString bundle = QFileDialog::getOpenFileName(
        this, tr("Select Workspace Bundle"), QDir::homePath(),
        tr("NotepadNext Bundle (*.npnbundle);;All Files (*.*)"));
    if (!bundle.isEmpty()) {
        m_importPathEdit->setText(bundle);
        onInspectBundle();
    }
}

void TransferBundleDialog::onInspectBundle()
{
    const QString bundle = m_importPathEdit->text().trimmed();
    if (bundle.isEmpty() || !QFile::exists(bundle)) {
        m_inspectStatusLabel->setText(tr("Please select an existing bundle file."));
        return;
    }

    QStringList files;
    const auto res = NotepadNextTransferManager::inspectBundle(bundle, m_importPassEdit->text(), &files);
    m_inspectList->clear();
    if (res.success) {
        for (const auto &f : files) {
            m_inspectList->addItem(f);
        }
        m_inspectStatusLabel->setText(tr("<font color='#22C55E'>✓ %1 items found in bundle.</font>").arg(files.size()));
    } else {
        m_inspectStatusLabel->setText(tr("<font color='#E53E3E'>Inspection failed: %1</font>").arg(res.message));
    }
}

void TransferBundleDialog::onImportClicked()
{
    const QString bundle = m_importPathEdit->text().trimmed();
    if (bundle.isEmpty() || !QFile::exists(bundle)) {
        QMessageBox::warning(this, tr("Bundle Not Found"), tr("Please select a valid bundle file first."));
        return;
    }

    const auto reply = QMessageBox::question(
        this, tr("Confirm Restore"),
        tr("Restoring a bundle will update matching configuration files in your settings folder.\n\n"
           "Do you wish to proceed?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    if (reply != QMessageBox::Yes) return;

    const auto res = NotepadNextTransferManager::importBundle(bundle, m_importPassEdit->text());
    if (res.success) {
        QMessageBox::information(this, tr("Restore Completed"),
            tr("Workspace bundle successfully restored!\n\n"
               "Files restored: %1\n\n"
               "Please restart NotepadNext to apply all restored configurations.")
                .arg(res.filesCount));
        accept();
    } else {
        QMessageBox::critical(this, tr("Restore Failed"), res.message);
    }
}
