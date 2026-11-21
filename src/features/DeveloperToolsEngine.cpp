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

#include "DeveloperToolsEngine.h"
#include <cmath>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <QByteArray>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QStringList>
#include <QTimeZone>
#include <QUrl>
#include <QUuid>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "../utils/CodeCompressPretty.h"
#include "../utils/MimeTools.h"

namespace {

QString adjustJsonIndentation(const QString &formattedJson, int indentSpaces) {
    if (indentSpaces == 4 || indentSpaces <= 0) {
        return formattedJson;
    }
    const QString targetIndent(indentSpaces, QLatin1Char(' '));
    const QStringList lines = formattedJson.split(QLatin1Char('\n'));
    QStringList adjusted;
    adjusted.reserve(lines.size());

    for (const QString &line : lines) {
        int leadingSpaces = 0;
        while (leadingSpaces < line.length() && line.at(leadingSpaces) == QLatin1Char(' ')) {
            ++leadingSpaces;
        }
        const int depth = leadingSpaces / 4;
        adjusted.append(targetIndent.repeated(depth) + line.mid(leadingSpaces));
    }
    return adjusted.join(QLatin1Char('\n'));
}

bool isVoidHtmlTag(const QString &tag) {
    static const QStringList s_voidTags = {QStringLiteral("area"),  QStringLiteral("base"),  QStringLiteral("br"),
                                           QStringLiteral("col"),   QStringLiteral("embed"), QStringLiteral("hr"),
                                           QStringLiteral("img"),   QStringLiteral("input"), QStringLiteral("link"),
                                           QStringLiteral("meta"),  QStringLiteral("param"), QStringLiteral("source"),
                                           QStringLiteral("track"), QStringLiteral("wbr")};
    return s_voidTags.contains(tag.toLower());
}

}  // namespace

QString DeveloperToolsEngine::formatJson(const QString &json, int indentSpaces, QString *errorMsg) {
    QJsonParseError parseErr;
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &parseErr);
    if (parseErr.error != QJsonParseError::NoError) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("JSON Error at offset %1: %2").arg(parseErr.offset).arg(parseErr.errorString());
        }
        return QString();
    }
    if (errorMsg) {
        errorMsg->clear();
    }
    const QString formatted = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    return adjustJsonIndentation(formatted, indentSpaces);
}

QString DeveloperToolsEngine::minifyJson(const QString &json, QString *errorMsg) {
    QJsonParseError parseErr;
    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &parseErr);
    if (parseErr.error != QJsonParseError::NoError) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("JSON Error at offset %1: %2").arg(parseErr.offset).arg(parseErr.errorString());
        }
        return QString();
    }
    if (errorMsg) {
        errorMsg->clear();
    }
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

QString DeveloperToolsEngine::formatXml(const QString &xml, int indentSpaces, QString *errorMsg) {
    const QString trimmed = xml.trimmed();
    if (trimmed.isEmpty()) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("Empty XML input");
        }
        return QString();
    }

    QXmlStreamReader reader(trimmed);
    QString output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(indentSpaces);

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.hasError()) {
            if (errorMsg) {
                *errorMsg = QStringLiteral("XML Error (Line %1, Col %2): %3")
                                .arg(reader.lineNumber())
                                .arg(reader.columnNumber())
                                .arg(reader.errorString());
            }
            // Fallback to CodeCompressPretty parser
            const auto res = CodeCompressPretty::prettifyXml(trimmed);
            if (res.success) {
                if (errorMsg) {
                    errorMsg->clear();
                }
                return res.formatted;
            }
            return QString();
        }
        writer.writeCurrentToken(reader);
    }

    if (errorMsg) {
        errorMsg->clear();
    }
    return output;
}

QString DeveloperToolsEngine::minifyXml(const QString &xml, QString *errorMsg) {
    const auto res = CodeCompressPretty::compressXml(xml);
    if (!res.success) {
        if (errorMsg) {
            *errorMsg = res.errorMessage;
        }
        return QString();
    }
    if (errorMsg) {
        errorMsg->clear();
    }
    QString out = res.formatted.trimmed();
    static const QRegularExpression interTagWs(QStringLiteral(R"(>\s+<)"));
    out.replace(interTagWs, QStringLiteral("><"));
    static const QRegularExpression newlines(QStringLiteral(R"([\r\n]+)"));
    out.remove(newlines);
    return out;
}

