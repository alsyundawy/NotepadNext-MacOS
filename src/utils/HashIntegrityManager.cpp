/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "HashIntegrityManager.h"
#include <QFile>
#include <QFileInfo>
#include <QtGlobal>
#include <QByteArrayView>
#include <vector>

QString HashIntegrityManager::computeFileHash(const QString &filePath, QCryptographicHash::Algorithm algorithm,
                                              bool *ok, const std::function<void(qint64, qint64)> &progressCallback) {
    if (ok) {
        *ok = false;
    }

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        return {};
    }

    QCryptographicHash hasher(algorithm);
    const qint64 totalBytes = file.size();
    qint64 processedBytes = 0;

    constexpr qint64 kChunkSize = 64LL * 1024LL;
    std::vector<char> buffer(kChunkSize);

    while (!file.atEnd()) {
        const qint64 bytesRead = file.read(buffer.data(), kChunkSize);
        if (bytesRead > 0) {
            hasher.addData(QByteArrayView(buffer.data(), bytesRead));
            processedBytes += bytesRead;
            if (progressCallback) {
                progressCallback(processedBytes, totalBytes);
            }
        } else if (bytesRead < 0) {
            return {};
        }
    }

    if (ok) {
        *ok = true;
    }
    return QString::fromLatin1(hasher.result().toHex().toLower());
}

QString HashIntegrityManager::computeTextHash(const QString &text, QCryptographicHash::Algorithm algorithm) {
    QCryptographicHash hasher(algorithm);
    hasher.addData(text.toUtf8());
    return QString::fromLatin1(hasher.result().toHex().toLower());
}

bool HashIntegrityManager::verifyHashMatch(const QString &computedHash, const QString &expectedHash) {
    const QString cleanComputed = computedHash.trimmed().toLower();
    const QString cleanExpected = expectedHash.trimmed().toLower();

    if (cleanComputed.isEmpty() || cleanExpected.isEmpty()) {
        return false;
    }

    return cleanComputed == cleanExpected;
}
