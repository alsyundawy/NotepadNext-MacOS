/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * POSIX File Inspector, Shebang & Shell Dialect Detector,
 * Permission Inspector & Fast File Timestamp Touch Utility.
 *
 * Copyright 2025-2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "PosixFileInspector.h"

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

#ifdef Q_OS_UNIX
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#endif

QString PosixFileInspector::detectDialectFromShebang(const QString &firstLine, QString *outLexer) {
    const QString line = firstLine.trimmed();

    // FreeBSD rc.d script headers
    if (line.startsWith(QStringLiteral("# PROVIDE:")) || line.startsWith(QStringLiteral("# REQUIRE:")) ||
        line.startsWith(QStringLiteral("# KEYWORD:"))) {
        if (outLexer) {
            *outLexer = QStringLiteral("bash");
        }
        return QStringLiteral("FreeBSD rc.d");
    }

    if (!line.startsWith(QStringLiteral("#!"))) {
        return {};
    }

    // Shebang matching
    static const struct {
        const char *pattern;
        const char *dialect;
        const char *lexer;
    } DialectMap[] = {{R"(/bin/bash|/usr/bin/bash|/usr/local/bin/bash|env\s+bash)", "Bash", "bash"},
                      {R"(/bin/zsh|/usr/bin/zsh|/usr/local/bin/zsh|env\s+zsh)", "Zsh", "bash"},
                      {R"(/bin/dash|/usr/bin/dash|env\s+dash)", "Dash", "bash"},
                      {R"(/bin/ksh|/usr/bin/ksh|env\s+ksh)", "Ksh", "bash"},
                      {R"(/bin/sh|/usr/bin/sh|env\s+sh)", "POSIX sh", "bash"},
                      {R"(python3?|env\s+python3?)", "Python", "python"},
                      {R"(lua|env\s+lua)", "Lua", "lua"},
                      {R"(perl|env\s+perl)", "Perl", "perl"},
                      {R"(ruby|env\s+ruby)", "Ruby", "ruby"},
                      {R"(node|env\s+node)", "Node.js", "javascript"}};

    for (const auto &entry : DialectMap) {
        const QRegularExpression re(QString::fromUtf8(entry.pattern), QRegularExpression::CaseInsensitiveOption);
        if (line.contains(re)) {
            if (outLexer) {
                *outLexer = QString::fromUtf8(entry.lexer);
            }
            return QString::fromUtf8(entry.dialect);
        }
    }

    if (outLexer) {
        *outLexer = QStringLiteral("bash");
    }
    return QStringLiteral("Shell Script");
}

PosixFileInspector::InspectionResult PosixFileInspector::inspect(const QString &filePath, const QString &firstLine) {
    InspectionResult res;
    if (filePath.isEmpty()) {
        return res;
    }

    const QFileInfo fi(filePath);
    if (!fi.exists()) {
        return res;
    }

    res.isValidFile = true;
    res.lastModified = fi.lastModified();
    res.lastAccessed = fi.lastRead();

    // Read first line if not provided
    QString headerLine = firstLine;
    if (headerLine.isEmpty() && fi.isFile() && fi.size() > 0) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            headerLine = QString::fromUtf8(file.readLine(256));
            file.close();
        }
    }

    res.shellDialect = detectDialectFromShebang(headerLine, &res.suggestedLexer);
    res.isScript = !res.shellDialect.isEmpty();

