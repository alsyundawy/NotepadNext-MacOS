/*
 * This file is part of NotepadNext.
 * Copyright 2026 Harry DS Alsyundawy (NotepadNext Maintainer)
 *
 * NotepadNext is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "ThemeSwitcherDialog.h"
#include <QtGlobal>

#ifndef QT_VERSION_CHECK
#define QT_VERSION_CHECK(major, minor, patch) (((major)<<16)|((minor)<<8)|(patch))
#endif

#include "dialogs/MainWindow.h"
#include "features/ThemeManager.h"
#include "features/ThemePaletteData.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QPainter>
#include <QKeySequence>

ThemeSwitcherDialog::ThemeSwitcherDialog(MainWindow *mainWindow, QWidget *parent)
    : QDialog(parent ? parent : mainWindow),
      m_mainWindow(mainWindow),
      m_initialPaletteId(ThemeManager::instance()->currentPaletteId()) {
    setupUi();
    populateTable();
}

ThemeSwitcherDialog::~ThemeSwitcherDialog() {}

void ThemeSwitcherDialog::setupUi() {
    setWindowTitle(tr("Theme Switcher — NotepadNext"));
    resize(720, 520);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(12);

    QLabel *titleLabel = new QLabel(tr("<h2>Select Visual Theme</h2><p style='opacity: 0.8;'>Choose from %1 "
                                       "professional themes across curated families with real-time preview.</p>")
                                        .arg(kTotalThemePaletteCount),
                                    this);
    mainLayout->addWidget(titleLabel);

    // Filter bar
    QHBoxLayout *filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(12);
    filterLayout->setContentsMargins(0, 2, 0, 2);
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(tr("Search theme by name or family (e.g. catppuccin, dracula, nord, light)..."));
    m_searchEdit->setClearButtonEnabled(true);
    filterLayout->addWidget(m_searchEdit, 1);

    m_filterGroup = new QButtonGroup(this);
    m_radioAll = new QRadioButton(tr("All (%1)").arg(kTotalThemePaletteCount), this);
    m_radioLight = new QRadioButton(tr("Light"), this);
    m_radioDark = new QRadioButton(tr("Dark"), this);
    m_radioAll->setChecked(true);

    m_filterGroup->addButton(m_radioAll, 0);
    m_filterGroup->addButton(m_radioLight, 1);
    m_filterGroup->addButton(m_radioDark, 2);

    filterLayout->addWidget(m_radioAll);
    filterLayout->addWidget(m_radioLight);
    filterLayout->addWidget(m_radioDark);
    mainLayout->addLayout(filterLayout);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({tr("Theme Name"), tr("Family"), tr("Type"), tr("Preview Swatches")});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    m_table->setColumnWidth(3, 180);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);
    mainLayout->addWidget(m_table);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QLabel *shortcutHint = new QLabel(
        tr("<small style='opacity: 0.8;'>Tip: Press Esc to revert or Ctrl+Shift+T to open again.</small>"), this);
    btnLayout->addWidget(shortcutHint);
    btnLayout->addStretch();

    m_cancelBtn = new QPushButton(tr("Cancel"), this);
    m_applyBtn = new QPushButton(tr("Apply Theme"), this);
    m_applyBtn->setDefault(true);
    btnLayout->addWidget(m_cancelBtn);
    btnLayout->addWidget(m_applyBtn);
    mainLayout->addLayout(btnLayout);

    // Connections
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ThemeSwitcherDialog::filterThemes);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_filterGroup, &QButtonGroup::idToggled, this, &ThemeSwitcherDialog::filterThemes);
#else
    connect(m_radioAll, &QRadioButton::toggled, this, &ThemeSwitcherDialog::filterThemes);
    connect(m_radioLight, &QRadioButton::toggled, this, &ThemeSwitcherDialog::filterThemes);
    connect(m_radioDark, &QRadioButton::toggled, this, &ThemeSwitcherDialog::filterThemes);
#endif
    connect(m_table, &QTableWidget::cellClicked, this, &ThemeSwitcherDialog::onThemeSelected);
    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int r, int c) {
        onThemeSelected(r, c);
        applyAndClose();
    });
    connect(m_applyBtn, &QPushButton::clicked, this, &ThemeSwitcherDialog::applyAndClose);
    connect(m_cancelBtn, &QPushButton::clicked, this, &ThemeSwitcherDialog::reject);
}

void ThemeSwitcherDialog::populateTable(const QString &filterText, int modeFilter) {
    m_table->setRowCount(0);
    const ThemePalette *curPal = ThemeManager::instance()->currentPalette();
    QString activeId = curPal ? QString::fromLatin1(curPal->id) : ThemeManager::instance()->currentPaletteId();
    int selectedRow = -1;

    for (size_t i = 0; i < kTotalThemePaletteCount; ++i) {
        const ThemePalette &p = kAllThemePalettes[i];

        // Mode filter: 1 = Light only, 2 = Dark only
        if (modeFilter == 1 && p.isDark)
            continue;
        if (modeFilter == 2 && !p.isDark)
            continue;

        // Text filter
        if (!filterText.isEmpty()) {
            QString query = filterText.trimmed();
            QString name = QString::fromUtf8(p.displayName);
            QString fam = QString::fromUtf8(p.family);
            if (!name.contains(query, Qt::CaseInsensitive) && !fam.contains(query, Qt::CaseInsensitive)) {
                continue;
            }
        }

        int row = m_table->rowCount();
        m_table->insertRow(row);

        QTableWidgetItem *item0 = new QTableWidgetItem(QString::fromUtf8(p.displayName));
        item0->setData(Qt::UserRole, QString::fromLatin1(p.id));
        m_table->setItem(row, 0, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem(QString::fromUtf8(p.family));
        m_table->setItem(row, 1, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem(p.isDark ? tr("Dark") : tr("Light"));
        m_table->setItem(row, 2, item2);

        // Render preview swatches (Win, Card, TxtPri, Accent, Cyan, Net)
        QPixmap pix(160, 22);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const char *colors[] = {p.win, p.card, p.border, p.txtPri, p.blue, p.cyan};
        for (int c = 0; c < 6; ++c) {
            painter.setBrush(QColor(colors[c]));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(c * 26 + 2, 2, 22, 18, 4, 4);
        }
        painter.end();

        QTableWidgetItem *item3 = new QTableWidgetItem();
        item3->setIcon(QIcon(pix));
        m_table->setItem(row, 3, item3);

        if (activeId.compare(QLatin1String(p.id), Qt::CaseInsensitive) == 0) {
            selectedRow = row;
        }
    }

    if (selectedRow >= 0) {
        m_table->selectRow(selectedRow);
        m_table->scrollToItem(m_table->item(selectedRow, 0));
    }
}

void ThemeSwitcherDialog::filterThemes() {
    int filterId = m_filterGroup->checkedId();
    populateTable(m_searchEdit->text(), filterId);
}

void ThemeSwitcherDialog::onThemeSelected(int row, int column) {
    Q_UNUSED(column)
    QTableWidgetItem *item = m_table->item(row, 0);
    if (!item)
        return;

    QString palId = item->data(Qt::UserRole).toString();
    ThemeManager::instance()->setPalette(palId, m_mainWindow);
}

void ThemeSwitcherDialog::applyAndClose() {
    int row = m_table->currentRow();
    if (row >= 0) {
        QTableWidgetItem *item = m_table->item(row, 0);
        if (item) {
            QString palId = item->data(Qt::UserRole).toString();
            ThemeManager::instance()->setPalette(palId, m_mainWindow);
        }
    }
    accept();
}

void ThemeSwitcherDialog::reject() {
    // Revert back to the initial theme
    ThemeManager::instance()->setPalette(m_initialPaletteId, m_mainWindow);
    QDialog::reject();
}