QString DeveloperToolsEngine::formatHtml(const QString &html, int indentSpaces, QString *errorMsg) {
    const QString trimmed = html.trimmed();
    if (trimmed.isEmpty()) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("Empty HTML input");
        }
        return QString();
    }

    const QString indentStr(indentSpaces, QLatin1Char(' '));
    QString result;
    result.reserve(trimmed.length() * 2);

    int depth = 0;
    qsizetype pos = 0;
    const qsizetype len = trimmed.length();
    bool inPreformatted = false;

    while (pos < len) {
        if (!inPreformatted && trimmed.at(pos).isSpace()) {
            while (pos < len && trimmed.at(pos).isSpace()) {
                ++pos;
            }
            continue;
        }

        if (trimmed.at(pos) == QLatin1Char('<')) {
            const qsizetype endTag = trimmed.indexOf(QLatin1Char('>'), pos);
            if (endTag == -1) {
                result.append(trimmed.mid(pos));
                break;
            }

            const QString tagContent = trimmed.mid(pos, endTag - pos + 1);
            pos = endTag + 1;

            // Check if comment or doctype
            if (tagContent.startsWith(QStringLiteral("<!--")) ||
                tagContent.startsWith(QStringLiteral("<!DOCTYPE"), Qt::CaseInsensitive)) {
                if (!result.isEmpty() && !result.endsWith(QLatin1Char('\n'))) {
                    result.append(QLatin1Char('\n'));
                }
                result.append(indentStr.repeated(depth) + tagContent + QLatin1Char('\n'));
                continue;
            }

            // Extract tag name
            static const QRegularExpression tagNameRegex(QStringLiteral(R"(^<\/?([a-zA-Z0-9_-]+))"));
            const auto match = tagNameRegex.match(tagContent);
            const QString tagName = match.hasMatch() ? match.captured(1).toLower() : QString();

            if (tagName == QStringLiteral("pre") || tagName == QStringLiteral("textarea") ||
                tagName == QStringLiteral("code") || tagName == QStringLiteral("script") ||
                tagName == QStringLiteral("style")) {
                if (tagContent.startsWith(QStringLiteral("</"))) {
                    inPreformatted = false;
                } else if (!tagContent.endsWith(QStringLiteral("/>"))) {
                    inPreformatted = true;
                }
            }

            const bool isClosing = tagContent.startsWith(QStringLiteral("</"));
            const bool isSelfClosing = tagContent.endsWith(QStringLiteral("/>")) || isVoidHtmlTag(tagName);

            if (isClosing) {
                depth = std::max(0, depth - 1);
            }

            if (!inPreformatted) {
                if (!result.isEmpty() && !result.endsWith(QLatin1Char('\n'))) {
                    result.append(QLatin1Char('\n'));
                }
                result.append(indentStr.repeated(depth) + tagContent);
            } else {
                result.append(tagContent);
            }

            if (!isClosing && !isSelfClosing && !inPreformatted) {
                ++depth;
            }
            if (!inPreformatted) {
                result.append(QLatin1Char('\n'));
            }
        } else {
            // Text node
            const qsizetype nextTag = trimmed.indexOf(QLatin1Char('<'), pos);
            const qsizetype textEnd = (nextTag == -1) ? len : nextTag;
            QString textContent = trimmed.mid(pos, textEnd - pos);
            pos = textEnd;

            if (!inPreformatted) {
                textContent = textContent.trimmed();
                if (!textContent.isEmpty()) {
                    if (!result.isEmpty() && !result.endsWith(QLatin1Char('\n'))) {
                        result.append(QLatin1Char('\n'));
                    }
                    result.append(indentStr.repeated(depth) + textContent + QLatin1Char('\n'));
                }
            } else {
                result.append(textContent);
            }
        }
    }

    if (errorMsg) {
        errorMsg->clear();
    }
    return result.trimmed();
}

QString DeveloperToolsEngine::minifyHtml(const QString &html, QString *errorMsg) {
    QString text = html.trimmed();
    if (text.isEmpty()) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("Empty HTML input");
        }
        return QString();
    }

    // Protect <pre>, <code>, <script>, <style> blocks
    static const QRegularExpression protectRegex(QStringLiteral(R"(<(pre|code|script|style)[^>]*>[\s\S]*?<\/\1>)"),
                                                 QRegularExpression::CaseInsensitiveOption);

    QStringList preservedBlocks;
    qsizetype blockIdx = 0;
    auto match = protectRegex.match(text);
    while (match.hasMatch()) {
        const QString placeholder = QStringLiteral("___SPK_HTML_BLOCK_%1___").arg(blockIdx++);
        preservedBlocks.append(match.captured(0));
        text.replace(match.capturedStart(), match.capturedLength(), placeholder);
        match = protectRegex.match(text, match.capturedStart() + placeholder.length());
    }

    // Remove comments
    static const QRegularExpression commentRegex(QStringLiteral(R"(<!--(?!\[if)[\s\S]*?-->)"));
    text.remove(commentRegex);

    // Collapse whitespace between tags
    static const QRegularExpression interTagSpace(QStringLiteral(R"(>\s+<)"));
    text.replace(interTagSpace, QStringLiteral("><"));

    // Collapse multiple whitespace characters into single space
    static const QRegularExpression multiSpace(QStringLiteral(R"(\s{2,})"));
    text.replace(multiSpace, QStringLiteral(" "));

    // Restore preserved blocks
    for (int i = 0; i < preservedBlocks.size(); ++i) {
        text.replace(QStringLiteral("___SPK_HTML_BLOCK_%1___").arg(i), preservedBlocks.at(i));
    }

    if (errorMsg) {
        errorMsg->clear();
    }
    return text.trimmed();
}

