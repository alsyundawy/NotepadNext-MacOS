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

#include "ClosedTabsManager.h"
#include <QtGlobal>

QJsonObject ClosedTabInfo::toJson() const {
    QJsonObject obj;
    obj[QStringLiteral("filePath")] = filePath;
    obj[QStringLiteral("cursorPosition")] = cursorPosition;
    obj[QStringLiteral("firstVisibleLine")] = firstVisibleLine;
    if (tabColor.isValid()) {
        obj[QStringLiteral("tabColor")] = tabColor.name(QColor::HexArgb);
    }
    obj[QStringLiteral("encoding")] = encoding;
    obj[QStringLiteral("eolMode")] = eolMode;
    obj[QStringLiteral("timestamp")] = timestamp > 0 ? timestamp : QDateTime::currentSecsSinceEpoch();
    obj[QStringLiteral("isUntitled")] = isUntitled;
    if (isUntitled) {
        obj[QStringLiteral("untitledContent")] = untitledContent;
        obj[QStringLiteral("untitledName")] = untitledName;
    }
    return obj;
}

ClosedTabInfo ClosedTabInfo::fromJson(const QJsonObject &obj) {
    ClosedTabInfo info;
    info.filePath = obj[QStringLiteral("filePath")].toString();
    info.cursorPosition = obj[QStringLiteral("cursorPosition")].toInteger();
    info.firstVisibleLine = obj[QStringLiteral("firstVisibleLine")].toInteger();
    if (obj.contains(QStringLiteral("tabColor"))) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
        info.tabColor = QColor::fromString(obj[QStringLiteral("tabColor")].toString());
#else
        info.tabColor = QColor(obj[QStringLiteral("tabColor")].toString());
#endif
    }
    info.encoding = obj[QStringLiteral("encoding")].toString();
    info.eolMode = obj[QStringLiteral("eolMode")].toInt();
    info.timestamp = obj[QStringLiteral("timestamp")].toInteger();
    info.isUntitled = obj[QStringLiteral("isUntitled")].toBool();
    if (info.isUntitled) {
        info.untitledContent = obj[QStringLiteral("untitledContent")].toString();
        info.untitledName = obj[QStringLiteral("untitledName")].toString();
    }
    return info;
}

ClosedTabsManager::ClosedTabsManager(QObject *parent) : QObject(parent) {}

ClosedTabsManager *ClosedTabsManager::instance() {
    static ClosedTabsManager s_instance;
    return &s_instance;
}

void ClosedTabsManager::pushClosedTab(const ClosedTabInfo &info) {
    if (info.filePath.isEmpty() && (!info.isUntitled || info.untitledContent.isEmpty())) {
        return;
    }

    m_stack.append(info);
    while (m_stack.size() > kMaxClosedTabs) {
        m_stack.removeFirst();
    }
    emit stackChanged();
}

std::optional<ClosedTabInfo> ClosedTabsManager::popClosedTab() {
    if (m_stack.isEmpty()) {
        return std::nullopt;
    }
    ClosedTabInfo info = m_stack.takeLast();
    emit stackChanged();
    return info;
}

bool ClosedTabsManager::canReopen() const {
    return !m_stack.isEmpty();
}

int ClosedTabsManager::count() const {
    return static_cast<int>(m_stack.size());
}

void ClosedTabsManager::clear() {
    if (!m_stack.isEmpty()) {
        m_stack.clear();
        emit stackChanged();
    }
}

const QList<ClosedTabInfo> &ClosedTabsManager::allClosedTabs() const {
    return m_stack;
}

QJsonArray ClosedTabsManager::toJson() const {
    QJsonArray arr;
    for (const auto &item : m_stack) {
        arr.append(item.toJson());
    }
    return arr;
}

void ClosedTabsManager::fromJson(const QJsonArray &arr) {
    m_stack.clear();
    for (const auto &val : arr) {
        if (val.isObject()) {
            m_stack.append(ClosedTabInfo::fromJson(val.toObject()));
        }
    }
    while (m_stack.size() > kMaxClosedTabs) {
        m_stack.removeFirst();
    }
    emit stackChanged();
}
