/*
 * This file is part of NotepadNext.
 *
 * Implements in-place tab file renaming dialog with disk verification,
 * name sanitization, and path conflict prevention.
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 */

#include "RenameTabDialog.h"

#include <QDir>
#include <QHBoxLayout>
#include <QRegularExpression>

RenameTabDialog::RenameTabDialog(const QString &currentFilePath, QWidget *parent)
    : QDialog(parent)
    , m_oldFilePath(currentFilePath)
    , m_fileInfo(currentFilePath)
{
    setWindowTitle(tr("Rename File — In-Place"));
    setMinimumWidth(480);
    setupUi();
    validateInput();
}

void RenameTabDialog::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    auto *infoLabel = new QLabel(tr("<b>Enter a new name for the file:</b>"), this);
    mainLayout->addWidget(infoLabel);

    m_locationLabel = new QLabel(this);
    m_locationLabel->setText(tr("Directory: <code>%1</code>").arg(m_fileInfo.dir().absolutePath()));
    m_locationLabel->setWordWrap(true);
    m_locationLabel->setStyleSheet(QStringLiteral("color: #888888; font-size: 11px;"));
    mainLayout->addWidget(m_locationLabel);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setText(m_fileInfo.fileName());
    const QString baseName = m_fileInfo.baseName();
    m_nameEdit->setSelection(0, static_cast<int>(baseName.length()));
    mainLayout->addWidget(m_nameEdit);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet(QStringLiteral("font-size: 11px;"));
    mainLayout->addWidget(m_statusLabel);

    auto *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    m_btnCancel = new QPushButton(tr("Cancel"), this);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(m_btnCancel);

    m_btnRename = new QPushButton(tr("Rename"), this);
    m_btnRename->setDefault(true);
    connect(m_btnRename, &QPushButton::clicked, this, [this] {
        m_newFilePath = m_fileInfo.dir().filePath(m_nameEdit->text().trimmed());
        accept();
    });
    btnLayout->addWidget(m_btnRename);

    mainLayout->addLayout(btnLayout);

    connect(m_nameEdit, &QLineEdit::textChanged, this, &RenameTabDialog::validateInput);
}

void RenameTabDialog::validateInput()
{
    const QString newName = m_nameEdit->text().trimmed();

    if (newName.isEmpty()) {
        m_statusLabel->setText(tr("<font color='#E53E3E'>File name cannot be empty.</font>"));
        m_btnRename->setEnabled(false);
        return;
    }

    if (newName == m_fileInfo.fileName()) {
        m_statusLabel->setText(tr("<font color='#888888'>Name unchanged.</font>"));
        m_btnRename->setEnabled(false);
        return;
    }

    // Check invalid characters for cross-platform safety
    static const QRegularExpression invalidChars(QStringLiteral(R"([\\/:*?"<>|])"));
    if (newName.contains(invalidChars)) {
        m_statusLabel->setText(tr("<font color='#E53E3E'>Name contains invalid characters (\\ / : * ? \" &lt; &gt; |).</font>"));
        m_btnRename->setEnabled(false);
        return;
    }

    const QString candidatePath = m_fileInfo.dir().filePath(newName);
    if (QFileInfo::exists(candidatePath)) {
        m_statusLabel->setText(tr("<font color='#E53E3E'>A file with this name already exists in the same folder.</font>"));
        m_btnRename->setEnabled(false);
        return;
    }

    m_statusLabel->setText(tr("<font color='#22C55E'>✓ Ready to rename.</font>"));
    m_btnRename->setEnabled(true);
}

QString RenameTabDialog::newFilePath() const
{
    return m_newFilePath;
}

QString RenameTabDialog::newFileName() const
{
    return QFileInfo(m_newFilePath).fileName();
}
