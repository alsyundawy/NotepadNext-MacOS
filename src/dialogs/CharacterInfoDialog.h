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

#ifndef CHARACTERINFODIALOG_H
#define CHARACTERINFODIALOG_H

#include <QDialog>
#include "../features/CharacterInfo.h"

namespace Ui {
class CharacterInfoDialog;
}

class CharacterInfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit CharacterInfoDialog(const CharacterInfo::Details &details, QWidget *parent = nullptr);
    ~CharacterInfoDialog() override;

private slots:
    void copyCharacter();
    void copyCodePoint();
    void copyUtf8();
    void copyHtmlEntity();

private:
    Ui::CharacterInfoDialog *ui;
    CharacterInfo::Details m_details;
};

#endif  // CHARACTERINFODIALOG_H
