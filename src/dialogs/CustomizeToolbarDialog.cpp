/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "CustomizeToolbarDialog.h"
#include "MainWindow.h"
#include "../features/ToolbarIconManager.h"

#include <algorithm>
#include <QAction>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

const QStringList &CustomizeToolbarWidget::defaultToolbarActions() {
    static const QStringList list = {
        QStringLiteral("New"),
        QStringLiteral("Open"),
        QStringLiteral("Save"),
        QStringLiteral("SaveAll"),
        QStringLiteral("Close"),
        QStringLiteral("CloseAll"),
        QStringLiteral("Print"),
        QStringLiteral(""),
        QStringLiteral("Cut"),
        QStringLiteral("Copy"),
        QStringLiteral("Paste"),
        QStringLiteral(""),
        QStringLiteral("Undo"),
        QStringLiteral("Redo"),
        QStringLiteral(""),
        QStringLiteral("Find"),
        QStringLiteral("Replace"),
        QStringLiteral(""),
        QStringLiteral("ZoomIn"),
        QStringLiteral("ZoomOut"),
        QStringLiteral(""),
        QStringLiteral("WordWrap"),
        QStringLiteral("ShowAllCharacters"),
        QStringLiteral("ShowIndentGuide"),
        QStringLiteral(""),
        QStringLiteral("MacroRecording"),
        QStringLiteral("Playback"),
        QStringLiteral("RunMacroMultipleTimes"),
        QStringLiteral("SaveCurrentRecordedMacro"),
        QStringLiteral(""),
        QStringLiteral("Restart"),
        QStringLiteral("Preferences"),
        QStringLiteral("Exit"),
    };
    return list;
}

QString CustomizeToolbarWidget::cleanActionText(const QString &text) {
    QString res = text;
    res.remove(QLatin1Char('&'));
    res.remove(QStringLiteral("..."));
    return res.trimmed();
}

CustomizeToolbarWidget::CustomizeToolbarWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent), m_mainWindow(mainWindow) {
    setupUi();
    populateAvailableActions();
    populateCurrentActions();
}

