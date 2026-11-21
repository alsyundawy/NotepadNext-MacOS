#pragma once
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

#ifndef TAILOPTIONSDIALOG_H
#define TAILOPTIONSDIALOG_H

#include <QDialog>

#include <memory>

namespace Ui {
class TailOptionsDialog;
}

class ScintillaNext;

class TailOptionsDialog : public QDialog {
    Q_OBJECT

public:
    explicit TailOptionsDialog(QWidget *parent = nullptr, ScintillaNext *currentEditor = nullptr);
    ~TailOptionsDialog() override;

private Q_SLOTS:
    void onApplyClicked();

private:
    void loadSettings();

    std::unique_ptr<Ui::TailOptionsDialog> ui;
    ScintillaNext *m_editor{nullptr};
};

#endif  // TAILOPTIONSDIALOG_H
