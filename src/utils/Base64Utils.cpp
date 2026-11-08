/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUSTION | https://www.alsyundawy.com
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Base64Utils.h"
#include "ScintillaNext.h"
#include "UndoAction.h"

Base64Utils::Result Base64Utils::encode(ScintillaNext *editor) {
    if (!editor) {
        return {false, false, QStringLiteral("No active editor")};
    }

    if (editor->selectionEmpty()) {
        const auto docLength = editor->length();
        if (docLength == 0) {
            return {false, true, QStringLiteral("Document is empty")};
        }

        const QByteArray raw = editor->get_text_range(0, static_cast<int>(docLength));
        const QByteArray encoded = raw.toBase64();

        const UndoAction ua(editor);
        editor->setText(encoded.constData());

        return {true, true, QString()};
    }

    const QByteArray selText = editor->getSelText();
    const QByteArray encoded = selText.toBase64();

    const UndoAction ua(editor);
    editor->replaceSel(encoded.constData());

    return {true, false, QString()};
}

Base64Utils::Result Base64Utils::decode(ScintillaNext *editor) {
    if (!editor) {
        return {false, false, QStringLiteral("No active editor")};
    }

    if (editor->selectionEmpty()) {
        const auto docLength = editor->length();
        if (docLength == 0) {
            return {false, true, QStringLiteral("Document is empty")};
        }

        const QByteArray raw = editor->get_text_range(0, static_cast<int>(docLength));
        const auto decodedResult = QByteArray::fromBase64Encoding(raw.trimmed(), QByteArray::AbortOnBase64DecodingErrors);
        if (!decodedResult) {
            return {false, true, QStringLiteral("Invalid Base64 data")};
        }

        const UndoAction ua(editor);
        editor->setText(decodedResult.decoded.constData());

        return {true, true, QString()};
    }

    const QByteArray selText = editor->getSelText();
    const auto decodedResult = QByteArray::fromBase64Encoding(selText.trimmed(), QByteArray::AbortOnBase64DecodingErrors);
    if (!decodedResult) {
        return {false, false, QStringLiteral("Invalid Base64 data")};
    }

    const UndoAction ua(editor);
    editor->replaceSel(decodedResult.decoded.constData());

    return {true, false, QString()};
}

QString Base64Utils::encode(const QString &plainText) {
    return QString::fromLatin1(plainText.toUtf8().toBase64());
}

QString Base64Utils::decode(const QString &base64Text, bool *ok) {
    if (const auto decodedResult = QByteArray::fromBase64Encoding(base64Text.toUtf8().trimmed(),
                                                                  QByteArray::AbortOnBase64DecodingErrors);
        decodedResult) {
        if (ok) {
            *ok = true;
        }
        return QString::fromUtf8(decodedResult.decoded);
    }
    if (ok) {
        *ok = false;
    }
    return {};
}

QString Base64Utils::encodeUrlSafe(const QString &plainText) {
    return QString::fromLatin1(plainText.toUtf8().toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

QString Base64Utils::decodeUrlSafe(const QString &base64Text, bool *ok) {
    if (const auto decodedResult = QByteArray::fromBase64Encoding(base64Text.toUtf8().trimmed(),
                                                                  QByteArray::Base64UrlEncoding | QByteArray::AbortOnBase64DecodingErrors);
        decodedResult) {
        if (ok) {
            *ok = true;
        }
        return QString::fromUtf8(decodedResult.decoded);
    }
    if (ok) {
        *ok = false;
    }
    return {};
}
