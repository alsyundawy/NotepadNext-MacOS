/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry DS Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
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

#include "Finder.h"

#include "UndoAction.h"

Finder::Finder(ScintillaNext *editor) :
    editor(editor)
{
}

void Finder::setEditor(ScintillaNext *newEditor)
{
    this->editor = newEditor;
}

int Finder::searchFlags() const
{
    return static_cast<int>(find_options.flags);
}

bool Finder::isRegexSearch() const
{
    return Scintilla::FlagSet(find_options.flags, Scintilla::FindOption::RegExp);
}

Sci_CharacterRange Finder::searchTarget(Sci_Position start, Sci_Position end)
{
    const QByteArray searchText = find_options.text.toUtf8();

    editor->setTargetRange(start, end);

    if (editor->searchInTarget(searchText.length(), searchText.constData()) == INVALID_POSITION) {
        return {INVALID_POSITION, INVALID_POSITION};
    }

    return {
        static_cast<Sci_PositionCR>(editor->targetStart()),
        static_cast<Sci_PositionCR>(editor->targetEnd())
    };
}

FindResult Finder::findNext()
{
    return findNextFrom(editor->selectionEnd());
}

FindResult Finder::findNextFrom(Sci_Position startPos)
{
    FindResult result;

    if (find_options.text.isEmpty()) {
        return result;
    }

    editor->setSearchFlags(searchFlags());

    if (use_selection_range) {
        const Sci_Position rangeEnd = selection_range.cpMax;
        Sci_Position pos = startPos;
        if (pos < selection_range.cpMin) { pos = selection_range.cpMin; }
        if (pos > rangeEnd) { pos = rangeEnd; }

        result.range = searchTarget(pos, rangeEnd);
        if (result || !find_options.wrapAround) {
            return result;
        }

        if (pos > selection_range.cpMin) {
            result.range = searchTarget(selection_range.cpMin, pos);
            if (result) {
                result.wrapped = true;
            }
        }
        return result;
    }

    result.range = searchTarget(startPos, editor->length());

    // Return the valid result, or if not wrap around, go ahead and return it anyways
    if (result || !find_options.wrapAround) {
        return result;
    }

    result.range = searchTarget(0, startPos);

    if (result) {
        result.wrapped = true;
    }

    return result;
}

FindResult Finder::findPrev()
{
    FindResult result;

    if (find_options.text.isEmpty()) {
        return result;
    }

    const Sci_Position startPos = editor->selectionStart();

    editor->setSearchFlags(searchFlags());

    if (use_selection_range) {
        Sci_Position pos = startPos;
        if (pos > selection_range.cpMax) { pos = selection_range.cpMax; }
        if (pos < selection_range.cpMin) { pos = selection_range.cpMin; }

        result.range = searchTarget(pos, selection_range.cpMin);
        if (result || !find_options.wrapAround) {
            return result;
        }

        if (pos < selection_range.cpMax) {
            result.range = searchTarget(selection_range.cpMax, pos);
            if (result) {
                result.wrapped = true;
            }
        }
        return result;
    }

    // A target with start > end searches backwards.
    result.range = searchTarget(startPos, 0);

    // Return the valid result, or if not wrap around, go ahead and return it anyways
    if (result || !find_options.wrapAround) {
        return result;
    }

    result.range = searchTarget(editor->length(), startPos);

    if (result) {
        result.wrapped = true;
    }

    return result;
}

int Finder::count()
{
    if (find_options.text.isEmpty()) {
        return 0;
    }

    int total = 0;

    auto counter = [&total](int start, int end) {
        Q_UNUSED(start);
        ++total;
        return end;
    };

    if (use_selection_range) {
        forEachMatchInRange(counter, selection_range);
    } else {
        forEachMatch(counter);
    }

    return total;
}

FindResult Finder::replaceSelectionIfMatch(const QString &replaceText)
{
    FindResult result;

    if (find_options.text.isEmpty()) {
        return result;
    }

    const Sci_Position selectionStart = editor->selectionStart();
    const Sci_Position selectionEnd = editor->selectionEnd();

    if (use_selection_range && (selectionStart < selection_range.cpMin || selectionEnd > selection_range.cpMax)) {
        return result;
    }

    result.range = searchTarget(selectionStart, selectionEnd);

    if (!result) {
        return result;
    }

    const QByteArray replacementText = replaceText.toUtf8();
    const Sci_Position matchStart = editor->targetStart();
    const Sci_Position matchEnd = editor->targetEnd();
    Sci_Position replacementLength;

    if (isRegexSearch()) {
        replacementLength = editor->replaceTargetRE(replacementText.length(), replacementText.constData());
    }
    else {
        replacementLength = editor->replaceTarget(replacementText.length(), replacementText.constData());
    }

    if (use_selection_range) {
        selection_range.cpMax += replacementLength - (matchEnd - matchStart);
    }

    result.range = {
        static_cast<Sci_PositionCR>(matchStart),
        static_cast<Sci_PositionCR>(matchStart + replacementLength)
    };

    return result;
}

int Finder::replaceAll(const QString &replaceText)
{
    if (find_options.text.isEmpty()) {
        return 0;
    }

    const QByteArray searchText = find_options.text.toUtf8();
    const QByteArray replacementText = replaceText.toUtf8();

    const Sci_Position rangeStart = use_selection_range ? selection_range.cpMin : 0;
    Sci_Position rangeEnd = use_selection_range ? selection_range.cpMax : editor->length();

    Sci_TextToFind findText{
        {static_cast<Sci_PositionCR>(rangeStart), static_cast<Sci_PositionCR>(rangeEnd)},
        searchText.constData(),
        {-1, -1}
    };

    editor->setSearchFlags(searchFlags());

    UndoAction undoAction(editor);

    int total = 0;

    // We intentionally use SCI_FINDTEXT here rather than forEachMatch().
    //
    // Replacing text changes the document length, so the search range has
    // to be updated after every replacement.
    while (editor->send(SCI_FINDTEXT, searchFlags(), reinterpret_cast<sptr_t>(&findText)) != INVALID_POSITION) {
        const Sci_Position start = findText.chrgText.cpMin;
        const Sci_Position end = findText.chrgText.cpMax;

        editor->setTargetRange(start, end);

        Sci_Position replacementLength;
        if (isRegexSearch()) {
            replacementLength = editor->replaceTargetRE(replacementText.length(), replacementText.constData());
        }
        else {
            replacementLength = editor->replaceTarget(replacementText.length(), replacementText.constData());
        }

        // Continue searching after the replacement.
        findText.chrg.cpMin = start + replacementLength;
        if (use_selection_range) {
            rangeEnd += findText.chrg.cpMin - end; // delta = newLength - oldMatchLength
            findText.chrg.cpMax = rangeEnd;
        } else {
            findText.chrg.cpMax = editor->length();
        }

        ++total;

        // An empty match must advance past the next original character, not
        // match the same position again after inserting the replacement.
        if (start == end) {
            if (findText.chrg.cpMin == findText.chrg.cpMax) {
                break;
            }

            findText.chrg.cpMin = editor->positionAfter(findText.chrg.cpMin);
        }
    }

    if (use_selection_range) {
        selection_range.cpMax = rangeEnd;
    }

    return total;
}
