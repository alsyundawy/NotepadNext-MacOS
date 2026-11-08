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

#ifndef AUTOCLOSEPAIRS_H
#define AUTOCLOSEPAIRS_H

#include "../decorators/EditorDecorator.h"

class AutoClosePairs : public EditorDecorator {
    Q_OBJECT

public:
    explicit AutoClosePairs(ScintillaNext *editor);
    ~AutoClosePairs() override = default;

    [[nodiscard]] bool autoCloseBrackets() const { return m_autoCloseBrackets; }
    void setAutoCloseBrackets(bool enabled) { m_autoCloseBrackets = enabled; }

    [[nodiscard]] bool autoCloseQuotes() const { return m_autoCloseQuotes; }
    void setAutoCloseQuotes(bool enabled) { m_autoCloseQuotes = enabled; }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void notify(const Scintilla::NotificationData *pscn) override;

private:
    bool handleKeyPress(class QKeyEvent *keyEvent);
    static bool isWordChar(char ch);

    bool m_autoCloseBrackets{true};
    bool m_autoCloseQuotes{true};
};

#endif  // AUTOCLOSEPAIRS_H
