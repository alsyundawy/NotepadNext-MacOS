/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "GenerateHashDigestDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QClipboard>
#include <QApplication>
#include <QFontDatabase>
#include <QCryptographicHash>
#include <QTimer>

#include <array>
#include <cstring>
#include <numeric>
#include <optional>

namespace {

quint32 computeCrc32(const QByteArray &data) {
    static constexpr auto table = []() constexpr {
        std::array<quint32, 256> t{};
        for (quint32 i = 0; i < 256; ++i) {
            quint32 c = i;
            for (int j = 0; j < 8; ++j) {
                c = (c & 1) ? (0xEDB88320U ^ (c >> 1)) : (c >> 1);
            }
            t[i] = c;
        }
        return t;
    }();

    const quint32 crc = std::accumulate(data.cbegin(), data.cend(), 0xFFFFFFFFU, [](quint32 currentCrc, char b) {
        return table[(currentCrc ^ static_cast<quint8>(b)) & 0xFF] ^ (currentCrc >> 8);
    });
    return crc ^ 0xFFFFFFFFU;
}

quint64 computeXxHash64(const QByteArray &data) {
    static constexpr quint64 PRIME64_1 = 11400714785074694791ULL;
    static constexpr quint64 PRIME64_2 = 14029467366897019727ULL;
    static constexpr quint64 PRIME64_3 = 1609587929392839161ULL;
    static constexpr quint64 PRIME64_4 = 9650029242287828579ULL;
    static constexpr quint64 PRIME64_5 = 2870177450012600261ULL;

    const auto *p = reinterpret_cast<const quint8 *>(data.constData());
    const qsizetype len = data.size();
    quint64 h64 = 0;

    auto roundFunc = [](quint64 acc, quint64 input) {
        acc += input * PRIME64_2;
        acc = (acc << 31) | (acc >> (64 - 31));
        return acc * PRIME64_1;
    };

    if (len >= 32) {
        quint64 v1 = PRIME64_1 + PRIME64_2;
        quint64 v2 = PRIME64_2;
        quint64 v3 = 0;
        quint64 v4 = 0 - PRIME64_1;

        const auto *limit = p + len - 32;
        do {
            auto read64 = [](const quint8 *ptr) {
                quint64 val = 0;
                std::memcpy(&val, ptr, sizeof(val));
                return val;
            };

            v1 = roundFunc(v1, read64(p));
            p += 8;
            v2 = roundFunc(v2, read64(p));
            p += 8;
            v3 = roundFunc(v3, read64(p));
            p += 8;
            v4 = roundFunc(v4, read64(p));
            p += 8;
        } while (p <= limit);

        auto rotl = [](quint64 x, int r) { return (x << r) | (x >> (64 - r)); };
        h64 = rotl(v1, 1) + rotl(v2, 7) + rotl(v3, 12) + rotl(v4, 18);

        auto mergeRound = [&](quint64 acc, quint64 val) {
            val = roundFunc(0, val);
            acc ^= val;
            return acc * PRIME64_1 + PRIME64_4;
        };
        h64 = mergeRound(h64, v1);
        h64 = mergeRound(h64, v2);
        h64 = mergeRound(h64, v3);
        h64 = mergeRound(h64, v4);
    } else {
        h64 = PRIME64_5;
    }

    h64 += static_cast<quint64>(len);

    while (len - (p - reinterpret_cast<const quint8 *>(data.constData())) >= 8) {
        quint64 k1 = 0;
        std::memcpy(&k1, p, sizeof(k1));
        k1 = roundFunc(0, k1);
        h64 ^= k1;
        h64 = ((h64 << 27) | (h64 >> (64 - 27))) * PRIME64_1 + PRIME64_4;
        p += 8;
    }

    if (len - (p - reinterpret_cast<const quint8 *>(data.constData())) >= 4) {
        quint32 k1 = 0;
        std::memcpy(&k1, p, sizeof(k1));
        h64 ^= static_cast<quint64>(k1) * PRIME64_1;
        h64 = ((h64 << 23) | (h64 >> (64 - 23))) * PRIME64_2 + PRIME64_3;
        p += 4;
    }

    while (p < reinterpret_cast<const quint8 *>(data.constData()) + len) {
        h64 ^= static_cast<quint64>(*p) * PRIME64_5;
        h64 = ((h64 << 11) | (h64 >> (64 - 11))) * PRIME64_1;
        ++p;
    }

    h64 ^= h64 >> 33;
    h64 *= PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= PRIME64_3;
    h64 ^= h64 >> 32;

    return h64;
}

std::optional<QCryptographicHash::Algorithm> resolveCryptoAlgorithm(const QString &algStr) {
    if (algStr == QLatin1String("MD5")) {
        return QCryptographicHash::Md5;
    }
    if (algStr == QLatin1String("SHA1") || algStr == QLatin1String("SHA-1")) {
        return QCryptographicHash::Sha1;
    }
    if (algStr == QLatin1String("SHA256") || algStr == QLatin1String("SHA-256")) {
        return QCryptographicHash::Sha256;
    }
    if (algStr == QLatin1String("SHA384") || algStr == QLatin1String("SHA-384")) {
        return QCryptographicHash::Sha384;
    }
    if (algStr == QLatin1String("SHA512") || algStr == QLatin1String("SHA-512")) {
        return QCryptographicHash::Sha512;
    }
    if (algStr == QLatin1String("SHA3-1") || algStr == QLatin1String("SHA3_224") ||
        algStr == QLatin1String("SHA3-224") || algStr == QLatin1String("SHA3-1 (SHA3-224)")) {
        return QCryptographicHash::Sha3_224;
    }
    if (algStr == QLatin1String("SHA3-256") || algStr == QLatin1String("SHA3_256")) {
        return QCryptographicHash::Sha3_256;
    }
    if (algStr == QLatin1String("SHA3-384") || algStr == QLatin1String("SHA3_384")) {
        return QCryptographicHash::Sha3_384;
    }
    if (algStr == QLatin1String("SHA3-512") || algStr == QLatin1String("SHA3_512")) {
        return QCryptographicHash::Sha3_512;
    }
    return std::nullopt;
}

}  // namespace

