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

#include <QDateTime>
#include <QString>
#include <cstdint>

struct JwtInfo {
    QString headerJson;
    QString payloadJson;
    QString signature;
    QDateTime expirationDate;
    QDateTime issuedAtDate;
    bool isValid{false};
    QString error;
};

struct PasswordStrength {
    int score{0};         // 0 to 4
    double entropy{0.0};  // bits of entropy
    QString label;        // "Very Weak", "Weak", "Medium", "Strong", "Very Strong"
    bool isStrong{false};
    QString feedback;
};

enum class HtmlEncryptMethod : std::uint8_t { JavascriptEscape = 0, Base64 = 1, PasswordProtected = 2 };

class DeveloperToolsEngine {
public:
    // JSON Prettify & Minify
    static QString formatJson(const QString &json, int indentSpaces = 4, QString *errorMsg = nullptr);
    static QString minifyJson(const QString &json, QString *errorMsg = nullptr);

    // XML Format & Minify
    static QString formatXml(const QString &xml, int indentSpaces = 2, QString *errorMsg = nullptr);
    static QString minifyXml(const QString &xml, QString *errorMsg = nullptr);

    // HTML Format & Minify
    static QString formatHtml(const QString &html, int indentSpaces = 2, QString *errorMsg = nullptr);
    static QString minifyHtml(const QString &html, QString *errorMsg = nullptr);

    // CSS Format & Minify
    static QString formatCss(const QString &css, QString *errorMsg = nullptr);
    static QString minifyCss(const QString &css, QString *errorMsg = nullptr);

    // JavaScript Format & Minify
    static QString formatJs(const QString &js, QString *errorMsg = nullptr);
    static QString minifyJs(const QString &js, QString *errorMsg = nullptr);

    // HTML Encryption & Decryption
    static QString encryptHtml(const QString &rawHtml, HtmlEncryptMethod method = HtmlEncryptMethod::JavascriptEscape,
                               const QString &password = QString(), QString *errorMsg = nullptr);
    static QString decryptHtml(const QString &input, QString *errorMsg = nullptr);

    // JWT Inspector
    static JwtInfo parseJwt(const QString &jwtToken);

    // URL & Unicode Encoding
    static QString urlEncode(const QString &text);
    static QString urlDecode(const QString &text);
    static QString unicodeEscape(const QString &text);
    static QString unicodeUnescape(const QString &text);

    // HTML Entities
    static QString htmlEntityEncode(const QString &text);
    static QString htmlEntityDecode(const QString &text);

    // Base64 Suite
    static QString base64EncodeLines(const QString &text);
    static QString base64DecodeLines(const QString &text);

    // Quoted-Printable (RFC 2045)
    static QString quotedPrintableEncode(const QString &text);
    static QString quotedPrintableDecode(const QString &text);

    // ASCII <-> Hex
    static QString asciiToHex(const QString &ascii, const QString &separator = QStringLiteral(" "));
    static QString hexToAscii(const QString &hex, QString *errorMsg = nullptr);

    // Radix Base Conversion (2, 8, 10, 16)
    static QString convertRadix(const QString &input, int fromBase, int toBase, bool uppercase = true,
                                QString *errorMsg = nullptr);

    // Advanced Case Conversion
    static QString toTitleCase(const QString &text);
    static QString toInvertCase(const QString &text);
    static QString toSentenceCase(const QString &text);

    // Password Strength & Entropy
    static PasswordStrength evaluatePasswordStrength(const QString &password);

    // UUID & Epoch Time
    static QString generateUuidV4(bool uppercase = false, bool withBraces = false, bool withDashes = true);
    static QString epochToIso(qint64 epochSeconds);
    static qint64 isoToEpoch(const QString &isoString, bool *ok = nullptr);
};