void CustomizeToolbarWidget::setupUi() {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    auto *headerLabel = new QLabel(tr("Select actions to display on the main toolbar. You can add, remove, and reorder "
                                      "items or insert separators."),
                                   this);
    headerLabel->setWordWrap(true);
    mainLayout->addWidget(headerLabel);

    auto *centerLayout = new QHBoxLayout(); // NOSONAR(cpp:S5025)
    centerLayout->setSpacing(12);

    // Left column: Available Actions
    auto *leftCol = new QVBoxLayout(); // NOSONAR(cpp:S5025)
    auto *lblAvailable = new QLabel(tr("Available Actions:"), this);
    lblAvailable->setStyleSheet(QStringLiteral("font-weight: bold;"));
    leftCol->addWidget(lblAvailable);

    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText(tr("Search actions..."));
    m_filterEdit->setClearButtonEnabled(true);
    connect(m_filterEdit, &QLineEdit::textChanged, this, &CustomizeToolbarWidget::onFilterTextChanged);
    leftCol->addWidget(m_filterEdit);

    m_availableList = new QListWidget(this);
    m_availableList->setIconSize(QSize(24, 24));
    m_availableList->setAlternatingRowColors(true);
    connect(m_availableList, &QListWidget::itemDoubleClicked, this, [this](const QListWidgetItem *item) {
        if (item) {
            onAddClicked();
        }
    });
    leftCol->addWidget(m_availableList, 1);
    centerLayout->addLayout(leftCol, 1);

    // Middle column: Action Buttons
    auto *midCol = new QVBoxLayout(); // NOSONAR(cpp:S5025)
    midCol->addStretch();

    m_btnAdd = new QPushButton(tr("Add ▶"), this);
    m_btnAdd->setToolTip(tr("Add selected action to toolbar"));
    connect(m_btnAdd, &QPushButton::clicked, this, &CustomizeToolbarWidget::onAddClicked);
    midCol->addWidget(m_btnAdd);

    m_btnRemove = new QPushButton(tr("◀ Remove"), this);
    m_btnRemove->setToolTip(tr("Remove selected item from toolbar"));
    connect(m_btnRemove, &QPushButton::clicked, this, &CustomizeToolbarWidget::onRemoveClicked);
    midCol->addWidget(m_btnRemove);

    midCol->addSpacing(16);

    m_btnAddSeparator = new QPushButton(tr("+ Separator"), this);
    m_btnAddSeparator->setToolTip(tr("Insert a separator line at selected position"));
    connect(m_btnAddSeparator, &QPushButton::clicked, this, &CustomizeToolbarWidget::onAddSeparatorClicked);
    midCol->addWidget(m_btnAddSeparator);

    midCol->addSpacing(16);

    m_btnMoveUp = new QPushButton(tr("▲ Move Up"), this);
    m_btnMoveUp->setToolTip(tr("Move selected item up"));
    connect(m_btnMoveUp, &QPushButton::clicked, this, &CustomizeToolbarWidget::onMoveUpClicked);
    midCol->addWidget(m_btnMoveUp);

    m_btnMoveDown = new QPushButton(tr("▼ Move Down"), this);
    m_btnMoveDown->setToolTip(tr("Move selected item down"));
    connect(m_btnMoveDown, &QPushButton::clicked, this, &CustomizeToolbarWidget::onMoveDownClicked);
    midCol->addWidget(m_btnMoveDown);

    midCol->addStretch();
    centerLayout->addLayout(midCol);

    // Right column: Current Toolbar Buttons
    auto *rightCol = new QVBoxLayout(); // NOSONAR(cpp:S5025)
    auto *lblCurrent = new QLabel(tr("Current Toolbar Buttons:"), this);
    lblCurrent->setStyleSheet(QStringLiteral("font-weight: bold;"));
    rightCol->addWidget(lblCurrent);

    m_currentList = new QListWidget(this);
    m_currentList->setIconSize(QSize(24, 24));
    m_currentList->setAlternatingRowColors(true);
    m_currentList->setDragDropMode(QAbstractItemView::InternalMove);
    m_currentList->setDefaultDropAction(Qt::MoveAction);
    connect(m_currentList, &QListWidget::itemSelectionChanged, this,
            &CustomizeToolbarWidget::onCurrentItemSelectionChanged);
    connect(m_currentList, &QListWidget::itemDoubleClicked, this, [this](const QListWidgetItem *item) {
        if (item) {
            onRemoveClicked();
        }
    });
    rightCol->addWidget(m_currentList, 1);
    centerLayout->addLayout(rightCol, 1);

    mainLayout->addLayout(centerLayout, 1);

    // Bottom controls
    auto *bottomLayout = new QHBoxLayout(); // NOSONAR(cpp:S5025)
    m_btnReset = new QPushButton(tr("Reset to Defaults"), this);
    m_btnReset->setToolTip(tr("Restore standard NotepadNext toolbar buttons"));
    connect(m_btnReset, &QPushButton::clicked, this, &CustomizeToolbarWidget::onResetDefaultsClicked);
    bottomLayout->addWidget(m_btnReset);

    bottomLayout->addStretch();

    m_btnApply = new QPushButton(tr("Apply"), this);
    m_btnApply->setToolTip(tr("Apply toolbar changes immediately"));
    connect(m_btnApply, &QPushButton::clicked, this, &CustomizeToolbarWidget::onApplyClicked);
    bottomLayout->addWidget(m_btnApply);

    mainLayout->addLayout(bottomLayout);
    onCurrentItemSelectionChanged();
}