#ifdef Q_OS_UNIX
    struct stat st{};
    if (::stat(QFile::encodeName(filePath).constData(), &st) == 0) {
        res.octalMode = static_cast<uint32_t>(st.st_mode & 07777);
        res.octalString = QString::asprintf("%04o", res.octalMode & 0777);

        // Symbolic format: rwxr-xr-x
        QString sym;
        const mode_t m = st.st_mode;
        sym += (m & S_IRUSR) ? QLatin1Char('r') : QLatin1Char('-');
        sym += (m & S_IWUSR) ? QLatin1Char('w') : QLatin1Char('-');
        sym += (m & S_IXUSR) ? QLatin1Char('x') : QLatin1Char('-');
        sym += (m & S_IRGRP) ? QLatin1Char('r') : QLatin1Char('-');
        sym += (m & S_IWGRP) ? QLatin1Char('w') : QLatin1Char('-');
        sym += (m & S_IXGRP) ? QLatin1Char('x') : QLatin1Char('-');
        sym += (m & S_IROTH) ? QLatin1Char('r') : QLatin1Char('-');
        sym += (m & S_IWOTH) ? QLatin1Char('w') : QLatin1Char('-');
        sym += (m & S_IXOTH) ? QLatin1Char('x') : QLatin1Char('-');
        res.symbolicString = std::move(sym);

        res.isExecutable = (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0;
        res.isOwnerOnly = ((st.st_mode & 0077) == 0);
    }
#else
    // Fallback on non-UNIX
    const QFile::Permissions perms = fi.permissions();
    res.isExecutable = (perms & QFile::ExeUser) != 0;
    res.octalMode = res.isExecutable ? 0755 : 0644;
    res.octalString = res.isExecutable ? QStringLiteral("0755") : QStringLiteral("0644");
    res.symbolicString = res.isExecutable ? QStringLiteral("rwxr-xr-x") : QStringLiteral("rw-r--r--");
#endif

    return res;
}

bool PosixFileInspector::toggleExecutable(const QString &filePath, QString *errorMessage) {
#ifdef Q_OS_UNIX
    struct stat st{};
    const QByteArray encoded = QFile::encodeName(filePath);
    if (::stat(encoded.constData(), &st) != 0) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Cannot stat file.");
        }
        return false;
    }

    mode_t newMode = st.st_mode;
    if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        // Remove executable bits
        newMode &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
    } else {
        // Add executable bits: 0755
        newMode |= (S_IXUSR | S_IXGRP | S_IXOTH);
    }

    if (::chmod(encoded.constData(), newMode & 07777) != 0) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("chmod failed.");
        }
        return false;
    }
    return true;
#else
    Q_UNUSED(filePath);
    if (errorMessage) {
        *errorMessage = QStringLiteral("chmod not supported on this platform.");
    }
    return false;
#endif
}

bool PosixFileInspector::setOwnerOnly(const QString &filePath, QString *errorMessage) {
#ifdef Q_OS_UNIX
    const QByteArray encoded = QFile::encodeName(filePath);
    // Enforce 0600 (owner read/write only)
    if (::chmod(encoded.constData(), S_IRUSR | S_IWUSR) != 0) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("Failed to set 0600 permission.");
        }
        return false;
    }
    return true;
#else
    Q_UNUSED(filePath);
    if (errorMessage) {
        *errorMessage = QStringLiteral("chmod not supported on this platform.");
    }
    return false;
#endif
}

bool PosixFileInspector::touchFile(const QString &filePath, QString *errorMessage) {
    if (filePath.isEmpty() || !QFileInfo::exists(filePath)) {
        if (errorMessage) {
            *errorMessage = QStringLiteral("File does not exist.");
        }
        return false;
    }

#ifdef Q_OS_UNIX
    const QByteArray encoded = QFile::encodeName(filePath);
#if defined(AT_FDCWD) && !defined(Q_OS_DARWIN)
    if (::utimensat(AT_FDCWD, encoded.constData(), nullptr, 0) == 0) {
        return true;
    }
#endif
    // Fallback using utimes(path, NULL) sets to current time
    if (::utimes(encoded.constData(), nullptr) == 0) {
        return true;
    }
#endif

    // High-level fallback via QFile::setFileTime
    QFile f(filePath);
    const QDateTime now = QDateTime::currentDateTime();
    if (f.setFileTime(now, QFileDevice::FileModificationTime) && f.setFileTime(now, QFileDevice::FileAccessTime)) {
        return true;
    }

    if (errorMessage) {
        *errorMessage = QStringLiteral("Failed to update timestamp via utimes / QFile.");
    }
    return false;
}

QString PosixFileInspector::formatPermissionBadge(const InspectionResult &res) {
    if (!res.isValidFile) {
        return {};
    }
    if (!res.shellDialect.isEmpty()) {
        return QStringLiteral("%1 | %2 %3").arg(res.shellDialect, res.octalString, res.symbolicString);
    }
    return QStringLiteral("%1 %2").arg(res.octalString, res.symbolicString);
}
