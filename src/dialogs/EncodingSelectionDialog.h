/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUSTION | https://www.alsyundawy.com
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

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
    void onFilterChanged(const QString &text) const;
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
