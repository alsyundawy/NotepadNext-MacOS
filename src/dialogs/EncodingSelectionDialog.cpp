/*
 * This file is part of Notepad Next.
 *
 * Adapted from Notepad4 (zufuliu/notepad4) and Notepad3 (rizonesoft/Notepad3).
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "EncodingSelectionDialog.h"
#include "ui_EncodingSelectionDialog.h"

#include <QListWidgetItem>
#include <QMessageBox>

EncodingSelectionDialog::EncodingSelectionDialog(ScintillaNext *editor, QWidget *parent)
    : QDialog(parent), ui(new Ui::EncodingSelectionDialog), m_editor(editor) {
    ui->setupUi(this);

    m_allEncodings = EncodingDetector::supportedEncodings();
    populateList();
    updateCurrentDocInfo();

    connect(ui->editFilter, &QLineEdit::textChanged, this, &EncodingSelectionDialog::onFilterChanged);
    connect(ui->btnQuickUtf8, &QPushButton::clicked, this, &EncodingSelectionDialog::selectQuickUtf8);
    connect(ui->btnQuickUtf8Bom, &QPushButton::clicked, this, &EncodingSelectionDialog::selectQuickUtf8Bom);
    connect(ui->btnQuickAnsi, &QPushButton::clicked, this, &EncodingSelectionDialog::selectQuickAnsi);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this,
            &EncodingSelectionDialog::onApply);
}

EncodingSelectionDialog::~EncodingSelectionDialog() {
    delete ui;
}

void EncodingSelectionDialog::updateCurrentDocInfo() {
    if (!m_editor)
        return;

    ui->labelActiveEncoding->setText(m_editor->encodingName());

    switch (m_editor->bom()) {
        case ScintillaNext::BomType::Utf8:
            ui->labelBomStatus->setText(tr("Present (UTF-8 BOM, 3 bytes)"));
            break;
        case ScintillaNext::BomType::Utf16LE:
            ui->labelBomStatus->setText(tr("Present (UTF-16LE BOM, 2 bytes)"));
            break;
        case ScintillaNext::BomType::Utf16BE:
            ui->labelBomStatus->setText(tr("Present (UTF-16BE BOM, 2 bytes)"));
            break;
        case ScintillaNext::BomType::None:
        default:
            ui->labelBomStatus->setText(tr("None"));
            break;
    }

    // Run quick detection on sample to show detection method and confidence
    const QByteArray sample =
        QByteArray::fromRawData((char *)m_editor->characterPointer(), qMin(m_editor->textLength(), 4096));
    auto res = EncodingDetector::detect(sample);
    ui->labelDetectionMethod->setText(
        QStringLiteral("%1 (Confidence: %2%)").arg(res.detectionMethod).arg(res.confidence));
}

void EncodingSelectionDialog::populateList() {
    ui->listEncodings->clear();
    const QString currentEnc = m_editor ? m_editor->encodingName() : QStringLiteral("UTF-8");

    QString currentCategory;
    for (const auto &info : m_allEncodings) {
        if (info.category != currentCategory) {
            currentCategory = info.category;
            QListWidgetItem *catItem = new QListWidgetItem(QStringLiteral("── %1 ──").arg(currentCategory));
            catItem->setFlags(Qt::NoItemFlags);
            QFont font = catItem->font();
            font.setBold(true);
            catItem->setFont(font);
            ui->listEncodings->addItem(catItem);
        }

        QListWidgetItem *item = new QListWidgetItem(QStringLiteral("%1  (%2)").arg(info.description, info.name));
        item->setData(Qt::UserRole, info.name);
        ui->listEncodings->addItem(item);

        if (info.name.compare(currentEnc, Qt::CaseInsensitive) == 0) {
            ui->listEncodings->setCurrentItem(item);
        }
    }
}

void EncodingSelectionDialog::onFilterChanged(const QString &text) {
    const QString filter = text.trimmed().toLower();
    for (int i = 0; i < ui->listEncodings->count(); ++i) {
        QListWidgetItem *item = ui->listEncodings->item(i);
        if (!(item->flags() & Qt::ItemIsEnabled)) {
            // Category header
            continue;
        }
        const QString itemText = item->text().toLower();
        const QString codeName = item->data(Qt::UserRole).toString().toLower();
        const bool match = filter.isEmpty() || itemText.contains(filter) || codeName.contains(filter);
        item->setHidden(!match);
    }
}

QString EncodingSelectionDialog::selectedEncoding() const {
    QListWidgetItem *item = ui->listEncodings->currentItem();
    if (item && (item->flags() & Qt::ItemIsEnabled)) {
        return item->data(Qt::UserRole).toString();
    }
    return QStringLiteral("UTF-8");
}

ScintillaNext::BomType EncodingSelectionDialog::selectedBom() const {
    const QString enc = selectedEncoding().toUpper();
    if (enc == QStringLiteral("UTF-8")) {
        return (m_editor && m_editor->bom() == ScintillaNext::BomType::Utf8) ? ScintillaNext::BomType::Utf8
                                                                             : ScintillaNext::BomType::None;
    }
    if (enc == QStringLiteral("UTF-16LE"))
        return ScintillaNext::BomType::Utf16LE;
    if (enc == QStringLiteral("UTF-16BE"))
        return ScintillaNext::BomType::Utf16BE;
    return ScintillaNext::BomType::None;
}

bool EncodingSelectionDialog::isReopenMode() const {
    return ui->radioReopen->isChecked();
}

void EncodingSelectionDialog::selectQuickUtf8() {
    for (int i = 0; i < ui->listEncodings->count(); ++i) {
        QListWidgetItem *item = ui->listEncodings->item(i);
        if (item->data(Qt::UserRole).toString().compare(QLatin1String("UTF-8"), Qt::CaseInsensitive) == 0) {
            ui->listEncodings->setCurrentItem(item);
            break;
        }
    }
}

void EncodingSelectionDialog::selectQuickUtf8Bom() {
    selectQuickUtf8();
}

void EncodingSelectionDialog::selectQuickAnsi() {
    for (int i = 0; i < ui->listEncodings->count(); ++i) {
        QListWidgetItem *item = ui->listEncodings->item(i);
        if (item->data(Qt::UserRole).toString().compare(QLatin1String("windows-1252"), Qt::CaseInsensitive) == 0) {
            ui->listEncodings->setCurrentItem(item);
            break;
        }
    }
}

void EncodingSelectionDialog::onApply() {
    if (!m_editor)
        return;

    const QString enc = selectedEncoding();
    const bool reopen = isReopenMode();

    if (reopen) {
        if (!m_editor->isFile()) {
            QMessageBox::information(this, tr("Notepad Next"), tr("Cannot reopen unsaved new buffer from disk."));
            return;
        }
        m_editor->reloadWithEncoding(enc);
    } else {
        ScintillaNext::BomType bom = ScintillaNext::BomType::None;
        if (enc.compare("UTF-8", Qt::CaseInsensitive) == 0 && m_editor->bom() == ScintillaNext::BomType::Utf8) {
            bom = ScintillaNext::BomType::Utf8;
        } else if (enc.compare("UTF-16LE", Qt::CaseInsensitive) == 0) {
            bom = ScintillaNext::BomType::Utf16LE;
        } else if (enc.compare("UTF-16BE", Qt::CaseInsensitive) == 0) {
            bom = ScintillaNext::BomType::Utf16BE;
        }
        m_editor->convertToEncoding(enc, bom);
    }

    updateCurrentDocInfo();
    accept();
}
