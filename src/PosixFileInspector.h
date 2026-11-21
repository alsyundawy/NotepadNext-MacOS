#pragma once
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

#ifndef POSIXFILEINSPECTOR_H
#define POSIXFILEINSPECTOR_H

#include <QDateTime>
#include <QFileInfo>
#include <QString>
#include <cstdint>

class PosixFileInspector {
public:
    struct InspectionResult {
        bool isValidFile{false};
        QString shellDialect;    // "Bash", "Zsh", "POSIX sh", "Dash", "Ksh", "FreeBSD rc.d", "Python", "Lua"
        QString suggestedLexer;  // "bash", "python", "lua"
        bool isScript{false};
        uint32_t octalMode{0};     // e.g. 0755, 0644, 0600
        QString octalString;       // "0755"
        QString symbolicString;    // "rwxr-xr-x"
        bool isExecutable{false};  // User/group/other execute bit set
        bool isOwnerOnly{false};   // 0600 or 0700
        QDateTime lastModified;
        QDateTime lastAccessed;
    };

    // Inspect file path and optional first line content
    static InspectionResult inspect(const QString &filePath, const QString &firstLine = QString());

    // Detect shebang and script dialect from first line
    static QString detectDialectFromShebang(const QString &firstLine, QString *outLexer = nullptr);

    // Permission Operations
    static bool toggleExecutable(const QString &filePath, QString *errorMessage = nullptr);
    static bool setOwnerOnly(const QString &filePath, QString *errorMessage = nullptr);

    // Update File Timestamp (Touch) via POSIX utimensat / utimes
    static bool touchFile(const QString &filePath, QString *errorMessage = nullptr);

    // Format helper for status bar badge
    static QString formatPermissionBadge(const InspectionResult &res);
};

#endif  // POSIXFILEINSPECTOR_H
