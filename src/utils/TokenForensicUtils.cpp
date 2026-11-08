/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "TokenForensicUtils.h"
#include <QRegularExpression>
#include <QStringList>
#include <QTimeZone>
#include <algorithm>
#include <array>
#include <cstddef>

double TokenForensicUtils::calculateShannonEntropy(const QByteArray &data) {
    if (data.isEmpty()) {
        return 0.0;
    }

    std::array<std::size_t, 256> frequencies{};
    frequencies.fill(0);

    for (const unsigned char byte : data) {
        frequencies.at(byte)++;
    }

    const auto total = static_cast<double>(data.size());
    double entropy = 0.0;

    for (const std::size_t count : frequencies) {
        if (count > 0) {
            const auto prob = static_cast<double>(count) / total;
            entropy -= prob * (std::log(prob) / std::log(2.0));
        }
    }

    return entropy;
}

double TokenForensicUtils::calculateShannonEntropy(const QString &text) {
    return calculateShannonEntropy(text.toUtf8());
}

bool TokenForensicUtils::isLikelyJwt(const QString &token) {
    const QString trimmed = token.trimmed();
    const QStringList parts = trimmed.split(QLatin1Char('.'));
    if (parts.size() != 3) {
        return false;
    }

    static const QRegularExpression b64UrlRegex(QStringLiteral("^[A-Za-z0-9_-]+$"));
    return b64UrlRegex.match(parts[0]).hasMatch() && b64UrlRegex.match(parts[1]).hasMatch();
}

TokenForensicUtils::JwtInfo TokenForensicUtils::decodeJwt(const QString &token) {
    JwtInfo info;
    const QString trimmed = token.trimmed();
    const QStringList parts = trimmed.split(QLatin1Char('.'));

    if (parts.size() != 3) {
        info.valid = false;
        info.errorMessage = QStringLiteral("Invalid JWT format (expected 3 dot-separated segments)");
        return info;
    }

    const QByteArray headerBytes = QByteArray::fromBase64(parts[0].toUtf8(), QByteArray::Base64UrlEncoding);
    const QByteArray payloadBytes = QByteArray::fromBase64(parts[1].toUtf8(), QByteArray::Base64UrlEncoding);

    QJsonParseError err;
    const QJsonDocument headerDoc = QJsonDocument::fromJson(headerBytes, &err);
    if (err.error != QJsonParseError::NoError || !headerDoc.isObject()) {
        info.valid = false;
        info.errorMessage = QStringLiteral("Header JSON parse error: %1").arg(err.errorString());
        return info;
    }

    const QJsonDocument payloadDoc = QJsonDocument::fromJson(payloadBytes, &err);
    if (err.error != QJsonParseError::NoError || !payloadDoc.isObject()) {
        info.valid = false;
        info.errorMessage = QStringLiteral("Payload JSON parse error: %1").arg(err.errorString());
        return info;
    }

    info.valid = true;
    info.header = headerDoc.object();
    info.payload = payloadDoc.object();
    info.signature = parts[2];

    if (info.payload.contains(QStringLiteral("iat"))) {
        const qint64 iatSec = info.payload.value(QStringLiteral("iat")).toInteger();
        info.issuedAt = QDateTime::fromSecsSinceEpoch(iatSec, QTimeZone::utc());
    }

    if (info.payload.contains(QStringLiteral("exp"))) {
        const qint64 expSec = info.payload.value(QStringLiteral("exp")).toInteger();
        info.expiration = QDateTime::fromSecsSinceEpoch(expSec, QTimeZone::utc());
        info.isExpired = (info.expiration < QDateTime::currentDateTimeUtc());
    }

    QString out;
    out += QStringLiteral("=== JSON WEB TOKEN (JWT) DECODE ===\n\n");
    out += QStringLiteral("[HEADER]\n");
    out += QString::fromUtf8(headerDoc.toJson(QJsonDocument::Indented)) + QLatin1Char('\n');

    out += QStringLiteral("[PAYLOAD]\n");
    out += QString::fromUtf8(payloadDoc.toJson(QJsonDocument::Indented)) + QLatin1Char('\n');

    out += QStringLiteral("[METADATA]\n");
    if (info.issuedAt.isValid()) {
        out += QStringLiteral("Issued At (iat): %1 UTC\n").arg(info.issuedAt.toString(Qt::ISODate));
    }
    if (info.expiration.isValid()) {
        out += QStringLiteral("Expires (exp):   %1 UTC (%2)\n")
                   .arg(info.expiration.toString(Qt::ISODate))
                   .arg(info.isExpired ? QStringLiteral("EXPIRED") : QStringLiteral("ACTIVE"));
    }
    out += QStringLiteral("Signature:       %1\n").arg(info.signature);

    info.formattedOutput = std::move(out);
    return info;
}

TokenForensicUtils::DocumentStats TokenForensicUtils::analyzeText(const QString &text) {
    DocumentStats stats;
    stats.totalChars = text.length();

    const auto lines = text.split(QLatin1Char('\n'));
    stats.lines = lines.size();
    stats.nonEmptyLines =
        std::ranges::count_if(lines, [](const QString &line) { return !line.trimmed().isEmpty(); });

    stats.charsNoSpaces = std::ranges::count_if(text, [](const QChar &ch) { return !ch.isSpace(); });

    static const QRegularExpression wordRegex(QStringLiteral("\\S+"));
    auto it = wordRegex.globalMatch(text);
    qsizetype wordCount = 0;
    while (it.hasNext()) {
        it.next();
        wordCount++;
    }
    stats.words = wordCount;

    const QByteArray utf8 = text.toUtf8();
    stats.bytesUtf8 = utf8.size();

    stats.readingTimeMinutes = (stats.words > 0) ? (static_cast<double>(stats.words) / 200.0) : 0.0;
    stats.speakingTimeMinutes = (stats.words > 0) ? (static_cast<double>(stats.words) / 130.0) : 0.0;

    stats.shannonEntropy = calculateShannonEntropy(utf8);
    stats.isHighEntropy = (stats.shannonEntropy >= 7.2 && stats.bytesUtf8 > 64);

    return stats;
}
