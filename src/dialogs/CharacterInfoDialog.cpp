/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "CharacterInfoDialog.h"
#include "ui_CharacterInfoDialog.h"

#include <QClipboard>
#include <QApplication>

CharacterInfoDialog::CharacterInfoDialog(const CharacterInfo::Details &d, QWidget *parent)
    : QDialog(parent), ui(new Ui::CharacterInfoDialog), m_details(d) {
    ui->setupUi(this);

    ui->labelGlyph->setText(d.displayGlyph);
    ui->labelCharName->setText(d.name);
    ui->labelCodePoint->setText(QStringLiteral("%1  (Dec: %2,  Oct: 0%3)")
                                    .arg(CharacterInfo::formatCodePoint(d.codePoint))
                                    .arg(d.codePoint)
                                    .arg(QString::number(d.codePoint, 8)));
    ui->labelBlock->setText(QStringLiteral("Block: %1").arg(d.block));

    ui->labelCategory->setText(QStringLiteral("%1 (%2)").arg(d.categoryDescription, d.categoryCode));
    ui->labelScript->setText(d.script);
    ui->labelDirection->setText(d.direction);

    ui->labelUtf8->setText(QStringLiteral("%1  (Dec: %2)").arg(d.utf8Hex, d.utf8Dec));
    ui->labelUtf16->setText(QStringLiteral("%1  (Dec: %2)").arg(d.utf16Hex, d.utf16Dec));
    ui->labelUtf32->setText(d.utf32Hex);

    ui->labelHtmlNamed->setText(d.htmlNamedEntity.isEmpty() ? QStringLiteral("-") : d.htmlNamedEntity);
    ui->labelHtmlDec->setText(d.htmlDecEntity);
    ui->labelHtmlHex->setText(d.htmlHexEntity);
    ui->labelCEscape->setText(d.cEscape);
    ui->labelJsonEscape->setText(d.jsonEscape);
    ui->labelCssEscape->setText(d.cssEscape);
    ui->labelUrlPercent->setText(d.urlPercent);

    connect(ui->btnCopyChar, &QPushButton::clicked, this, &CharacterInfoDialog::copyCharacter);
    connect(ui->btnCopyCodePoint, &QPushButton::clicked, this, &CharacterInfoDialog::copyCodePoint);
    connect(ui->btnCopyUtf8, &QPushButton::clicked, this, &CharacterInfoDialog::copyUtf8);
    connect(ui->btnCopyHtml, &QPushButton::clicked, this, &CharacterInfoDialog::copyHtmlEntity);
}

CharacterInfoDialog::~CharacterInfoDialog() {
    delete ui;
}

void CharacterInfoDialog::copyCharacter() {
    QApplication::clipboard()->setText(m_details.glyph);
}

void CharacterInfoDialog::copyCodePoint() {
    QApplication::clipboard()->setText(CharacterInfo::formatCodePoint(m_details.codePoint));
}

void CharacterInfoDialog::copyUtf8() {
    QApplication::clipboard()->setText(m_details.utf8Hex);
}

void CharacterInfoDialog::copyHtmlEntity() {
    if (!m_details.htmlNamedEntity.isEmpty()) {
        QApplication::clipboard()->setText(m_details.htmlNamedEntity);
    } else {
        QApplication::clipboard()->setText(m_details.htmlDecEntity);
    }
}