void CustomizeToolbarWidget::populateAvailableActions() {
    m_availableList->clear();
    m_availableActions.clear();

    if (!m_mainWindow) {
        return;
    }

    const auto allActions = m_mainWindow->findChildren<QAction *>();
    for (QAction *action : allActions) {
        if (!action || action->isSeparator()) {
            continue;
        }

        const QString objName = action->objectName();
        if (!objName.startsWith(QLatin1String("action")) || objName.length() <= 6) {
            continue;
        }

        if (const QString cleanText = cleanActionText(action->text()); cleanText.isEmpty()) {
            continue;
        }

        const QString actionName = objName.mid(6);
        if (!m_availableActions.contains(actionName)) {
            m_availableActions.insert(actionName, action);
        }
    }

    QStringList sortedActionNames = m_availableActions.keys();
    std::ranges::sort(sortedActionNames, [this](const QString &a, const QString &b) {
        return cleanActionText(m_availableActions[a]->text())
                   .localeAwareCompare(cleanActionText(m_availableActions[b]->text())) < 0;
    });

    for (const QString &actionName : sortedActionNames) {
        const QAction *action = m_availableActions.value(actionName);
        QIcon icon = ToolbarIconManager::instance()->actionIcon(actionName);
        if (icon.isNull()) {
            icon = action->icon();
        }

        auto *item = new QListWidgetItem(icon, cleanActionText(action->text()), m_availableList);
        item->setData(Qt::UserRole, actionName);
        if (!action->toolTip().isEmpty()) {
            item->setToolTip(action->toolTip());
        }
    }
}

void CustomizeToolbarWidget::populateCurrentActions() {
    m_currentList->clear();

    if (!m_mainWindow) {
        return;
    }

    const QStringList actionNames = m_mainWindow->currentToolbarActionNames();
    for (const QString &actionName : actionNames) {
        insertCurrentActionItem(m_currentList->count(), actionName);
    }
}

void CustomizeToolbarWidget::insertCurrentActionItem(int row, const QString &actionName) {
    if (actionName.isEmpty()) {
        auto *sepItem = new QListWidgetItem(); // NOSONAR(cpp:S5025)
        sepItem->setText(tr("─────────── [ Separator ] ───────────"));
        sepItem->setData(Qt::UserRole, QString());
        sepItem->setTextAlignment(Qt::AlignCenter);
        sepItem->setForeground(QBrush(QColor(130, 130, 130)));
        QFont font = sepItem->font();
        font.setItalic(true);
        sepItem->setFont(font);
        m_currentList->insertItem(row, sepItem);
        return;
    }

    const QAction *action = nullptr;
    if (m_availableActions.contains(actionName)) {
        action = m_availableActions.value(actionName);
    } else if (m_mainWindow) {
        action = m_mainWindow->findChild<QAction *>(QStringLiteral("action") + actionName, Qt::FindDirectChildrenOnly);
    }

    QString labelText = actionName;
    QIcon icon = ToolbarIconManager::instance()->actionIcon(actionName);
    QString toolTip;

    if (action) {
        labelText = cleanActionText(action->text());
        if (icon.isNull()) {
            icon = action->icon();
        }
        toolTip = action->toolTip();
    }

    auto *item = new QListWidgetItem(); // NOSONAR(cpp:S5025)
    item->setText(labelText);
    item->setIcon(icon);
    item->setData(Qt::UserRole, actionName);
    if (!toolTip.isEmpty()) {
        item->setToolTip(toolTip);
    }

    m_currentList->insertItem(row, item);
}

QStringList CustomizeToolbarWidget::currentActionNames() const {
    QStringList result;
    const int count = m_currentList->count();
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
        const QListWidgetItem *item = m_currentList->item(i);
        result.append(item->data(Qt::UserRole).toString());
    }
    return result;
}

void CustomizeToolbarWidget::onAddClicked() {
    const QListWidgetItem *availItem = m_availableList->currentItem();
    if (!availItem) {
        return;
    }

    const QString actionName = availItem->data(Qt::UserRole).toString();
    int insertIndex = m_currentList->currentRow();
    if (insertIndex < 0) {
        insertIndex = m_currentList->count();
    } else {
        insertIndex += 1;
    }

    insertCurrentActionItem(insertIndex, actionName);
    m_currentList->setCurrentRow(insertIndex);
}

