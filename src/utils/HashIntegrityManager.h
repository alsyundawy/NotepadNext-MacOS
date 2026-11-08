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

#ifndef HASHINTEGRITYMANAGER_H
#define HASHINTEGRITYMANAGER_H

#include <QString>
#include <QCryptographicHash>
#include <functional>

class HashIntegrityManager {
public:
    HashIntegrityManager() = default;

    static QString computeFileHash(const QString &filePath,
                                   QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha256,
                                   bool *ok = nullptr,
                                   const std::function<void(qint64, qint64)> &progressCallback = nullptr);

    static QString computeTextHash(const QString &text,
                                   QCryptographicHash::Algorithm algorithm = QCryptographicHash::Sha256);

    static bool verifyHashMatch(const QString &computedHash, const QString &expectedHash);
};

#endif  // HASHINTEGRITYMANAGER_H
