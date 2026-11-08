/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "LogAutoTimestamp.h"
#include "../ScintillaNext.h"

#include <QDateTime>
#include <QSettings>

bool LogAutoTimestamp::hasLogHeader(ScintillaNext *editor) {
    if (!editor || editor->lineCount() <= 0) {
        return false;
    }

    const QByteArray line0 = editor->getLine(0);
    const QString lineStr = QString::fromUtf8(line0).trimmed();
    return (lineStr == QLatin1String(".LOG"));
}

QString LogAutoTimestamp::currentLogTimestampString() {
    const QSettings settings;
    const QString customFmt = settings.value(QStringLiteral("Settings/LogTimestampFormat")).toString();

    const QDateTime now = QDateTime::currentDateTime();
    if (!customFmt.isEmpty()) {
        return now.toString(customFmt);
    }

    return now.toString(QStringLiteral("hh:mm AP MM/dd/yyyy"));
}

bool LogAutoTimestamp::checkAndApplyLogTimestamp(ScintillaNext *editor, bool force) {
    if (!editor) {
        return false;
    }
    if (editor->readOnly()) {
        return false;
    }

    if (!force && editor->QObject::property("LogTimestampApplied").toBool()) {
        return false;
    }

    if (!hasLogHeader(editor)) {
        return false;
    }

    editor->QObject::setProperty("LogTimestampApplied", true);

    const auto docLen = static_cast<int>(editor->length());
    const QString eol = (editor->eOLMode() == SC_EOL_CRLF) ? QStringLiteral("\r\n") : QStringLiteral("\n");

    QString prefix;
    if (docLen > 0) {
        const QByteArray lastChar = editor->get_text_range(docLen - 1, docLen);
        if (lastChar != "\n" && lastChar != "\r") {
            prefix = eol;
        }
    }

    const QString timestamp = currentLogTimestampString();
    const QString entry = prefix + timestamp + eol;

    editor->beginUndoAction();
    const QByteArray utf8 = entry.toUtf8();
    editor->insertText(docLen, utf8.constData());
    const auto newPos = docLen + static_cast<int>(utf8.length());
    editor->setSelection(newPos, newPos);
    editor->endUndoAction();

    editor->scrollCaret();
    return true;
}

bool LogAutoTimestamp::appendLogTimestamp(ScintillaNext *editor) {
    if (!editor || editor->readOnly()) {
        return false;
    }

    const auto docLen = static_cast<int>(editor->length());
    const QString eol = (editor->eOLMode() == SC_EOL_CRLF) ? QStringLiteral("\r\n") : QStringLiteral("\n");

    QString prefix;
    if (docLen > 0) {
        const QByteArray lastChar = editor->get_text_range(docLen - 1, docLen);
        if (lastChar != "\n" && lastChar != "\r") {
            prefix = eol;
        }
    }

    const QString timestamp = currentLogTimestampString();
    const QString entry = prefix + timestamp + eol;

    editor->beginUndoAction();
    const QByteArray utf8 = entry.toUtf8();
    editor->insertText(docLen, utf8.constData());
    const auto newPos = docLen + static_cast<int>(utf8.length());
    editor->setSelection(newPos, newPos);
    editor->endUndoAction();

    editor->scrollCaret();
    return true;
}

bool LogAutoTimestamp::insertLogHeader(ScintillaNext *editor) {
    if (!editor || editor->readOnly()) {
        return false;
    }
    if (hasLogHeader(editor)) {
        return false;
    }

    const QString eol = (editor->eOLMode() == SC_EOL_CRLF) ? QStringLiteral("\r\n") : QStringLiteral("\n");
    const QString header = QStringLiteral(".LOG") + eol;

    editor->beginUndoAction();
    const QByteArray utf8 = header.toUtf8();
    editor->insertText(0, utf8.constData());
    editor->endUndoAction();
    return true;
}

bool LogAutoTimestamp::isAutoLogEnabled() {
    const QSettings settings;
    return settings.value(QStringLiteral("Settings/AutoLogTimestamp"), true).toBool();
}

void LogAutoTimestamp::setAutoLogEnabled(bool enabled) {
    QSettings settings;
    settings.setValue(QStringLiteral("Settings/AutoLogTimestamp"), enabled);
}
