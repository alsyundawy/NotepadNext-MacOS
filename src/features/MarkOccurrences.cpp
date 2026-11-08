/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "MarkOccurrences.h"
#include "../ScintillaNext.h"
#include <QByteArray>

static constexpr const char *const kIndicatorName = "notesci_mark_all";

static int QColorToScintillaColour(const QColor &c) {
    return c.red() | (c.green() << 8) | (c.blue() << 16);
}

MarkOccurrences::MarkOccurrences(QObject *parent) : QObject(parent) {}

void MarkOccurrences::setupIndicator(ScintillaNext *editor, int indicator) const {
    if (!editor || indicator < 0) {
        return;
    }

    editor->indicSetFore(indicator, QColorToScintillaColour(m_indicatorColor));
    editor->indicSetStyle(indicator, INDIC_ROUNDBOX);
    editor->indicSetOutlineAlpha(indicator, 180);
    editor->indicSetAlpha(indicator, 90);
    editor->indicSetUnder(indicator, true);
}

int MarkOccurrences::indicatorForEditor(ScintillaNext *editor) const {
    if (!editor) {
        return -1;
    }
    return editor->allocateIndicator(QString::fromLatin1(kIndicatorName));
}

int MarkOccurrences::markAll(ScintillaNext *editor, bool matchCase, bool wholeWord, bool useRegex) {
    if (!editor) {
        return 0;
    }

    auto selectionStart = static_cast<int>(editor->selectionStart());
    auto selectionEnd = static_cast<int>(editor->selectionEnd());
    bool isWord = wholeWord;

    if (selectionStart == selectionEnd) {
        const auto curPos = static_cast<int>(editor->currentPos());
        selectionStart = static_cast<int>(editor->wordStartPosition(curPos, true));
        selectionEnd = static_cast<int>(editor->wordEndPosition(curPos, true));
        isWord = true;

        if (selectionStart == selectionEnd) {
            clearMarks(editor);
            return 0;
        }
    }

    if ((selectionEnd - selectionStart) > 2048) {
        return 0;
    }

    const QByteArray selText = editor->get_text_range(selectionStart, selectionEnd);
    return markText(editor, QString::fromUtf8(selText), matchCase, isWord, useRegex);
}

int MarkOccurrences::markText(ScintillaNext *editor, const QString &text, bool matchCase, bool wholeWord,
                              bool useRegex) {
    if (!editor || text.isEmpty()) {
        m_lastMatchCount = 0;
        m_lastQuery.clear();
        return 0;
    }

    const int indicator = indicatorForEditor(editor);
    setupIndicator(editor, indicator);

    editor->setIndicatorCurrent(indicator);
    editor->indicatorClearRange(0, static_cast<int>(editor->length()));

    int flags = 0;
    if (matchCase) {
        flags |= SCFIND_MATCHCASE;
    }
    if (wholeWord) {
        flags |= SCFIND_WHOLEWORD;
    }
    if (useRegex) {
        flags |= SCFIND_REGEXP;
    }

    editor->setSearchFlags(flags);

    const QByteArray queryBytes = text.toUtf8();
    int count = 0;

    editor->forEachMatch(queryBytes, [&count, editor](int start, int end) {
        if (start < end) {
            editor->indicatorFillRange(start, end - start);
            count++;
        }
        return end;
    });

    m_lastMatchCount = count;
    m_lastQuery = text;

    emit occurrencesMarked(text, count);
    return count;
}

void MarkOccurrences::clearMarks(ScintillaNext *editor) {
    if (!editor) {
        return;
    }

    if (const int indicator = indicatorForEditor(editor); indicator >= 0) {
        editor->setIndicatorCurrent(indicator);
        editor->indicatorClearRange(0, static_cast<int>(editor->length()));
    }

    m_lastMatchCount = 0;
    m_lastQuery.clear();
    emit marksCleared();
}
