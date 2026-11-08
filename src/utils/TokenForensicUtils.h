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

#ifndef TOKENFORENSICUTILS_H
#define TOKENFORENSICUTILS_H

#include <cmath>
#include <QByteArray>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

class TokenForensicUtils {
public:
    struct JwtInfo {
        bool valid{false};
        QJsonObject header;
        QJsonObject payload;
        QString signature;
        QDateTime issuedAt;
        QDateTime expiration;
        bool isExpired{false};
        QString formattedOutput;
        QString errorMessage;
    };

    static double calculateShannonEntropy(const QString &text);
    static double calculateShannonEntropy(const QByteArray &data);

    static bool isLikelyJwt(const QString &token);
    static JwtInfo decodeJwt(const QString &token);

    struct DocumentStats {
        qsizetype totalChars{0};
        qsizetype charsNoSpaces{0};
        qsizetype words{0};
        qsizetype lines{0};
        qsizetype nonEmptyLines{0};
        qsizetype bytesUtf8{0};
        double readingTimeMinutes{0.0};
        double speakingTimeMinutes{0.0};
        double shannonEntropy{0.0};
        bool isHighEntropy{false};
    };

    static DocumentStats analyzeText(const QString &text);
};

#endif  // TOKENFORENSICUTILS_H
