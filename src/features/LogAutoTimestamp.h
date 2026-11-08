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

#ifndef LOGAUTOTIMESTAMP_H
#define LOGAUTOTIMESTAMP_H

#include <QString>

class ScintillaNext;

class LogAutoTimestamp {
public:
    static bool hasLogHeader(ScintillaNext *editor);
    static bool checkAndApplyLogTimestamp(ScintillaNext *editor, bool force = false);
    static bool appendLogTimestamp(ScintillaNext *editor);
    static bool insertLogHeader(ScintillaNext *editor);
    static bool isAutoLogEnabled();
    static void setAutoLogEnabled(bool enabled);
    static QString currentLogTimestampString();
};

#endif  // LOGAUTOTIMESTAMP_H
