#pragma once
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

#ifndef ENCODINGSELECTIONDIALOG_H
#define ENCODINGSELECTIONDIALOG_H

#include <QDialog>
#include "ScintillaNext.h"
#include "EncodingDetector.h"

namespace Ui {
class EncodingSelectionDialog;
}

class EncodingSelectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit EncodingSelectionDialog(ScintillaNext *editor, QWidget *parent = nullptr);
    ~EncodingSelectionDialog() override;

    QString selectedEncoding() const;
    ScintillaNext::BomType selectedBom() const;
    bool isReopenMode() const;

private slots:
    void onFilterChanged(const QString &text);
    void onApply();
    void selectQuickUtf8();
    void selectQuickUtf8Bom();
    void selectQuickAnsi();

private:
    void populateList();
    void updateCurrentDocInfo();

    Ui::EncodingSelectionDialog *ui;
    ScintillaNext *m_editor;
    QList<EncodingDetector::EncodingInfo> m_allEncodings;
};

#endif  // ENCODINGSELECTIONDIALOG_H
