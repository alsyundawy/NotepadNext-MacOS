/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Adapted from Scintilla Change History and Notepad++ / Notepad4
 * change tracking margin concept.
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "ChangeHistoryDecorator.h"
#include "ScintillaNext.h"

ChangeHistoryDecorator::ChangeHistoryDecorator(ScintillaNext *editor) : EditorDecorator(editor) {
    setupMarkers();
}

ChangeHistoryDecorator::~ChangeHistoryDecorator() {}

void ChangeHistoryDecorator::setupMarkers() {
    if (!editor)
        return;

    // Ensure Scintilla has at least 5 margins available
    if (editor->margins() < 5) {
        editor->setMargins(5);
    }

    // Configure margin 3 for change history
    editor->setMarginTypeN(MARGIN_CHANGE_HISTORY, SC_MARGIN_SYMBOL);
    editor->setMarginMaskN(MARGIN_CHANGE_HISTORY, SC_MASK_HISTORY);
    editor->setMarginSensitiveN(MARGIN_CHANGE_HISTORY, true);

    // Style the 4 change history markers with continuous bars (SC_MARK_BAR)
    // 1. Modified since last save (Amber / Yellow #E5A50A)
    editor->markerDefine(SC_MARKNUM_HISTORY_MODIFIED, SC_MARK_BAR);
    editor->markerSetFore(SC_MARKNUM_HISTORY_MODIFIED, 0x0AA5E5);
    editor->markerSetBack(SC_MARKNUM_HISTORY_MODIFIED, 0x0AA5E5);

    // 2. Saved changes (Emerald Green #22C55E)
    editor->markerDefine(SC_MARKNUM_HISTORY_SAVED, SC_MARK_BAR);
    editor->markerSetFore(SC_MARKNUM_HISTORY_SAVED, 0x5EC522);
    editor->markerSetBack(SC_MARKNUM_HISTORY_SAVED, 0x5EC522);

    // 3. Reverted to origin (Sky Blue #3B82F6)
    editor->markerDefine(SC_MARKNUM_HISTORY_REVERTED_TO_ORIGIN, SC_MARK_BAR);
    editor->markerSetFore(SC_MARKNUM_HISTORY_REVERTED_TO_ORIGIN, 0xF6823B);
    editor->markerSetBack(SC_MARKNUM_HISTORY_REVERTED_TO_ORIGIN, 0xF6823B);

    // 4. Reverted to modified (Orange-Red #F97316)
    editor->markerDefine(SC_MARKNUM_HISTORY_REVERTED_TO_MODIFIED, SC_MARK_BAR);
    editor->markerSetFore(SC_MARKNUM_HISTORY_REVERTED_TO_MODIFIED, 0x1673F9);
    editor->markerSetBack(SC_MARKNUM_HISTORY_REVERTED_TO_MODIFIED, 0x1673F9);
}

void ChangeHistoryDecorator::setEnabled(bool b) {
    enabled = b;
    if (!editor)
        return;

    if (b) {
        setupMarkers();
        editor->setMarginWidthN(MARGIN_CHANGE_HISTORY, DEFAULT_MARGIN_WIDTH);
        editor->setChangeHistory(SC_CHANGE_HISTORY_MARKERS | SC_CHANGE_HISTORY_INDICATORS);
    } else {
        editor->setMarginWidthN(MARGIN_CHANGE_HISTORY, 0);
        editor->setChangeHistory(SC_CHANGE_HISTORY_DISABLED);
    }

    emit stateChanged(b);
}

int ChangeHistoryDecorator::nextChangeAfter(int line) {
    if (!editor)
        return -1;

    int nextLine = editor->markerNext(line + 1, SC_MASK_HISTORY);
    if (nextLine == -1) {
        // Wrap around to beginning
        return editor->markerNext(0, SC_MASK_HISTORY);
    }
    return nextLine;
}

int ChangeHistoryDecorator::previousChangeBefore(int line) {
    if (!editor)
        return -1;

    int prevLine = editor->markerPrevious(line - 1, SC_MASK_HISTORY);
    if (prevLine == -1) {
        // Wrap around to end
        return editor->markerPrevious(editor->lineCount() - 1, SC_MASK_HISTORY);
    }
    return prevLine;
}

void ChangeHistoryDecorator::notify(const Scintilla::NotificationData * /*pscn*/) {}
