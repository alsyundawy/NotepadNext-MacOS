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

#include "NotepadNextTransferManager.h"

#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>
#include <QSysInfo>

#include <openssl/evp.h>
#include <openssl/rand.h>

QString NotepadNextTransferManager::getAppConfigDirectory()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty()) {
        configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    return QDir::cleanPath(configDir);
}

bool NotepadNextTransferManager::isPathContained(const QString &baseDir, const QString &targetPath)
{
    const QString cleanBase = QDir::cleanPath(baseDir);
    const QString cleanTarget = QDir::cleanPath(targetPath);

    const QFileInfo baseInfo(cleanBase);
    const QFileInfo targetInfo(cleanTarget);

    const QString canonicalBase = baseInfo.exists() ? baseInfo.canonicalFilePath() : cleanBase;
    const QString canonicalTarget = targetInfo.exists() ? targetInfo.canonicalFilePath() : cleanTarget;

    if (canonicalBase.isEmpty() || canonicalTarget.isEmpty()) {
        return false;
    }

    if (canonicalTarget == canonicalBase) {
        return true;
    }

    const QString basePrefix =
        canonicalBase.endsWith(QLatin1Char('/')) ? canonicalBase : (canonicalBase + QLatin1Char('/'));
    return canonicalTarget.startsWith(basePrefix);
}

QString NotepadNextTransferManager::sanitizeRelativePath(const QString &relPath)
{
    if (relPath.startsWith(QLatin1Char('/')) || relPath.startsWith(QLatin1Char('\\')) ||
        QDir::isAbsolutePath(relPath)) {
        return {};
    }
    const QString clean = QDir::cleanPath(relPath).replace(QLatin1Char('\\'), QLatin1Char('/'));
    if (clean.contains(QStringLiteral("..")) || clean.startsWith(QLatin1Char('/')) ||
        clean.contains(QLatin1Char(':'))) {
        return {};
    }
    return clean;
}

QString NotepadNextTransferManager::translatePlatformPath(const QString &originalPath, const QString &sourceOs)
{
    const QString currentConfig = getAppConfigDirectory();
    QString translated = originalPath;

    if (sourceOs == QStringLiteral("macos") || sourceOs == QStringLiteral("darwin")) {
        const qsizetype idx = translated.indexOf(QStringLiteral("Library/Application Support/NotepadNext"));
        if (idx != -1) {
            const QString sub = translated.mid(idx + 39);
            return QDir::cleanPath(currentConfig + QLatin1Char('/') + sub);
        }
    } else if (sourceOs == QStringLiteral("linux") || sourceOs == QStringLiteral("freebsd")) {
        const qsizetype idx = translated.indexOf(QStringLiteral(".config/NotepadNext"));
        if (idx != -1) {
            const QString sub = translated.mid(idx + 19);
            return QDir::cleanPath(currentConfig + QLatin1Char('/') + sub);
        }
    }

    return QDir::cleanPath(currentConfig + QLatin1Char('/') + QFileInfo(translated).fileName());
}

