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

#include "AutosaveRecoveryManager.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

AutosaveRecoveryManager::AutosaveRecoveryManager(QObject *parent)
    : QObject(parent), m_recoveryDir(defaultRecoveryPath()) {
    m_timer.setInterval(kDefaultIntervalMs);
    connect(&m_timer, &QTimer::timeout, this, &AutosaveRecoveryManager::triggerAutosave);
}

AutosaveRecoveryManager *AutosaveRecoveryManager::instance() {
    static AutosaveRecoveryManager s_instance;
    return &s_instance;
}

QString AutosaveRecoveryManager::defaultRecoveryPath() {
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (basePath.isEmpty()) {
        basePath = QDir::homePath() + QStringLiteral("/.config/NotepadNext");
    }
    return QDir::cleanPath(basePath + QStringLiteral("/autosave"));
}

void AutosaveRecoveryManager::setRecoveryDirectory(const QString &dirPath) {
    m_recoveryDir = dirPath;
    (void)QDir().mkpath(m_recoveryDir);
    (void)QFile::setPermissions(m_recoveryDir,
                                QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
}

const QString &AutosaveRecoveryManager::recoveryDirectory() const {
    return m_recoveryDir;
}

QString AutosaveRecoveryManager::sanitizeKey(const QString &raw) {
    const QByteArray hash = QCryptographicHash::hash(raw.toUtf8(), QCryptographicHash::Sha256).toHex();
    return QString::fromLatin1(hash.left(16));
}

QString AutosaveRecoveryManager::saveSnapshot(const QString &originalPath, const QString &title, const QString &content,
                                              qint64 cursorPosition) const {
    const QDir dir(m_recoveryDir);
    if (!dir.exists()) {
        (void)dir.mkpath(QStringLiteral("."));
        (void)QFile::setPermissions(m_recoveryDir,
                                    QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
    }

    const bool isUntitled = originalPath.isEmpty();
    const QString identifier = isUntitled ? title : originalPath;
    const QString fileName = QStringLiteral("draft_%1.json").arg(sanitizeKey(identifier));
    QString targetPath = dir.filePath(fileName);

    QJsonObject obj;
    obj[QStringLiteral("originalPath")] = originalPath;
    obj[QStringLiteral("title")] = title;
    obj[QStringLiteral("content")] = content;
    obj[QStringLiteral("cursorPosition")] = cursorPosition;
    obj[QStringLiteral("timestamp")] = QDateTime::currentSecsSinceEpoch();
    obj[QStringLiteral("isUntitled")] = isUntitled;

    const QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);

    if (QFile file(targetPath); file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        (void)file.write(data);
        file.flush();
        file.close();
        (void)file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
        return targetPath;
    }

    return QString();
}

void AutosaveRecoveryManager::removeSnapshot(const QString &identifier) const {
    if (identifier.isEmpty()) {
        return;
    }
    const QString fileName = QStringLiteral("draft_%1.json").arg(sanitizeKey(identifier));
    (void)QFile::remove(QDir(m_recoveryDir).filePath(fileName));
}

QList<RecoverySnapshot> AutosaveRecoveryManager::recoverableSnapshots() const {
    QList<RecoverySnapshot> list;
    const QDir dir(m_recoveryDir);
    if (!dir.exists()) {
        return list;
    }

    const QStringList files = dir.entryList({QStringLiteral("draft_*.json")}, QDir::Files);
    for (const auto &file : files) {
        const QString fullPath = dir.filePath(file);
        if (QFile f(fullPath); f.open(QIODevice::ReadOnly)) {
            const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            if (doc.isObject()) {
                const QJsonObject obj = doc.object();
                RecoverySnapshot snap;
                snap.recoveryFilePath = fullPath;
                snap.originalPath = obj[QStringLiteral("originalPath")].toString();
                snap.title = obj[QStringLiteral("title")].toString();
                snap.content = obj[QStringLiteral("content")].toString();
                snap.cursorPosition = obj[QStringLiteral("cursorPosition")].toInteger();
                snap.timestamp = obj[QStringLiteral("timestamp")].toInteger();
                snap.isUntitled = obj[QStringLiteral("isUntitled")].toBool();
                list.append(snap);
            }
        }
    }
    return list;
}

void AutosaveRecoveryManager::clearSnapshots() const {
    const QDir dir(m_recoveryDir);
    if (!dir.exists()) {
        return;
    }
    const QStringList files = dir.entryList({QStringLiteral("draft_*.json")}, QDir::Files);
    for (const auto &file : files) {
        (void)QFile::remove(dir.filePath(file));
    }
}

void AutosaveRecoveryManager::setIntervalMs(int ms) {
    m_timer.setInterval(ms);
}

int AutosaveRecoveryManager::intervalMs() const {
    return m_timer.interval();
}

void AutosaveRecoveryManager::start() {
    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void AutosaveRecoveryManager::stop() {
    if (m_timer.isActive()) {
        m_timer.stop();
    }
}