QString DeveloperToolsEngine::formatCss(const QString &css, QString *errorMsg) {
    const auto res = CodeCompressPretty::prettifyCss(css);
    if (!res.success) {
        if (errorMsg) {
            *errorMsg = res.errorMessage;
        }
        return QString();
    }
    if (errorMsg) {
        errorMsg->clear();
    }
    return res.formatted;
}

QString DeveloperToolsEngine::minifyCss(const QString &css, QString *errorMsg) {
    const auto res = CodeCompressPretty::compressCss(css);
    if (!res.success) {
        if (errorMsg) {
            *errorMsg = res.errorMessage;
        }
        return QString();
    }
    if (errorMsg) {
        errorMsg->clear();
    }
    return res.formatted;
}

QString DeveloperToolsEngine::formatJs(const QString &js, QString *errorMsg) {
    const auto res = CodeCompressPretty::prettifyJs(js);
    if (!res.success) {
        if (errorMsg) {
            *errorMsg = res.errorMessage;
        }
        return QString();
    }
    if (errorMsg) {
        errorMsg->clear();
    }
    return res.formatted;
}

QString DeveloperToolsEngine::minifyJs(const QString &js, QString *errorMsg) {
    const auto res = CodeCompressPretty::compressJs(js);
    if (!res.success) {
        if (errorMsg) {
            *errorMsg = res.errorMessage;
        }
        return QString();
    }
    if (errorMsg) {
        errorMsg->clear();
    }
    return res.formatted;
}

