#pragma once
/*
 * This file is part of NotepadNext.
 *
 * Implements Workspace Transfer & Portable Bundle Dialog (.npnbundle).
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 */

#ifndef TRANSFERBUNDLEDIALOG_H
#define TRANSFERBUNDLEDIALOG_H

#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QTabWidget>

class TransferBundleDialog : public QDialog {
    Q_OBJECT

public:
    explicit TransferBundleDialog(QWidget *parent = nullptr);

private slots:
    void onExportClicked();
    void onBrowseImportBundle();
    void onInspectBundle();
    void onImportClicked();
    void onPasswordChanged(const QString &pass);

private:
    void setupUi();

    QTabWidget *m_tabWidget{nullptr};

    // Export Tab
    QCheckBox *m_chkPreferences{nullptr};
    QCheckBox *m_chkThemes{nullptr};
    QCheckBox *m_chkSessions{nullptr};
    QCheckBox *m_chkScripts{nullptr};
    QLineEdit *m_exportPassEdit{nullptr};
    QLabel *m_exportStrengthLabel{nullptr};
    QProgressBar *m_exportStrengthBar{nullptr};
    QPushButton *m_btnExport{nullptr};

    // Import Tab
    QLineEdit *m_importPathEdit{nullptr};
    QPushButton *m_btnBrowseImport{nullptr};
    QLineEdit *m_importPassEdit{nullptr};
    QPushButton *m_btnInspect{nullptr};
    QListWidget *m_inspectList{nullptr};
    QLabel *m_inspectStatusLabel{nullptr};
    QPushButton *m_btnImport{nullptr};
};

#endif // TRANSFERBUNDLEDIALOG_H
