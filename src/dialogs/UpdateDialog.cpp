/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry DS Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
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

#include "UpdateDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>
#include <QApplication>
#include <QKeyEvent>
#include <QEvent>
#include <QPalette>
#include <QPixmap>
#include <QCryptographicHash>
#include <QFile>

bool UpdateDialog::verifyPackageHash(const QString &filePath, const QString &expectedSha256)
{
    if (expectedSha256.trimmed().isEmpty()) {
        return true;
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QCryptographicHash hash(QCryptographicHash::Sha256);
    if (!hash.addData(&file)) {
        return false;
    }
    return hash.result().toHex().compare(expectedSha256.trimmed().toLatin1(), Qt::CaseInsensitive) == 0;
}

UpdateDialog::UpdateDialog(const UpdateManifest &manifest, QWidget *parent)
    : QDialog(parent)
    , m_updateAvailable(manifest.updateAvailable)
    , m_isError(manifest.latestVersion.trimmed().isEmpty())
    , m_latestVersion(manifest.latestVersion)
    , m_currentVersion(manifest.currentVersion.isEmpty() ? QStringLiteral("0.15-07") : manifest.currentVersion)
    , m_downloadUrl(manifest.downloadUrl)
    , m_changelogUrl(manifest.changelogUrl)
    , m_openUrl(manifest.openUrl.isEmpty() ? QStringLiteral("https://github.com/alsyundawy/NotepadNext-MacOS/releases") : manifest.openUrl)
    , m_expectedSha256(manifest.expectedSha256)
{
    // Enforce strict HTTPS URL pinning (CWE-494 / CVE-2025-15556)
    if (!m_downloadUrl.isEmpty() && !m_downloadUrl.startsWith(QLatin1String("https://"), Qt::CaseInsensitive)) {
        qWarning("Insecure non-HTTPS update download URL blocked: %s", qPrintable(m_downloadUrl));
        m_downloadUrl.clear();
    }
    setupUi();
    updateThemeColors();
}

UpdateDialog::UpdateDialog(bool updateAvailable,
                           const QString &latestVersion,
                           const QString &currentVersion,
                           const QString &downloadUrl,
                           const QString &changelogUrl,
                           const QString &openUrl,
                           QWidget *parent)
    : UpdateDialog(UpdateManifest{updateAvailable, latestVersion, currentVersion, downloadUrl, changelogUrl, openUrl, QString()}, parent)
{
}

void UpdateDialog::setupUi()
{
    setWindowTitle(tr("Software Update"));
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowModality(Qt::WindowModal);
    setMinimumWidth(500);
    setMaximumWidth(580);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 20);
    mainLayout->setSpacing(18);

    // Top Header: App Icon + Titles & Badge
    auto *topLayout = new QHBoxLayout(); // NOSONAR
    topLayout->setSpacing(16);
    topLayout->setAlignment(Qt::AlignTop);

    m_iconLabel = new QLabel(this);
    QPixmap iconPixmap(QStringLiteral(":/icons/NotepadNext.png"));
    if (iconPixmap.isNull()) {
        iconPixmap = windowIcon().pixmap(64, 64);
    }
    if (!iconPixmap.isNull()) {
        m_iconLabel->setPixmap(iconPixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    m_iconLabel->setFixedSize(64, 64);
    topLayout->addWidget(m_iconLabel);

    auto *headerTextLayout = new QVBoxLayout(); // NOSONAR
    headerTextLayout->setSpacing(4);

    auto *badgeRow = new QHBoxLayout(); // NOSONAR
    badgeRow->setSpacing(8);

    auto *appTitleLabel = new QLabel(QStringLiteral("Notepad Next"), this);
    QFont titleFont = appTitleLabel->font();
    titleFont.setPointSize(titleFont.pointSize() + 3);
    titleFont.setBold(true);
    appTitleLabel->setFont(titleFont);
    badgeRow->addWidget(appTitleLabel);

    m_badgeLabel = new QLabel(this);
    badgeRow->addWidget(m_badgeLabel);
    badgeRow->addStretch();
    headerTextLayout->addLayout(badgeRow);

    m_headingLabel = new QLabel(this);
    m_headingLabel->setWordWrap(true);
    QFont headingFont = m_headingLabel->font();
    headingFont.setPointSize(headingFont.pointSize() + 1);
    headingFont.setBold(true);
    m_headingLabel->setFont(headingFont);
    headerTextLayout->addWidget(m_headingLabel);

    m_subLabel = new QLabel(this);
    m_subLabel->setWordWrap(true);
    headerTextLayout->addWidget(m_subLabel);

    topLayout->addLayout(headerTextLayout, 1);
    mainLayout->addLayout(topLayout);

    // Architecture detection string
#if defined(Q_OS_MACOS)
#  if defined(__aarch64__) || defined(_M_ARM64)
    const auto archStr = QStringLiteral("macOS Apple Silicon (arm64)");
#  else
    const auto archStr = QStringLiteral("macOS Intel (x86_64)");
#  endif
#elif defined(Q_OS_WIN)
#  if defined(__aarch64__) || defined(_M_ARM64)
    const auto archStr = QStringLiteral("Windows (arm64)");
#  else
    const auto archStr = QStringLiteral("Windows (x64)");
#  endif
#else
    const QString archStr = QStringLiteral("Linux / Unix");
#endif

    // Metadata Card
    m_cardWidget = new QWidget(this);
    auto *cardLayout = new QFormLayout(m_cardWidget);
    cardLayout->setContentsMargins(16, 12, 16, 12);
    cardLayout->setHorizontalSpacing(16);
    cardLayout->setVerticalSpacing(8);
    cardLayout->setLabelAlignment(Qt::AlignLeft);

    auto addCardRow = [&](const QString &label, const QString &val) {
        auto *lblKey = new QLabel(label, m_cardWidget);
        lblKey->setStyleSheet(QStringLiteral("font-weight: 600; opacity: 0.85;"));
        auto *lblVal = new QLabel(val, m_cardWidget);
        lblVal->setTextInteractionFlags(Qt::TextSelectableByMouse);
        cardLayout->addRow(lblKey, lblVal);
    };

    if (m_updateAvailable) {
        m_badgeLabel->setText(tr("🚀 Update Available"));
        m_headingLabel->setText(tr("Notepad Next %1 is Available").arg(m_latestVersion));
        m_subLabel->setText(tr("A newer version is ready with improvements and fixes."));

        addCardRow(tr("Installed Version:"), QStringLiteral("v%1").arg(m_currentVersion));
        addCardRow(tr("Latest Version:"), QStringLiteral("v%1").arg(m_latestVersion));
        addCardRow(tr("Target Architecture:"), archStr);
        if (!m_expectedSha256.isEmpty()) {
            addCardRow(tr("SHA-256 Checksum:"), m_expectedSha256);
        }
        addCardRow(tr("Check Timestamp:"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm")));
    } else if (m_isError) {
        m_badgeLabel->setText(tr("ℹ Check Offline"));
        m_headingLabel->setText(tr("Unable to Reach Update Server"));
        m_subLabel->setText(tr("Could not verify the latest version definition. Check your connection or visit GitHub."));

        addCardRow(tr("Installed Version:"), QStringLiteral("v%1").arg(m_currentVersion));
        addCardRow(tr("System Architecture:"), archStr);
        addCardRow(tr("Status:"), tr("Network unreachable or server offline"));
    } else {
        m_badgeLabel->setText(tr("✓ Up to Date"));
        m_headingLabel->setText(tr("You're Running the Latest Release!"));
        m_subLabel->setText(tr("Notepad Next is completely up to date with full macOS optimization."));

        addCardRow(tr("Current Version:"), QStringLiteral("v%1").arg(m_currentVersion));
        addCardRow(tr("System Architecture:"), archStr);
        addCardRow(tr("Release Channel:"), tr("Official GitHub Stable (0.15-07)"));
        addCardRow(tr("Last Checked:"), QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm")));
    }

    mainLayout->addWidget(m_cardWidget);

    // If update available and changelog is present, show notes area
    if (m_updateAvailable && !m_changelogUrl.isEmpty()) {
        m_changelogBrowser = new QTextBrowser(this);
        m_changelogBrowser->setFixedHeight(100);
        m_changelogBrowser->setOpenExternalLinks(true);
        m_changelogBrowser->setHtml(tr("<p><strong>Release Highlights:</strong></p>"
                                       "<p>For complete changelog and download packages, visit: "
                                       "<a href=\"%1\">%1</a></p>").arg(m_changelogUrl));
        mainLayout->addWidget(m_changelogBrowser);
    }

    // Action Buttons
    auto *buttonLayout = new QHBoxLayout(); // NOSONAR
    buttonLayout->setSpacing(10);

    m_viewReleaseButton = new QPushButton(tr("Release Notes"), this);
    connect(m_viewReleaseButton, &QPushButton::clicked, this, [this]() {
        const QString targetUrl = !m_openUrl.isEmpty() ? m_openUrl : QStringLiteral("https://github.com/alsyundawy/NotepadNext-MacOS/releases");
        QDesktopServices::openUrl(QUrl(targetUrl));
    });
    buttonLayout->addWidget(m_viewReleaseButton);

    buttonLayout->addStretch();

    if (m_updateAvailable) {
        m_closeButton = new QPushButton(tr("Remind Me Later"), this);
        connect(m_closeButton, &QPushButton::clicked, this, &QDialog::reject);
        buttonLayout->addWidget(m_closeButton);

        m_primaryButton = new QPushButton(tr("Download Update"), this);
        m_primaryButton->setDefault(true);
        connect(m_primaryButton, &QPushButton::clicked, this, [this]() {
            const QString dl = !m_downloadUrl.isEmpty() ? m_downloadUrl : m_openUrl;
            QDesktopServices::openUrl(QUrl(dl));
            accept();
        });
        buttonLayout->addWidget(m_primaryButton);
    } else {
        m_primaryButton = new QPushButton(tr("OK"), this);
        m_primaryButton->setDefault(true);
        connect(m_primaryButton, &QPushButton::clicked, this, &QDialog::accept);
        buttonLayout->addWidget(m_primaryButton);
    }

    mainLayout->addLayout(buttonLayout);
}

void UpdateDialog::updateThemeColors()
{
    const bool isDark = palette().window().color().value() < 128;

    const QString cardBg = isDark ? QStringLiteral("rgba(255, 255, 255, 0.06)") : QStringLiteral("rgba(0, 0, 0, 0.03)");
    const QString cardBorder = isDark ? QStringLiteral("rgba(255, 255, 255, 0.12)") : QStringLiteral("rgba(0, 0, 0, 0.09)");

    m_cardWidget->setStyleSheet(QStringLiteral(
        "QWidget { background-color: %1; border: 1px solid %2; border-radius: 8px; }"
    ).arg(cardBg, cardBorder));

    if (m_changelogBrowser) {
        m_changelogBrowser->setStyleSheet(QStringLiteral(
            "QTextBrowser { background-color: %1; border: 1px solid %2; border-radius: 8px; padding: 6px; }"
        ).arg(cardBg, cardBorder));
    }

    // Badge styling based on state
    if (m_updateAvailable) {
        m_badgeLabel->setStyleSheet(QStringLiteral(
            "QLabel { background-color: rgba(59, 130, 246, 0.16); color: #3b82f6; "
            "border: 1px solid rgba(59, 130, 246, 0.35); border-radius: 10px; "
            "padding: 2px 10px; font-weight: 700; font-size: 11px; }"
        ));
    } else if (m_isError) {
        m_badgeLabel->setStyleSheet(QStringLiteral(
            "QLabel { background-color: rgba(245, 158, 11, 0.16); color: #f59e0b; "
            "border: 1px solid rgba(245, 158, 11, 0.35); border-radius: 10px; "
            "padding: 2px 10px; font-weight: 700; font-size: 11px; }"
        ));
    } else {
        m_badgeLabel->setStyleSheet(QStringLiteral(
            "QLabel { background-color: rgba(16, 185, 129, 0.16); color: #10b981; "
            "border: 1px solid rgba(16, 185, 129, 0.35); border-radius: 10px; "
            "padding: 2px 10px; font-weight: 700; font-size: 11px; }"
        ));
    }

    // Primary button modern accent styling
    if (m_primaryButton) {
        if (m_updateAvailable) {
            m_primaryButton->setStyleSheet(QStringLiteral(
                "QPushButton { background-color: #0284c7; color: white; border: none; "
                "border-radius: 6px; padding: 6px 18px; font-weight: 600; } "
                "QPushButton:hover { background-color: #0369a1; } "
                "QPushButton:pressed { background-color: #075985; }"
            ));
        } else {
            m_primaryButton->setStyleSheet(QStringLiteral(
                "QPushButton { min-width: 80px; padding: 6px 18px; font-weight: 600; border-radius: 6px; }"
            ));
        }
    }

    if (m_viewReleaseButton) {
        m_viewReleaseButton->setStyleSheet(QStringLiteral(
            "QPushButton { padding: 6px 14px; border-radius: 6px; }"
        ));
    }
    if (m_closeButton) {
        m_closeButton->setStyleSheet(QStringLiteral(
            "QPushButton { padding: 6px 14px; border-radius: 6px; }"
        ));
    }
}

void UpdateDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::ApplicationPaletteChange) {
        updateThemeColors();
    }
    QDialog::changeEvent(event);
}

void UpdateDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        reject();
        return;
    }
    QDialog::keyPressEvent(event);
}