QString DeveloperToolsEngine::encryptHtml(const QString &rawHtml, HtmlEncryptMethod method, const QString &password,
                                          QString *errorMsg) {
    const QString trimmed = rawHtml.trimmed();
    if (trimmed.isEmpty()) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("Empty HTML content");
        }
        return QString();
    }

    if (errorMsg) {
        errorMsg->clear();
    }

    switch (method) {
        case HtmlEncryptMethod::JavascriptEscape: {
            const QByteArray utf8 = trimmed.toUtf8();
            QString escaped;
            escaped.reserve(utf8.size() * 3);
            for (const char c : utf8) {
                escaped.append(QLatin1Char('%'));
                escaped.append(
                    QStringLiteral("%1").arg(static_cast<unsigned char>(c), 2, 16, QLatin1Char('0')).toUpper());
            }

            QString tmpl = QStringLiteral(
                "<!-- Encrypted HTML generated by NotepadNext -->\n"
                "<script type=\"text/javascript\">\n"
                "/*<![CDATA[*/\n"
                "document.write(unescape('__PAYLOAD__'));\n"
                "/*]]>*/\n"
                "</script>\n");
            tmpl.replace(QStringLiteral("__PAYLOAD__"), escaped);
            return tmpl;
        }

        case HtmlEncryptMethod::Base64: {
            const QString b64 = QString::fromLatin1(trimmed.toUtf8().toBase64());
            QString tmpl = QStringLiteral(
                "<!-- Encrypted HTML generated by NotepadNext -->\n"
                "<script type=\"text/javascript\">\n"
                "/*<![CDATA[*/\n"
                "document.write(decodeURIComponent(escape(window.atob('__PAYLOAD__'))));\n"
                "/*]]>*/\n"
                "</script>\n");
            tmpl.replace(QStringLiteral("__PAYLOAD__"), b64);
            return tmpl;
        }

        case HtmlEncryptMethod::PasswordProtected: {
            const QString pass = password.trimmed();
            if (pass.isEmpty()) {
                if (errorMsg) {
                    *errorMsg = QStringLiteral("Password is required for password-protected HTML encryption");
                }
                return QString();
            }

            // OpenSSL PBKDF2 (100,000 iter, SHA-256) + AES-256-GCM
            unsigned char salt[16];
            unsigned char nonce[12];
            if (RAND_bytes(salt, sizeof(salt)) != 1 || RAND_bytes(nonce, sizeof(nonce)) != 1) {
                if (errorMsg) {
                    *errorMsg = QStringLiteral("CSPRNG entropy failure generating salt/nonce");
                }
                return QString();
            }

            unsigned char key[32];
            const QByteArray passUtf8 = pass.toUtf8();
            if (PKCS5_PBKDF2_HMAC(passUtf8.constData(), static_cast<int>(passUtf8.size()), salt, sizeof(salt), 100000,
                                  EVP_sha256(), sizeof(key), key) != 1) {
                if (errorMsg) {
                    *errorMsg = QStringLiteral("Key derivation failure (PBKDF2)");
                }
                return QString();
            }

            const QByteArray plainUtf8 = trimmed.toUtf8();
            QByteArray cipherBytes(plainUtf8.size() + 16, Qt::Uninitialized);
            unsigned char tag[16];

            EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
            if (!ctx) {
                if (errorMsg) {
                    *errorMsg = QStringLiteral("EVP_CIPHER_CTX_new failure");
                }
                return QString();
            }

            int outLen1 = 0;
            int outLen2 = 0;
            bool ok = true;
            if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1 ||
                EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, sizeof(nonce), nullptr) != 1 ||
                EVP_EncryptInit_ex(ctx, nullptr, nullptr, key, nonce) != 1 ||
                EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(cipherBytes.data()), &outLen1,
                                  reinterpret_cast<const unsigned char *>(plainUtf8.constData()),
                                  static_cast<int>(plainUtf8.size())) != 1 ||
                EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(cipherBytes.data()) + outLen1, &outLen2) !=
                    1 ||
                EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, sizeof(tag), tag) != 1) {
                ok = false;
            }

            EVP_CIPHER_CTX_free(ctx);
            OPENSSL_cleanse(key, sizeof(key));

            if (!ok) {
                if (errorMsg) {
                    *errorMsg = QStringLiteral("AES-256-GCM encryption computation error");
                }
                return QString();
            }

            cipherBytes.resize(outLen1 + outLen2);

            QByteArray container;
            container.append(reinterpret_cast<const char *>(salt), sizeof(salt));
            container.append(reinterpret_cast<const char *>(nonce), sizeof(nonce));
            container.append(cipherBytes);
            container.append(reinterpret_cast<const char *>(tag), sizeof(tag));

            const QString encryptedPayload = QStringLiteral("SPKENC01:") + QString::fromLatin1(container.toBase64());

            QString tmpl = QString::fromUtf8(R"SPK_HTML(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Protected Document</title>
  <style>
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; display: flex; align-items: center; justify-content: center; min-height: 100vh; margin: 0; background: #0f141c; color: #e6edf3; }
    .box { background: #161b22; padding: 28px; border-radius: 12px; border: 1px solid #30363d; width: 100%; max-width: 380px; box-shadow: 0 8px 24px rgba(0,0,0,0.4); text-align: center; }
    input { width: 100%; padding: 10px 12px; margin: 16px 0; border: 1px solid #30363d; background: #0d1117; color: #fff; border-radius: 6px; box-sizing: border-box; font-size: 14px; }
    button { width: 100%; padding: 10px; background: #238636; color: #fff; border: none; border-radius: 6px; font-weight: 600; cursor: pointer; font-size: 14px; }
    button:hover { background: #2ea043; }
    .err { color: #f85149; font-size: 13px; margin-top: 10px; display: none; }
  </style>
</head>
<body>
  <div id="authBox" class="box">
    <h2>Protected Document</h2>
    <p style="font-size: 13px; color: #8b949e;">This HTML file is encrypted. Enter the passphrase to view contents.</p>
    <input type="password" id="pwd" placeholder="Enter passphrase..." autofocus onkeydown="if(event.key==='Enter')decrypt();" />
    <button onclick="decrypt()">Unlock &amp; View</button>
    <div id="err" class="err">Invalid passphrase or corrupted payload</div>
  </div>
  <script>
    const payload = "__PAYLOAD__";
    async function decrypt() {
      const p = document.getElementById("pwd").value;
      const err = document.getElementById("err");
      try {
        const raw = atob(payload.replace(/^(SPKENC01|NPMENC01):/, ''));
        const bytes = new Uint8Array(raw.length);
        for (let i = 0; i < raw.length; i++) bytes[i] = raw.charCodeAt(i);
        const salt = bytes.slice(0, 16);
        const nonce = bytes.slice(16, 28);
        const tag = bytes.slice(bytes.length - 16);
        const cipher = bytes.slice(28, bytes.length - 16);
        const enc = new TextEncoder();
        const keyMat = await crypto.subtle.importKey("raw", enc.encode(p), {name: "PBKDF2"}, false, ["deriveKey"]);
        const key = await crypto.subtle.deriveKey({name: "PBKDF2", salt: salt, iterations: 100000, hash: "SHA-256"}, keyMat, {name: "AES-GCM", length: 256}, false, ["decrypt"]);
        const combined = new Uint8Array(cipher.length + tag.length);
        combined.set(cipher);
        combined.set(tag, cipher.length);
        const dec = await crypto.subtle.decrypt({name: "AES-GCM", iv: nonce}, key, combined);
        document.open();
        document.write(new TextDecoder().decode(dec));
        document.close();
      } catch(e) {
        err.style.display = "block";
      }
    }
  </script>
</body>
</html>)SPK_HTML");
            tmpl.replace(QStringLiteral("__PAYLOAD__"), encryptedPayload);
            return tmpl;
        }
    }

    return QString();
}

QString DeveloperToolsEngine::decryptHtml(const QString &input, QString *errorMsg) {
    const QString trimmed = input.trimmed();
    if (trimmed.isEmpty()) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("Empty HTML content to decrypt");
        }
        return QString();
    }

    // Match unescape('...')
    static const QRegularExpression rxUnescape(QStringLiteral(R"(unescape\(['"]([^'"]+)['"]\))"));
    const auto matchUnescape = rxUnescape.match(trimmed);
    if (matchUnescape.hasMatch()) {
        if (errorMsg) {
            errorMsg->clear();
        }
        return QUrl::fromPercentEncoding(matchUnescape.captured(1).toUtf8());
    }

    // Match atob('...')
    static const QRegularExpression rxAtob(QStringLiteral(R"(atob\(['"]([^'"]+)['"]\))"));
    const auto matchAtob = rxAtob.match(trimmed);
    if (matchAtob.hasMatch()) {
        if (errorMsg) {
            errorMsg->clear();
        }
        return MimeTools::base64Decode(matchAtob.captured(1));
    }

    // Match direct percent-encoded string without tags
    if (trimmed.contains(QLatin1Char('%')) && !trimmed.contains(QLatin1Char('<'))) {
        if (errorMsg) {
            errorMsg->clear();
        }
        return QUrl::fromPercentEncoding(trimmed.toUtf8());
    }

    if (errorMsg) {
        *errorMsg =
            QStringLiteral("Unable to recognize encrypted HTML format. Supported formats: JS unescape and Base64.");
    }
    return QString();
}

JwtInfo DeveloperToolsEngine::parseJwt(const QString &jwtToken) {
    JwtInfo info;
    const QString trimmed = jwtToken.trimmed();
    if (trimmed.isEmpty()) {
        info.error = QStringLiteral("JWT token is empty");
        return info;
    }

    const QStringList parts = trimmed.split(QLatin1Char('.'));
    if (parts.size() != 3) {
        info.error = QStringLiteral(
            "Invalid JWT format: Token must have exactly 3 parts separated by dots (header.payload.signature)");
        return info;
    }

    // Header
    const QByteArray headerBytes = QByteArray::fromBase64(parts.at(0).toLatin1(), QByteArray::Base64UrlEncoding);
    QJsonParseError headerErr;
    const QJsonDocument headerDoc = QJsonDocument::fromJson(headerBytes, &headerErr);
    if (headerErr.error != QJsonParseError::NoError) {
        info.error = QStringLiteral("JWT Header parsing error: %1").arg(headerErr.errorString());
        return info;
    }
    info.headerJson = QString::fromUtf8(headerDoc.toJson(QJsonDocument::Indented));

    // Payload
    const QByteArray payloadBytes = QByteArray::fromBase64(parts.at(1).toLatin1(), QByteArray::Base64UrlEncoding);
    QJsonParseError payloadErr;
    const QJsonDocument payloadDoc = QJsonDocument::fromJson(payloadBytes, &payloadErr);
    if (payloadErr.error != QJsonParseError::NoError) {
        info.error = QStringLiteral("JWT Payload parsing error: %1").arg(payloadErr.errorString());
        return info;
    }
    info.payloadJson = QString::fromUtf8(payloadDoc.toJson(QJsonDocument::Indented));
    info.signature = parts.at(2);

    // Extract exp and iat
    if (payloadDoc.isObject()) {
        const QJsonObject obj = payloadDoc.object();
        if (obj.contains(QStringLiteral("exp"))) {
            const qint64 expSec = obj.value(QStringLiteral("exp")).toVariant().toLongLong();
            if (expSec > 0) {
                info.expirationDate = QDateTime::fromSecsSinceEpoch(expSec, QTimeZone::utc());
            }
        }
        if (obj.contains(QStringLiteral("iat"))) {
            const qint64 iatSec = obj.value(QStringLiteral("iat")).toVariant().toLongLong();
            if (iatSec > 0) {
                info.issuedAtDate = QDateTime::fromSecsSinceEpoch(iatSec, QTimeZone::utc());
            }
        }
    }

    info.isValid = true;
    return info;
}

QString DeveloperToolsEngine::urlEncode(const QString &text) {
    return QString::fromUtf8(QUrl::toPercentEncoding(text));
}

QString DeveloperToolsEngine::urlDecode(const QString &text) {
    return QUrl::fromPercentEncoding(text.toUtf8());
}

QString DeveloperToolsEngine::unicodeEscape(const QString &text) {
    QString result;
    result.reserve(text.length() * 2);

    for (qsizetype i = 0; i < text.length(); ++i) {
        const QChar ch = text.at(i);
        if (ch.isHighSurrogate() && i + 1 < text.length() && text.at(i + 1).isLowSurrogate()) {
            const QChar low = text.at(++i);
            result += QStringLiteral("\\u%1\\u%2")
                          .arg(static_cast<ushort>(ch.unicode()), 4, 16, QLatin1Char('0'))
                          .arg(static_cast<ushort>(low.unicode()), 4, 16, QLatin1Char('0'));
        } else if (ch.unicode() >= 0x20 && ch.unicode() <= 0x7E && ch.unicode() != '\\') {
            result += ch;
        } else {
            result += QStringLiteral("\\u%1").arg(static_cast<ushort>(ch.unicode()), 4, 16, QLatin1Char('0'));
        }
    }

    return result;
}

QString DeveloperToolsEngine::unicodeUnescape(const QString &text) {
    QString result;
    result.reserve(text.length());

    for (qsizetype i = 0; i < text.length(); ++i) {
        if (text.at(i) == QLatin1Char('\\') && i + 1 < text.length()) {
            const QChar next = text.at(i + 1);
            if (next == QLatin1Char('u') && i + 5 < text.length()) {
                bool ok = false;
                const ushort code = text.mid(i + 2, 4).toUShort(&ok, 16);
                if (ok) {
                    i += 5;
                    const QChar ch(code);
                    if (ch.isHighSurrogate() && i + 6 < text.length() && text.mid(i + 1, 2) == QStringLiteral("\\u")) {
                        bool okLow = false;
                        const ushort lowCode = text.mid(i + 3, 4).toUShort(&okLow, 16);
                        if (okLow) {
                            i += 6;
                            result += ch;
                            result += QChar(lowCode);
                            continue;
                        }
                    }
                    result += ch;
                    continue;
                }
            } else if (next == QLatin1Char('\\')) {
                result += QLatin1Char('\\');
                ++i;
                continue;
            } else if (next == QLatin1Char('n')) {
                result += QLatin1Char('\n');
                ++i;
                continue;
            } else if (next == QLatin1Char('r')) {
                result += QLatin1Char('\r');
                ++i;
                continue;
            } else if (next == QLatin1Char('t')) {
                result += QLatin1Char('\t');
                ++i;
                continue;
            }
        }
        result += text.at(i);
    }

    return result;
}

QString DeveloperToolsEngine::htmlEntityEncode(const QString &text) {
    QString encoded;
    encoded.reserve(text.length() * 2);
    for (const QChar c : text) {
        if (c == QLatin1Char('&')) {
            encoded += QStringLiteral("&amp;");
        } else if (c == QLatin1Char('<')) {
            encoded += QStringLiteral("&lt;");
        } else if (c == QLatin1Char('>')) {
            encoded += QStringLiteral("&gt;");
        } else if (c == QLatin1Char('"')) {
            encoded += QStringLiteral("&quot;");
        } else if (c == QLatin1Char('\'')) {
            encoded += QStringLiteral("&#39;");
        } else {
            encoded += c;
        }
    }
    return encoded;
}

QString DeveloperToolsEngine::htmlEntityDecode(const QString &text) {
    QString decoded = text;
    decoded.replace(QStringLiteral("&amp;"), QStringLiteral("&"));
    decoded.replace(QStringLiteral("&lt;"), QStringLiteral("<"));
    decoded.replace(QStringLiteral("&gt;"), QStringLiteral(">"));
    decoded.replace(QStringLiteral("&quot;"), QStringLiteral("\""));
    decoded.replace(QStringLiteral("&apos;"), QStringLiteral("'"));
    decoded.replace(QStringLiteral("&#39;"), QStringLiteral("'"));
    decoded.replace(QStringLiteral("&nbsp;"), QStringLiteral(" "));

    // Numeric entity decode: &#NNN; and &#xHHH;
    static const QRegularExpression decEntity(QStringLiteral(R"(&#([0-9]{1,7});)"));
    auto itDec = decEntity.globalMatch(decoded);
    while (itDec.hasNext()) {
        const auto m = itDec.next();
        bool ok = false;
        const uint cp = m.captured(1).toUInt(&ok, 10);
        if (ok && QChar::isPrint(cp)) {
            decoded.replace(m.captured(0), QString(QChar(cp)));
        }
    }

    static const QRegularExpression hexEntity(QStringLiteral(R"(&#x([0-9a-fA-F]{1,6});)"));
    auto itHex = hexEntity.globalMatch(decoded);
    while (itHex.hasNext()) {
        const auto m = itHex.next();
        bool ok = false;
        const uint cp = m.captured(1).toUInt(&ok, 16);
        if (ok && QChar::isPrint(cp)) {
            decoded.replace(m.captured(0), QString(QChar(cp)));
        }
    }

    return decoded;
}

QString DeveloperToolsEngine::base64EncodeLines(const QString &text) {
    return MimeTools::base64EncodeByLine(text);
}

QString DeveloperToolsEngine::base64DecodeLines(const QString &text) {
    return MimeTools::base64DecodeByLine(text);
}

QString DeveloperToolsEngine::quotedPrintableEncode(const QString &text) {
    return MimeTools::quotedPrintableEncode(text);
}

QString DeveloperToolsEngine::quotedPrintableDecode(const QString &text) {
    return MimeTools::quotedPrintableDecode(text);
}

QString DeveloperToolsEngine::asciiToHex(const QString &ascii, const QString &separator) {
    const QByteArray bytes = ascii.toUtf8();
    QStringList parts;
    parts.reserve(bytes.size());
    std::transform(bytes.cbegin(), bytes.cend(), std::back_inserter(parts), [](char b) {
        return QStringLiteral("%1").arg(static_cast<unsigned char>(b), 2, 16, QLatin1Char('0')).toUpper();
    });
    return parts.join(separator);
}

QString DeveloperToolsEngine::hexToAscii(const QString &hex, QString *errorMsg) {
    QString clean = hex;
    clean.remove(QLatin1Char(' '));
    clean.remove(QLatin1Char('\t'));
    clean.remove(QLatin1Char('\n'));
    clean.remove(QLatin1Char('\r'));
    clean.remove(QStringLiteral("0x"), Qt::CaseInsensitive);

    if (clean.length() % 2 != 0) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("Hex string length must be even (got %1 characters)").arg(clean.length());
        }
        return QString();
    }

    QByteArray bytes;
    bytes.reserve(clean.length() / 2);
    for (qsizetype i = 0; i < clean.length(); i += 2) {
        bool ok = false;
        const auto byte = static_cast<char>(clean.mid(i, 2).toUShort(&ok, 16));
        if (!ok) {
            if (errorMsg) {
                *errorMsg =
                    QStringLiteral("Invalid hex character sequence '%1' at offset %2").arg(clean.mid(i, 2)).arg(i);
            }
            return QString();
        }
        bytes.append(byte);
    }

    if (errorMsg) {
        errorMsg->clear();
    }
    return QString::fromUtf8(bytes);
}

