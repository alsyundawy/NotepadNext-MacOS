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

#ifndef MARKOCCURRENCES_H
#define MARKOCCURRENCES_H

#include <QObject>
#include <QString>
#include <QColor>

class ScintillaNext;

class MarkOccurrences : public QObject {
    Q_OBJECT

public:
    explicit MarkOccurrences(QObject *parent = nullptr);
    ~MarkOccurrences() override = default;

    int markAll(ScintillaNext *editor, bool matchCase = false, bool wholeWord = false, bool useRegex = false);
    int markText(ScintillaNext *editor, const QString &text, bool matchCase = false, bool wholeWord = false,
                 bool useRegex = false);
    void clearMarks(ScintillaNext *editor);
    [[nodiscard]] int indicatorForEditor(ScintillaNext *editor) const;

    [[nodiscard]] int lastMatchCount() const { return m_lastMatchCount; }
    [[nodiscard]] const QString &lastQuery() const { return m_lastQuery; }

signals:
    void occurrencesMarked(const QString &query, int count);
    void marksCleared();

private:
    void setupIndicator(ScintillaNext *editor, int indicator);

    int m_lastMatchCount{0};
    QString m_lastQuery;
    QColor m_indicatorColor{255, 153, 0};
};

#endif  // MARKOCCURRENCES_H
