#pragma once
/*
 * This file is part of NotepadNext.
 *
 * Implements Workspace System Transfer & Portable Configuration Bundle (.npnbundle)
 * with platform path translation (macOS <-> FreeBSD <-> Linux), Zip-Slip containment,
 * SHA-256 digest validation, and optional AES-256-CBC encryption.
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 */

#ifndef NOTEPADNEXTTRANSFERMANAGER_H
#define NOTEPADNEXTTRANSFERMANAGER_H

#include <QByteArray>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QStringList>

class NotepadNextTransferManager {
public:
    static constexpr const char *BUNDLE_MAGIC = "NPNBUNDLE01";
    static constexpr size_t MAX_BUNDLE_SIZE = 100ULL * 1024ULL * 1024ULL; // 100 MB safety cap

    struct ExportOptions {
        bool includePreferences{true};
        bool includeThemes{true};
        bool includeSessions{true};
        bool includeScripts{true};
        QString passphrase; // Optional AES-256 encryption
    };

    struct TransferResult {
        bool success{false};
        QString message;
        int filesCount{0};
        qint64 totalBytes{0};
        QString bundleSha256;
    };

    // Bundle Operations
    static TransferResult exportBundle(const QString &targetFilePath, const ExportOptions &options);
    static TransferResult importBundle(const QString &bundleFilePath, const QString &passphrase = QString());
    static TransferResult inspectBundle(const QString &bundleFilePath, const QString &passphrase = QString(),
                                        QStringList *outFiles = nullptr);

    // Security & Path Normalization Helpers
    static bool isPathContained(const QString &baseDir, const QString &targetPath);
    static QString sanitizeRelativePath(const QString &relPath);
    static QString getAppConfigDirectory();
    static QString translatePlatformPath(const QString &originalPath, const QString &sourceOs);
};

#endif // NOTEPADNEXTTRANSFERMANAGER_H