static bool encryptAes256(const QByteArray &plaintext, const QString &passphrase, QByteArray &outData)
{
    unsigned char salt[16];
    if (RAND_bytes(salt, sizeof(salt)) != 1) {
        return false;
    }

    unsigned char key[32];
    unsigned char iv[16];
    const QByteArray passBytes = passphrase.toUtf8();
    if (PKCS5_PBKDF2_HMAC(passBytes.constData(), static_cast<int>(passBytes.size()),
                          salt, sizeof(salt), 10000, EVP_sha256(),
                          sizeof(key), key) != 1) {
        return false;
    }
    // Derive IV
    if (PKCS5_PBKDF2_HMAC(passBytes.constData(), static_cast<int>(passBytes.size()),
                          salt, sizeof(salt), 1000, EVP_sha256(),
                          sizeof(iv), iv) != 1) {
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    QByteArray ciphertext;
    ciphertext.resize(static_cast<qsizetype>(plaintext.size()) + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len = 0;
    int ciphertext_len = 0;

    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(ciphertext.data()), &len,
                          reinterpret_cast<const unsigned char *>(plaintext.constData()), static_cast<int>(plaintext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(ciphertext.data()) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ciphertext_len += len;
    ciphertext.resize(ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);

    outData.clear();
    outData.append(NotepadNextTransferManager::BUNDLE_MAGIC);
    outData.append(":ENC:");
    outData.append(reinterpret_cast<const char *>(salt), sizeof(salt));
    outData.append(ciphertext);
    return true;
}

static bool decryptAes256(const QByteArray &inData, const QString &passphrase, QByteArray &outPlaintext)
{
    const QByteArray magicPrefix = QByteArray(NotepadNextTransferManager::BUNDLE_MAGIC) + ":ENC:";
    if (!inData.startsWith(magicPrefix)) {
        return false;
    }

    const int headerSize = static_cast<int>(magicPrefix.size()) + 16;
    if (inData.size() < headerSize) {
        return false;
    }

    const unsigned char *salt = reinterpret_cast<const unsigned char *>(inData.constData() + magicPrefix.size());
    const unsigned char *ciphertext = reinterpret_cast<const unsigned char *>(inData.constData() + headerSize);
    const int ciphertext_len = static_cast<int>(inData.size()) - headerSize;

    unsigned char key[32];
    unsigned char iv[16];
    const QByteArray passBytes = passphrase.toUtf8();
    if (PKCS5_PBKDF2_HMAC(passBytes.constData(), static_cast<int>(passBytes.size()),
                          salt, 16, 10000, EVP_sha256(),
                          sizeof(key), key) != 1) {
        return false;
    }
    if (PKCS5_PBKDF2_HMAC(passBytes.constData(), static_cast<int>(passBytes.size()),
                          salt, 16, 1000, EVP_sha256(),
                          sizeof(iv), iv) != 1) {
        return false;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    outPlaintext.resize(ciphertext_len + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len = 0;
    int plaintext_len = 0;

    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(outPlaintext.data()), &len,
                          ciphertext, ciphertext_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(outPlaintext.data()) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return false; // Wrong passphrase or corrupted data
    }
    plaintext_len += len;
    outPlaintext.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

NotepadNextTransferManager::TransferResult NotepadNextTransferManager::exportBundle(
    const QString &targetFilePath, const ExportOptions &options)
{
    TransferResult result;
    const QString configDir = getAppConfigDirectory();
    QDir base(configDir);

    if (!base.exists()) {
        result.message = QStringLiteral("Configuration directory not found: ") + configDir;
        return result;
    }

    QJsonObject rootObj;
    rootObj[QStringLiteral("magic")] = QString::fromLatin1(BUNDLE_MAGIC);
    rootObj[QStringLiteral("version")] = 1;
    rootObj[QStringLiteral("timestamp")] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    rootObj[QStringLiteral("sourceOs")] = QSysInfo::productType();
    rootObj[QStringLiteral("sourceConfigDir")] = configDir;

    QJsonArray filesArray;
    int count = 0;
    qint64 totalBytes = 0;

    QDirIterator it(configDir, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        const QString filePath = it.filePath();
        const QString relPath = base.relativeFilePath(filePath);

        // Filter based on options
        if (relPath.endsWith(QStringLiteral(".ini")) || relPath.endsWith(QStringLiteral(".json"))) {
            if (!options.includePreferences) continue;
        } else if (relPath.contains(QStringLiteral("themes")) || relPath.endsWith(QStringLiteral(".qss"))) {
            if (!options.includeThemes) continue;
        } else if (relPath.contains(QStringLiteral("session"))) {
            if (!options.includeSessions) continue;
        } else if (relPath.endsWith(QStringLiteral(".lua")) || relPath.endsWith(QStringLiteral(".py"))) {
            if (!options.includeScripts) continue;
        }

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) continue;
        const QByteArray content = file.readAll();
        file.close();

        QJsonObject fileObj;
        fileObj[QStringLiteral("path")] = relPath;
        fileObj[QStringLiteral("data")] = QString::fromLatin1(content.toBase64());
        fileObj[QStringLiteral("sha256")] = QString::fromLatin1(
            QCryptographicHash::hash(content, QCryptographicHash::Sha256).toHex());
        fileObj[QStringLiteral("size")] = static_cast<qint64>(content.size());

        filesArray.append(fileObj);
        count++;
        totalBytes += content.size();
    }

    rootObj[QStringLiteral("files")] = filesArray;
    const QByteArray jsonBytes = QJsonDocument(rootObj).toJson(QJsonDocument::Compact);

    QByteArray finalData;
    if (!options.passphrase.isEmpty()) {
        if (!encryptAes256(jsonBytes, options.passphrase, finalData)) {
            result.message = QStringLiteral("Failed to encrypt bundle with passphrase.");
            return result;
        }
    } else {
        finalData.append(BUNDLE_MAGIC);
        finalData.append(":RAW:\n");
        finalData.append(jsonBytes);
    }

    QSaveFile outFile(targetFilePath);
    if (!outFile.open(QIODevice::WriteOnly) || outFile.write(finalData) != finalData.size() || !outFile.commit()) {
        result.message = QStringLiteral("Failed to write bundle to: ") + targetFilePath;
        return result;
    }

    result.success = true;
    result.filesCount = count;
    result.totalBytes = totalBytes;
    result.bundleSha256 = QString::fromLatin1(
        QCryptographicHash::hash(finalData, QCryptographicHash::Sha256).toHex());
    result.message = QStringLiteral("Successfully exported %1 files (%2 bytes).").arg(count).arg(totalBytes);
    return result;
}

NotepadNextTransferManager::TransferResult NotepadNextTransferManager::inspectBundle(
    const QString &bundleFilePath, const QString &passphrase, QStringList *outFiles)
{
    TransferResult result;
    QFile file(bundleFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        result.message = QStringLiteral("Cannot open bundle file.");
        return result;
    }

    const QByteArray data = file.readAll();
    file.close();

    QByteArray jsonData;
    if (data.startsWith(QByteArray(BUNDLE_MAGIC) + ":ENC:")) {
        if (passphrase.isEmpty()) {
            result.message = QStringLiteral("Bundle is encrypted. Passphrase required.");
            return result;
        }
        if (!decryptAes256(data, passphrase, jsonData)) {
            result.message = QStringLiteral("Failed to decrypt bundle. Invalid passphrase.");
            return result;
        }
    } else if (data.startsWith(QByteArray(BUNDLE_MAGIC) + ":RAW:\n")) {
        jsonData = data.mid(static_cast<int>(strlen(BUNDLE_MAGIC)) + 6);
    } else {
        result.message = QStringLiteral("Invalid bundle format (unrecognized magic header).");
        return result;
    }

    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(jsonData, &err);
    if (doc.isNull() || !doc.isObject()) {
        result.message = QStringLiteral("Corrupted bundle JSON: ") + err.errorString();
        return result;
    }

    const QJsonObject root = doc.object();
    const QJsonArray files = root[QStringLiteral("files")].toArray();

    result.filesCount = files.size();
    if (outFiles) {
        outFiles->clear();
        for (const auto &val : files) {
            outFiles->append(val.toObject()[QStringLiteral("path")].toString());
        }
    }

    result.success = true;
    result.message = QStringLiteral("Valid bundle containing %1 items.").arg(files.size());
    return result;
}

NotepadNextTransferManager::TransferResult NotepadNextTransferManager::importBundle(
    const QString &bundleFilePath, const QString &passphrase)
{
    TransferResult result;
    QFile file(bundleFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        result.message = QStringLiteral("Cannot open bundle file.");
        return result;
    }

    const QByteArray data = file.readAll();
    file.close();

    QByteArray jsonData;
    if (data.startsWith(QByteArray(BUNDLE_MAGIC) + ":ENC:")) {
        if (passphrase.isEmpty()) {
            result.message = QStringLiteral("Bundle is encrypted. Passphrase required.");
            return result;
        }
        if (!decryptAes256(data, passphrase, jsonData)) {
            result.message = QStringLiteral("Failed to decrypt bundle. Invalid passphrase.");
            return result;
        }
    } else if (data.startsWith(QByteArray(BUNDLE_MAGIC) + ":RAW:\n")) {
        jsonData = data.mid(static_cast<int>(strlen(BUNDLE_MAGIC)) + 6);
    } else {
        result.message = QStringLiteral("Invalid bundle format.");
        return result;
    }

    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(jsonData, &err);
    if (doc.isNull() || !doc.isObject()) {
        result.message = QStringLiteral("Corrupted bundle JSON payload.");
        return result;
    }

    const QJsonObject root = doc.object();
    const QString sourceOs = root[QStringLiteral("sourceOs")].toString();
    Q_UNUSED(sourceOs);
    const QJsonArray files = root[QStringLiteral("files")].toArray();
    const QString targetConfigDir = getAppConfigDirectory();
    QDir targetBase(targetConfigDir);

    int restoredCount = 0;
    qint64 totalBytes = 0;

    for (const auto &val : files) {
        const QJsonObject item = val.toObject();
        const QString relPath = sanitizeRelativePath(item[QStringLiteral("path")].toString());
        if (relPath.isEmpty()) continue;

        const QByteArray content = QByteArray::fromBase64(item[QStringLiteral("data")].toString().toLatin1());
        const QString expectedSha = item[QStringLiteral("sha256")].toString();
        const QString actualSha = QString::fromLatin1(
            QCryptographicHash::hash(content, QCryptographicHash::Sha256).toHex());

        if (!expectedSha.isEmpty() && expectedSha != actualSha) {
            continue; // Skip corrupted file
        }

        const QString fullTargetPath = targetBase.filePath(relPath);
        if (!isPathContained(targetConfigDir, fullTargetPath)) {
            continue; // Prevent directory traversal
        }

        QFileInfo(fullTargetPath).dir().mkpath(QStringLiteral("."));
        QSaveFile saveFile(fullTargetPath);
        if (saveFile.open(QIODevice::WriteOnly) &&
            saveFile.write(content) == content.size() &&
            saveFile.commit()) {
            restoredCount++;
            totalBytes += content.size();
        }
    }

    result.success = true;
    result.filesCount = restoredCount;
    result.totalBytes = totalBytes;
    result.message = QStringLiteral("Successfully restored %1 configuration files.").arg(restoredCount);
    return result;
}
