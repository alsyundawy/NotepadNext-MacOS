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

#ifndef CUSTOMIZETOOLBARDIALOG_H
#define CUSTOMIZETOOLBARDIALOG_H

#include <QDialog>
#include <QMap>
#include <QStringList>
#include <QWidget>

class QListWidget;
class QListWidgetItem;
class QLineEdit;
class QPushButton;
class QDialogButtonBox;
class QAction;
class MainWindow;

class CustomizeToolbarWidget : public QWidget {
    Q_OBJECT

public:
    explicit CustomizeToolbarWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    ~CustomizeToolbarWidget() override = default;

    static const QStringList &defaultToolbarActions();
    [[nodiscard]] QStringList currentActionNames() const;
    void saveToolbar();
    void refreshActions();

signals:
    void toolbarChanged();

public slots:
    void onApplyClicked();

private slots:
    void onAddClicked();
    void onRemoveClicked();
    void onAddSeparatorClicked();
    void onMoveUpClicked();
    void onMoveDownClicked();
    void onResetDefaultsClicked();
    void onFilterTextChanged(const QString &text) const;
    void onCurrentItemSelectionChanged();

private:
    void setupUi();
    void populateAvailableActions();
    void populateCurrentActions();
    void insertCurrentActionItem(int row, const QString &actionName);
    static QString cleanActionText(const QString &text);

    MainWindow *m_mainWindow = nullptr;
    QLineEdit *m_filterEdit = nullptr;
    QListWidget *m_availableList = nullptr;
    QListWidget *m_currentList = nullptr;

    QPushButton *m_btnAdd = nullptr;
    QPushButton *m_btnRemove = nullptr;
    QPushButton *m_btnAddSeparator = nullptr;
    QPushButton *m_btnMoveUp = nullptr;
    QPushButton *m_btnMoveDown = nullptr;
    QPushButton *m_btnReset = nullptr;
    QPushButton *m_btnApply = nullptr;

    QMap<QString, QAction *> m_availableActions;
};

class CustomizeToolbarDialog : public QDialog {
    Q_OBJECT

public:
    explicit CustomizeToolbarDialog(MainWindow *mainWindow, QWidget *parent = nullptr);
    ~CustomizeToolbarDialog() override = default;

    [[nodiscard]] CustomizeToolbarWidget *customizerWidget() const { return m_customizerWidget; }
    static const QStringList &defaultToolbarActions() { return CustomizeToolbarWidget::defaultToolbarActions(); }
    [[nodiscard]] QStringList currentActionNames() const {
        return m_customizerWidget ? m_customizerWidget->currentActionNames() : QStringList();
    }

private:
    CustomizeToolbarWidget *m_customizerWidget = nullptr;
    QDialogButtonBox *m_buttonBox = nullptr;
};

#endif  // CUSTOMIZETOOLBARDIALOG_H