QString GenerateHashDigestDialog::computeSingleDigest(const QString &algorithmName, const QByteArray &data) {
    if (algorithmName == QLatin1String("CRC32")) {
        return QStringLiteral("%1").arg(computeCrc32(data), 8, 16, QLatin1Char('0'));
    }
    if (algorithmName == QLatin1String("XXHASH")) {
        return QStringLiteral("%1").arg(computeXxHash64(data), 16, 16, QLatin1Char('0'));
    }
    if (const auto algo = resolveCryptoAlgorithm(algorithmName)) {
        return QString::fromLatin1(QCryptographicHash::hash(data, *algo).toHex());
    }
    return {};
}

GenerateHashDigestDialog::GenerateHashDigestDialog(const QString &algorithmName, const QString &initialText,
                                                   QWidget *parent)
    : QDialog(parent), m_algorithmName(algorithmName) {
    setupUi();

    if (!initialText.isEmpty()) {
        m_textInput->setPlainText(initialText);
    }
    updateDigest();
}

void GenerateHashDigestDialog::setupUi() {
    setWindowTitle(tr("Generate %1 digest").arg(m_algorithmName));
    setMinimumWidth(540);
    setMinimumHeight(380);
    resize(580, 420);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(10);

    m_chkPerLine = new QCheckBox(tr("Treat each line as a &separate string"), this);
    mainLayout->addWidget(m_chkPerLine);

    const QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    m_textInput = new QPlainTextEdit(this);
    m_textInput->setPlaceholderText(tr("Enter text or paste here to generate digest..."));
    m_textInput->setFont(monoFont);
    mainLayout->addWidget(m_textInput, 1);

    m_textOutput = new QPlainTextEdit(this);
    m_textOutput->setReadOnly(true);
    m_textOutput->setFont(monoFont);
    m_textOutput->setPlaceholderText(tr("Computed hash digest will appear here..."));
    mainLayout->addWidget(m_textOutput, 1);

    auto *copyRow = new QHBoxLayout();
    copyRow->addStretch();
    m_btnCopy = new QPushButton(tr("&Copy to Clipboard"), this);
    copyRow->addWidget(m_btnCopy);
    mainLayout->addLayout(copyRow);

    auto *closeRow = new QHBoxLayout();
    closeRow->addStretch();
    m_btnClose = new QPushButton(tr("&Close"), this);
    m_btnClose->setMinimumWidth(110);
    m_btnClose->setDefault(true);
    closeRow->addWidget(m_btnClose);
    closeRow->addStretch();
    mainLayout->addLayout(closeRow);

    connect(m_textInput, &QPlainTextEdit::textChanged, this, &GenerateHashDigestDialog::onInputChanged);
    connect(m_chkPerLine, &QCheckBox::toggled, this, &GenerateHashDigestDialog::onInputChanged);
    connect(m_btnCopy, &QPushButton::clicked, this, &GenerateHashDigestDialog::onCopyClicked);
    connect(m_btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

void GenerateHashDigestDialog::onInputChanged() {
    updateDigest();
}

void GenerateHashDigestDialog::updateDigest() {
    const QString text = m_textInput->toPlainText();

    if (!m_chkPerLine->isChecked()) {
        const QByteArray data = text.toUtf8();
        const QString digest = computeSingleDigest(m_algorithmName, data);
        m_textOutput->setPlainText(digest);
        return;
    }

    const QStringList lines = text.split(QLatin1Char('\n'));
    QStringList resultList;
    resultList.reserve(lines.size());

    for (QString line : lines) {
        if (line.endsWith(QLatin1Char('\r'))) {
            line.chop(1);
        }
        resultList.append(computeSingleDigest(m_algorithmName, line.toUtf8()));
    }

    m_textOutput->setPlainText(resultList.join(QLatin1Char('\n')));
}

void GenerateHashDigestDialog::onCopyClicked() {
    const QString digest = m_textOutput->toPlainText();
    if (digest.isEmpty()) {
        return;
    }

    QApplication::clipboard()->setText(digest);

    const QString oldText = m_btnCopy->text();
    m_btnCopy->setText(tr("✓ Copied!"));
    m_btnCopy->setEnabled(false);

    QTimer::singleShot(1500, this, [this, oldText]() {
        if (m_btnCopy) {
            m_btnCopy->setText(oldText);
            m_btnCopy->setEnabled(true);
        }
    });
}
