#pragma once
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

#include <QColor>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QString>
#include <cstdint>
#include <optional>

struct ClosedTabInfo {
    QString filePath;
    qint64 cursorPosition{0};
    qint64 firstVisibleLine{0};
    QColor tabColor;
    QString encoding;
    int eolMode{0};
    qint64 timestamp{0};
    bool isUntitled{false};
    QString untitledContent;
    QString untitledName;

    [[nodiscard]] QJsonObject toJson() const;
    static ClosedTabInfo fromJson(const QJsonObject &obj);
};

class ClosedTabsManager : public QObject {
    Q_OBJECT

public:
    static constexpr int kMaxClosedTabs = 25;

    explicit ClosedTabsManager(QObject *parent = nullptr);
    ~ClosedTabsManager() override = default;

    static ClosedTabsManager *instance();

    void pushClosedTab(const ClosedTabInfo &info);
    std::optional<ClosedTabInfo> popClosedTab();
    [[nodiscard]] bool canReopen() const;
    [[nodiscard]] int count() const;
    void clear();

    [[nodiscard]] const QList<ClosedTabInfo> &allClosedTabs() const;

    [[nodiscard]] QJsonArray toJson() const;
    void fromJson(const QJsonArray &arr);

signals:
    void stackChanged();

private:
    QList<ClosedTabInfo> m_stack;
};
