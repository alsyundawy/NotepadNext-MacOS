#pragma once
/*
 * This file is part of NotepadNext.
 * Copyright 2026 Harry DS Alsyundawy (NotepadNext Maintainer)
 *
 * NotepadNext is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef THEMESWITCHERDIALOG_H
#define THEMESWITCHERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>

class MainWindow;
struct ThemePalette;

class ThemeSwitcherDialog : public QDialog {
    Q_OBJECT

public:
    explicit ThemeSwitcherDialog(MainWindow *mainWindow, QWidget *parent = nullptr);
    ~ThemeSwitcherDialog() override;

private slots:
    void filterThemes();
    void onThemeSelected(int row, int column);
    void applyAndClose();
    void reject() override;

private:
    void setupUi();
    void populateTable(const QString &filterText = QString(), int modeFilter = 0);

    MainWindow *m_mainWindow = nullptr;
    QString m_initialPaletteId;
    QLineEdit *m_searchEdit = nullptr;
    QTableWidget *m_table = nullptr;
    QButtonGroup *m_filterGroup = nullptr;
    QRadioButton *m_radioAll = nullptr;
    QRadioButton *m_radioLight = nullptr;
    QRadioButton *m_radioDark = nullptr;
    QPushButton *m_applyBtn = nullptr;
    QPushButton *m_cancelBtn = nullptr;
};

#endif  // THEMESWITCHERDIALOG_H
