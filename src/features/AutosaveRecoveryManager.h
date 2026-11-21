#pragma once
/*
 * This file is part of Notepad Next (macOS Edition).
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>
#include <cstdint>

struct RecoverySnapshot {
    QString recoveryFilePath;
    QString originalPath;
    QString title;
    QString content;
    qint64 cursorPosition{0};
    qint64 timestamp{0};
    bool isUntitled{false};
};

class AutosaveRecoveryManager : public QObject {
    Q_OBJECT

public:
    static constexpr int kDefaultIntervalMs = 10000;  // 10 seconds

    explicit AutosaveRecoveryManager(QObject *parent = nullptr);
    ~AutosaveRecoveryManager() override = default;

    static AutosaveRecoveryManager *instance();

    void setRecoveryDirectory(const QString &dirPath);
    [[nodiscard]] const QString &recoveryDirectory() const;

    QString saveSnapshot(const QString &originalPath, const QString &title, const QString &content,
                         qint64 cursorPosition = 0) const;
    void removeSnapshot(const QString &identifier) const;
    [[nodiscard]] QList<RecoverySnapshot> recoverableSnapshots() const;
    void clearSnapshots() const;

    void setIntervalMs(int ms);
    [[nodiscard]] int intervalMs() const;

    void start();
    void stop();

signals:
    void triggerAutosave();

private:
    [[nodiscard]] static QString defaultRecoveryPath();
    [[nodiscard]] static QString sanitizeKey(const QString &raw);

    QString m_recoveryDir;
    QTimer m_timer;
};
