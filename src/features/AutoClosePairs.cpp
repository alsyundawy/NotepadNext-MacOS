/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "AutoClosePairs.h"
#include "../ScintillaNext.h"

#include <QEvent>
#include <QKeyEvent>

AutoClosePairs::AutoClosePairs(ScintillaNext *editor) : EditorDecorator(editor) {
    setObjectName(QStringLiteral("AutoClosePairs"));
    editor->installEventFilter(this);
}

void AutoClosePairs::notify(const Scintilla::NotificationData *pscn) {
    Q_UNUSED(pscn)
}

bool AutoClosePairs::isWordChar(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_';
}

bool AutoClosePairs::eventFilter(QObject *obj, QEvent *event) {
    Q_UNUSED(obj)

    if (event->type() == QEvent::KeyPress && isEnabled()) {
        const auto *keyEvent = static_cast<const QKeyEvent *>(event);
        if (handleKeyPress(keyEvent)) {
            return true;
        }
    }

    return false;
}

bool AutoClosePairs::handleBracketPair(QChar ch, int curPos, char charAtPos) {
    if (ch == '(') {
        editor->insertText(curPos, "()");
        editor->gotoPos(curPos + 1);
        return true;
    }
    if (ch == '[') {
        editor->insertText(curPos, "[]");
        editor->gotoPos(curPos + 1);
        return true;
    }
    if (ch == '{') {
        editor->insertText(curPos, "{}");
        editor->gotoPos(curPos + 1);
        return true;
    }
    if ((ch == ')' || ch == ']' || ch == '}') && charAtPos == ch.toLatin1()) {
        editor->gotoPos(curPos + 1);
        return true;
    }
    return false;
}

bool AutoClosePairs::handleQuotePair(QChar ch, int curPos, char charAtPos, char charBefore) {
    if (ch == '"') {
        if (charAtPos == '"') {
            editor->gotoPos(curPos + 1);
            return true;
        }
        editor->insertText(curPos, "\"\"");
        editor->gotoPos(curPos + 1);
        return true;
    }
    if (ch == '\'') {
        if (charAtPos == '\'') {
            editor->gotoPos(curPos + 1);
            return true;
        }
        if (!isWordChar(charBefore)) {
            editor->insertText(curPos, "''");
            editor->gotoPos(curPos + 1);
            return true;
        }
    }
    if (ch == '`') {
        if (charAtPos == '`') {
            editor->gotoPos(curPos + 1);
            return true;
        }
        editor->insertText(curPos, "``");
        editor->gotoPos(curPos + 1);
        return true;
    }
    return false;
}

bool AutoClosePairs::handleKeyPress(const QKeyEvent *keyEvent) {
    if (!editor || !editor->selectionEmpty()) {
        return false;
    }

    const auto curPos = static_cast<int>(editor->currentPos());
    const auto docLen = static_cast<int>(editor->length());
    const char charAtPos = (curPos < docLen) ? static_cast<char>(editor->charAt(curPos)) : 0;
    const char charBefore = (curPos > 0) ? static_cast<char>(editor->charAt(curPos - 1)) : 0;
    const QString text = keyEvent->text();

    if (text.isEmpty()) {
        // Handle Backspace between empty pairs
        if (keyEvent->key() == Qt::Key_Backspace && curPos > 0 && curPos < docLen) {
            const bool isPair = (charBefore == '(' && charAtPos == ')') || (charBefore == '[' && charAtPos == ']') ||
                                (charBefore == '{' && charAtPos == '}') || (charBefore == '"' && charAtPos == '"') ||
                                (charBefore == '\'' && charAtPos == '\'') || (charBefore == '`' && charAtPos == '`');
            if (isPair) {
                editor->deleteRange(curPos - 1, 2);
                return true;
            }
        }
        return false;
    }

    const QChar ch = text.at(0);

    if (m_autoCloseBrackets && handleBracketPair(ch, curPos, charAtPos)) {
        return true;
    }

    if (m_autoCloseQuotes && handleQuotePair(ch, curPos, charAtPos, charBefore)) {
        return true;
    }

    return false;
}
