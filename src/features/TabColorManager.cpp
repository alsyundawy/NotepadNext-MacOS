/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "TabColorManager.h"
#include <QSettings>
#include <QtGlobal>
#include <algorithm>

TabColorManager::TabColorManager(QObject *parent) : QObject(parent) {
    loadSettings();
}

TabColorManager *TabColorManager::instance() {
    static TabColorManager s_instance;
    return &s_instance;
}

QList<TabColorPreset> TabColorManager::presets() {
    return {
        {tr("Red"), QColor(QStringLiteral("#cf222e"))},
        {tr("Orange"), QColor(QStringLiteral("#bc4c00"))},
        {tr("Yellow"), QColor(QStringLiteral("#bf8700"))},
        {tr("Green"), QColor(QStringLiteral("#1a7f37"))},
        {tr("Teal"), QColor(QStringLiteral("#1b7c83"))},
        {tr("Cyan"), QColor(QStringLiteral("#0969da"))},
        {tr("Blue"), QColor(QStringLiteral("#0550ae"))},
        {tr("Purple"), QColor(QStringLiteral("#8250df"))},
        {tr("Magenta"), QColor(QStringLiteral("#bf3989"))},
        {tr("Coral"), QColor(QStringLiteral("#d0395b"))}
    };
}

QList<QColor> TabColorManager::paletteColors() {
    const auto p = presets();
    QList<QColor> list;
    list.reserve(p.size());
    std::transform(p.cbegin(), p.cend(), std::back_inserter(list),
                   [](const TabColorPreset &item) { return item.color; });
    return list;
}

void TabColorManager::setTabColor(const QString &filePath, const QColor &color) {
    if (filePath.isEmpty()) {
        return;
    }

    if (color.isValid()) {
        m_tabColors.insert(filePath, color);
    } else {
        m_tabColors.remove(filePath);
    }

    saveSettings();
    emit tabColorChanged(filePath, color);
}

QColor TabColorManager::tabColor(const QString &filePath) const {
    return m_tabColors.value(filePath, QColor());
}

void TabColorManager::clearTabColor(const QString &filePath) {
    setTabColor(filePath, QColor());
}

bool TabColorManager::hasTabColor(const QString &filePath) const {
    return m_tabColors.contains(filePath) && m_tabColors.value(filePath).isValid();
}

const QMap<QString, QColor> &TabColorManager::allTabColors() const {
    return m_tabColors;
}

void TabColorManager::loadSettings() {
    m_tabColors.clear();
    QSettings settings;
    settings.beginGroup(QStringLiteral("TabColors"));
    const int size = settings.beginReadArray(QStringLiteral("Colors"));
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        const QString path = settings.value(QStringLiteral("path")).toString();
        const QString colorHex = settings.value(QStringLiteral("color")).toString();
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
        const QColor c = QColor::fromString(colorHex);
#else
        const QColor c(colorHex);
#endif
        if (!path.isEmpty() && c.isValid()) {
            m_tabColors.insert(path, c);
        }
    }
    settings.endArray();
    settings.endGroup();
}

void TabColorManager::saveSettings() const {
    QSettings settings;
    settings.beginGroup(QStringLiteral("TabColors"));
    settings.remove(QString());
    int index = 0;
    settings.beginWriteArray(QStringLiteral("Colors"));
    for (auto it = m_tabColors.cbegin(); it != m_tabColors.cend(); ++it) {
        if (it.value().isValid()) {
            settings.setArrayIndex(index++);
            settings.setValue(QStringLiteral("path"), it.key());
            settings.setValue(QStringLiteral("color"), it.value().name(QColor::HexArgb));
        }
    }
    settings.endArray();
    settings.endGroup();
}
