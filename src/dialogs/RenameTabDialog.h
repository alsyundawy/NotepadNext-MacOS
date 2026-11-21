#pragma once
/*
 * This file is part of NotepadNext.
 *
 * Implements in-place tab file renaming dialog with disk verification,
 * name sanitization, and path conflict prevention.
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 */

#ifndef RENAMETABDIALOG_H
#define RENAMETABDIALOG_H

#include <QDialog>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class RenameTabDialog : public QDialog {
    Q_OBJECT

public:
    explicit RenameTabDialog(const QString &currentFilePath, QWidget *parent = nullptr);

    [[nodiscard]] QString newFilePath() const;
    [[nodiscard]] QString newFileName() const;

private slots:
    void validateInput();

private:
    void setupUi();

    QString m_oldFilePath;
    QFileInfo m_fileInfo;
    QString m_newFilePath;

    QLineEdit *m_nameEdit{nullptr};
    QLabel *m_statusLabel{nullptr};
    QLabel *m_locationLabel{nullptr};
    QPushButton *m_btnRename{nullptr};
    QPushButton *m_btnCancel{nullptr};
};

#endif // RENAMETABDIALOG_H