void CustomizeToolbarWidget::onRemoveClicked() {
    const int row = m_currentList->currentRow();
    if (row < 0) {
        return;
    }

    const std::unique_ptr<QListWidgetItem> removedItem(m_currentList->takeItem(row));

    if (row < m_currentList->count()) {
        m_currentList->setCurrentRow(row);
    } else if (row - 1 >= 0) {
        m_currentList->setCurrentRow(row - 1);
    }
}

void CustomizeToolbarWidget::onAddSeparatorClicked() {
    int insertIndex = m_currentList->currentRow();
    if (insertIndex < 0) {
        insertIndex = m_currentList->count();
    } else {
        insertIndex += 1;
    }

    insertCurrentActionItem(insertIndex, QString());
    m_currentList->setCurrentRow(insertIndex);
}

void CustomizeToolbarWidget::onMoveUpClicked() {
    const int row = m_currentList->currentRow();
    if (row <= 0) {
        return;
    }

    QListWidgetItem *item = m_currentList->takeItem(row);
    m_currentList->insertItem(row - 1, item);
    m_currentList->setCurrentRow(row - 1);
}

void CustomizeToolbarWidget::onMoveDownClicked() {
    const int row = m_currentList->currentRow();
    if (row < 0 || row >= m_currentList->count() - 1) {
        return;
    }

    QListWidgetItem *item = m_currentList->takeItem(row);
    m_currentList->insertItem(row + 1, item);
    m_currentList->setCurrentRow(row + 1);
}

void CustomizeToolbarWidget::onResetDefaultsClicked() {
    m_currentList->clear();
    const QStringList &defaults = defaultToolbarActions();
    for (const QString &actionName : defaults) {
        insertCurrentActionItem(m_currentList->count(), actionName);
    }
}

void CustomizeToolbarWidget::onFilterTextChanged(const QString &text) const {
    const QString trimmed = text.trimmed();
    const int count = m_availableList->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem *item = m_availableList->item(i);
        if (trimmed.isEmpty()) {
            item->setHidden(false);
        } else {
            const bool matches = item->text().contains(trimmed, Qt::CaseInsensitive) ||
                                 item->data(Qt::UserRole).toString().contains(trimmed, Qt::CaseInsensitive);
            item->setHidden(!matches);
        }
    }
}

void CustomizeToolbarWidget::onCurrentItemSelectionChanged() {
    const int row = m_currentList->currentRow();
    const int count = m_currentList->count();
    const bool hasSelection = (row >= 0);

    m_btnRemove->setEnabled(hasSelection);
    m_btnMoveUp->setEnabled(hasSelection && row > 0);
    m_btnMoveDown->setEnabled(hasSelection && row < count - 1);
}

void CustomizeToolbarWidget::onApplyClicked() {
    if (m_mainWindow) {
        m_mainWindow->updateToolbarActions(currentActionNames());
        emit toolbarChanged();
    }
}

void CustomizeToolbarWidget::saveToolbar() {
    onApplyClicked();
}

void CustomizeToolbarWidget::refreshActions() {
    populateAvailableActions();
    populateCurrentActions();
}

// -----------------------------------------------------------------------------
// CustomizeToolbarDialog
// -----------------------------------------------------------------------------

CustomizeToolbarDialog::CustomizeToolbarDialog(MainWindow *mainWindow, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(tr("Customize Toolbar Buttons"));
    setMinimumSize(740, 520);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    m_customizerWidget = new CustomizeToolbarWidget(mainWindow, this);
    mainLayout->addWidget(m_customizerWidget, 1);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this] {
        if (m_customizerWidget) {
            m_customizerWidget->saveToolbar();
        }
        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);
}