QString DeveloperToolsEngine::convertRadix(const QString &input, int fromBase, int toBase, bool uppercase,
                                           QString *errorMsg) {
    QString clean = input.trimmed();
    if ((fromBase == 16 && clean.startsWith(QStringLiteral("0x"), Qt::CaseInsensitive)) ||
        (fromBase == 2 && clean.startsWith(QStringLiteral("0b"), Qt::CaseInsensitive)) ||
        (fromBase == 8 && clean.startsWith(QStringLiteral("0o"), Qt::CaseInsensitive))) {
        clean = clean.mid(2);
    }

    bool ok = false;
    const qulonglong value = clean.toULongLong(&ok, fromBase);
    if (!ok) {
        if (errorMsg) {
            *errorMsg = QStringLiteral("Cannot parse '%1' as base %2 number").arg(input).arg(fromBase);
        }
        return QString();
    }

    if (errorMsg) {
        errorMsg->clear();
    }
    const QString formatted = QString::number(value, toBase);
    return uppercase ? formatted.toUpper() : formatted.toLower();
}

QString DeveloperToolsEngine::toTitleCase(const QString &text) {
    QString result;
    result.reserve(text.length());
    bool startOfWord = true;

    for (const QChar ch : text) {
        if (ch.isSpace() || ch.isPunct()) {
            startOfWord = true;
            result += ch;
        } else if (startOfWord) {
            result += ch.toUpper();
            startOfWord = false;
        } else {
            result += ch.toLower();
        }
    }
    return result;
}

