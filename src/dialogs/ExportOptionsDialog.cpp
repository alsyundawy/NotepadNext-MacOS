/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "ExportOptionsDialog.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

ExportOptionsDialog::ExportOptionsDialog(TargetFormat format, const QString &defaultTitle, QWidget *parent)
    : QDialog(parent), m_format(format), m_initialTitle(defaultTitle) {
    setWindowTitle(format == TargetFormat::Pdf ? tr("Export to PDF Options") : tr("Export to HTML Options"));
    setMinimumWidth(380);
    setupUi();
}

void ExportOptionsDialog::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);

    auto *formLayout = new QFormLayout();
    editTitle = new QLineEdit(this);
    editTitle->setText(m_initialTitle);
    editTitle->setPlaceholderText(tr("Document title"));
    formLayout->addRow(tr("Document Title:"), editTitle);
    mainLayout->addLayout(formLayout);

    if (m_format == TargetFormat::Pdf) {
        auto *grpPage = new QGroupBox(tr("Page Layout"), this);
        auto *layoutPage = new QFormLayout(grpPage);

        comboPageSize = new QComboBox(grpPage);
        comboPageSize->addItem(tr("A4 (210 × 297 mm)"), static_cast<int>(QPageSize::A4));
        comboPageSize->addItem(tr("Letter (8.5 × 11 in)"), static_cast<int>(QPageSize::Letter));
        comboPageSize->addItem(tr("Legal (8.5 × 14 in)"), static_cast<int>(QPageSize::Legal));
        comboPageSize->addItem(tr("Tabloid (11 × 17 in)"), static_cast<int>(QPageSize::Tabloid));
        layoutPage->addRow(tr("Page Size:"), comboPageSize);

        comboOrientation = new QComboBox(grpPage);
        comboOrientation->addItem(tr("Portrait"), static_cast<int>(QPageLayout::Orientation::Portrait));
        comboOrientation->addItem(tr("Landscape"), static_cast<int>(QPageLayout::Orientation::Landscape));
        layoutPage->addRow(tr("Orientation:"), comboOrientation);

        comboMargins = new QComboBox(grpPage);
        comboMargins->addItem(tr("Normal (15 mm)"), 15);
        comboMargins->addItem(tr("Narrow (10 mm)"), 10);
        comboMargins->addItem(tr("Wide (25 mm)"), 25);
        layoutPage->addRow(tr("Margins:"), comboMargins);

        mainLayout->addWidget(grpPage);
    }

    auto *grpStyle = new QGroupBox(tr("Styling & Engine"), this);
    auto *layoutStyle = new QVBoxLayout(grpStyle);

    chkIncludeCss = new QCheckBox(tr("Embed GitHub Markdown CSS Stylesheet"), grpStyle);
    chkIncludeCss->setChecked(true);
    layoutStyle->addWidget(chkIncludeCss);

    chkDarkTheme = new QCheckBox(tr("Use Dark Mode Theme for Output"), grpStyle);
    chkDarkTheme->setChecked(false);
    layoutStyle->addWidget(chkDarkTheme);

    const bool pandocFound = DocumentExportManager::isPandocAvailable();
    chkUsePandoc = new QCheckBox(tr("Use Pandoc Engine (Advanced CLI Converter)"), grpStyle);
    chkUsePandoc->setChecked(false);
    chkUsePandoc->setEnabled(pandocFound);
    if (!pandocFound) {
        chkUsePandoc->setToolTip(tr("Pandoc executable was not detected on PATH"));
    }
    layoutStyle->addWidget(chkUsePandoc);

    mainLayout->addWidget(grpStyle);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export..."));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
}

DocumentExportManager::ExportOptions ExportOptionsDialog::options() const {
    DocumentExportManager::ExportOptions opt;
    opt.title = editTitle ? editTitle->text().trimmed() : QString();
    if (chkIncludeCss) {
        opt.includeCss = chkIncludeCss->isChecked();
    }
    if (chkDarkTheme) {
        opt.darkTheme = chkDarkTheme->isChecked();
    }
    if (chkUsePandoc) {
        opt.usePandocIfAvailable = chkUsePandoc->isChecked();
    }

    if (m_format == TargetFormat::Pdf) {
        if (comboPageSize) {
            opt.pageSize = static_cast<QPageSize::PageSizeId>(comboPageSize->currentData().toInt());
        }
        if (comboOrientation) {
            opt.orientation = static_cast<QPageLayout::Orientation>(comboOrientation->currentData().toInt());
        }
        if (comboMargins) {
            const double margin = comboMargins->currentData().toDouble();
            opt.margins = QMarginsF(margin, margin, margin, margin);
        }
    }

    return opt;
}
