/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUSTION | https://www.alsyundawy.com
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

#include "BookMarkDecorator.h"
#include "UndoAction.h"

const int MARK_BOOKMARK = 24;
const int MARGIN = 1;

BookMarkDecorator::BookMarkDecorator(ScintillaNext *editor) :
    EditorDecorator(editor)
{
    editor->markerSetAlpha(MARK_BOOKMARK, 70);
    editor->markerDefine(MARK_BOOKMARK, SC_MARK_BOOKMARK);
    editor->markerSetFore(MARK_BOOKMARK, 0xFF2020);
    editor->markerSetBack(MARK_BOOKMARK, 0xFF2020);

    const int mask = static_cast<int>(editor->marginMaskN(MARGIN));
    editor->setMarginMaskN(MARGIN, static_cast<int>(1U << static_cast<unsigned int>(MARK_BOOKMARK)) | mask);

    editor->setMarginSensitiveN(MARGIN, true);
}

bool BookMarkDecorator::isBookmarkSet(int line) const
{
    return (static_cast<unsigned int>(editor->markerGet(line)) & (1U << static_cast<unsigned int>(MARK_BOOKMARK))) != 0U;
}

void BookMarkDecorator::addBookmark(int line)
{
    editor->markerAdd(line, MARK_BOOKMARK);
}

void BookMarkDecorator::removeBookmark(int line)
{
    // The marker can be set multiple times, so keep deleting it till it is no longer set
    while (isBookmarkSet(line)) {
        editor->markerDelete(line, MARK_BOOKMARK);
    }
}

void BookMarkDecorator::toggleBookmark(int line)
{
    if (isBookmarkSet(line)) {
        removeBookmark(line);
    }
    else {
        addBookmark(line);
    }
}

int BookMarkDecorator::nextBookmarkAfter(int line)
{
    const auto nextMarkedLine = static_cast<int>(editor->markerNext(line, 1U << static_cast<unsigned int>(MARK_BOOKMARK)));

    if (nextMarkedLine == -1) {
        return static_cast<int>(editor->markerNext(0, 1U << static_cast<unsigned int>(MARK_BOOKMARK)));
    }
    else {
        return nextMarkedLine;
    }
}

int BookMarkDecorator::previousBookMarkBefore(int line)
{
    const auto prevMarkedLine = static_cast<int>(editor->markerPrevious(line, 1U << static_cast<unsigned int>(MARK_BOOKMARK)));

    if (prevMarkedLine == -1) {
        return static_cast<int>(editor->markerPrevious(editor->lineCount(), 1U << static_cast<unsigned int>(MARK_BOOKMARK)));
    }
    else {
        return prevMarkedLine;
    }
}

void BookMarkDecorator::clearAllBookmarks()
{
    editor->markerDeleteAll(MARK_BOOKMARK);
}

QList<int> BookMarkDecorator::bookMarkedLines() const
{
    QList<int> bookMarkedLines;

    int line = 0;
    forever {
        line = static_cast<int>(editor->markerNext(line, 1U << static_cast<unsigned int>(MARK_BOOKMARK)));

        if (line != -1) {
            bookMarkedLines.append(line);
            line++;
        }
        else {
            break;
        }
    }

    return bookMarkedLines;
}

void BookMarkDecorator::setBookMarkedLines(const QList<int> &lines)
{
    // Make sure they are all clear first
    clearAllBookmarks();

    for (const int line : lines) {
        addBookmark(line);
    }
}

QString BookMarkDecorator::cutBookMarkedLines()
{
    QString s = copyBookMarkedLines();

    deleteBookMarkedLines();

    return s;
}

QString BookMarkDecorator::copyBookMarkedLines()
{
    QByteArray s;

    for (const int line : bookMarkedLines()) {
        s += editor->getLine(line);
    }

    return QString(s);
}

void BookMarkDecorator::deleteBookMarkedLines()
{
    const UndoAction ua(editor);

    forever {
        // Since every time a bookmark is found it is deleted, the search can restart from the beginning again to find the next bookmark
        const auto line = static_cast<int>(editor->markerNext(0, 1U << static_cast<unsigned int>(MARK_BOOKMARK)));

        if (line != -1) {
            removeBookmark(line);

            const auto lineStart = static_cast<int>(editor->positionFromLine(line));
            const auto lineLength = static_cast<int>(editor->lineLength(line));
            editor->deleteRange(lineStart, lineLength);
        }
        else {
            break;
        }
    }
}

void BookMarkDecorator::notify(const Scintilla::NotificationData *pscn)
{
    if (pscn->nmhdr.code == Scintilla::Notification::MarginClick && pscn->margin == MARGIN) {
        const auto line = static_cast<int>(editor->lineFromPosition(pscn->position));
        toggleBookmark(line);
    }
}