QString DeveloperToolsEngine::toInvertCase(const QString &text) {
    QString result;
    result.reserve(text.length());
    for (const QChar ch : text) {
        if (ch.isUpper()) {
            result += ch.toLower();
        } else if (ch.isLower()) {
            result += ch.toUpper();
        } else {
            result += ch;
        }
    }
    return result;
}

QString DeveloperToolsEngine::toSentenceCase(const QString &text) {
    QString result;
    result.reserve(text.length());
    bool capitalizeNext = true;

    for (const QChar ch : text) {
        if (ch.isLetter()) {
            if (capitalizeNext) {
                result += ch.toUpper();
                capitalizeNext = false;
            } else {
                result += ch.toLower();
            }
        } else {
            result += ch;
            if (ch == QLatin1Char('.') || ch == QLatin1Char('!') || ch == QLatin1Char('?')) {
                capitalizeNext = true;
            }
        }
    }
    return result;
}

PasswordStrength DeveloperToolsEngine::evaluatePasswordStrength(const QString &password) {
    PasswordStrength res;
    const int len = static_cast<int>(password.length());
    if (len == 0) {
        res.score = 0;
        res.entropy = 0.0;
        res.label = QStringLiteral("Empty");
        res.isStrong = false;
        res.feedback = QStringLiteral("Enter a password to evaluate.");
        return res;
    }

    bool hasLower = false;
    bool hasUpper = false;
    bool hasDigit = false;
    bool hasSymbol = false;

    for (const QChar c : password) {
        if (c.isLower()) {
            hasLower = true;
        } else if (c.isUpper()) {
            hasUpper = true;
        } else if (c.isDigit()) {
            hasDigit = true;
        } else {
            hasSymbol = true;
        }
    }

    int pool = 0;
    if (hasLower) {
        pool += 26;
    }
    if (hasUpper) {
        pool += 26;
    }
    if (hasDigit) {
        pool += 10;
    }
    if (hasSymbol) {
        pool += 32;
    }

    const double entropy = len * (pool > 0 ? std::log2(static_cast<double>(pool)) : 0.0);
    res.entropy = std::round(entropy * 10.0) / 10.0;

    if (len < 6 || entropy < 25.0) {
        res.score = 0;
        res.label = QStringLiteral("Very Weak");
        res.isStrong = false;
        res.feedback =
            QStringLiteral("Password is too short or predictable. Use at least 12 characters with mixed types.");
    } else if (entropy < 45.0) {
        res.score = 1;
        res.label = QStringLiteral("Weak");
        res.isStrong = false;
        res.feedback = QStringLiteral("Add uppercase letters, numbers, and symbols to increase complexity.");
    } else if (entropy < 65.0) {
        res.score = 2;
        res.label = QStringLiteral("Medium");
        res.isStrong = false;
        res.feedback = QStringLiteral("Decent password, but could be longer or include more special characters.");
    } else if (entropy < 85.0) {
        res.score = 3;
        res.label = QStringLiteral("Strong");
        res.isStrong = true;
        res.feedback = QStringLiteral("Great security profile. Sufficient against brute force attacks.");
    } else {
        res.score = 4;
        res.label = QStringLiteral("Very Strong");
        res.isStrong = true;
        res.feedback = QStringLiteral("Excellent enterprise-grade password entropy.");
    }

    return res;
}

QString DeveloperToolsEngine::generateUuidV4(bool uppercase, bool withBraces, bool withDashes) {
    const QUuid uuid = QUuid::createUuid();
    QString str = uuid.toString(withBraces ? QUuid::WithBraces : QUuid::WithoutBraces);
    if (!withDashes) {
        str.remove(QLatin1Char('-'));
    }
    return uppercase ? str.toUpper() : str.toLower();
}

QString DeveloperToolsEngine::epochToIso(qint64 epochSeconds) {
    const QDateTime dt = QDateTime::fromSecsSinceEpoch(epochSeconds, QTimeZone::utc());
    return dt.toString(Qt::ISODate);
}

qint64 DeveloperToolsEngine::isoToEpoch(const QString &isoString, bool *ok) {
    QDateTime dt = QDateTime::fromString(isoString, Qt::ISODate);
    if (!dt.isValid()) {
        dt = QDateTime::fromString(isoString, Qt::ISODateWithMs);
    }
    if (ok) {
        *ok = dt.isValid();
    }
    return dt.isValid() ? dt.toSecsSinceEpoch() : 0;
}
