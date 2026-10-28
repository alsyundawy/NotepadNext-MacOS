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

#include "EditorInfoStatusBar.h"
#include "EncodingDetector.h"
#include "MainWindow.h"
#include "StatusLabel.h"


EditorInfoStatusBar::EditorInfoStatusBar(QMainWindow *window) :
    QStatusBar(window)
{
    // Set up the status bar
    docType = new StatusLabel();
    addWidget(docType, 1);

    docSize = new StatusLabel(200);
    addPermanentWidget(docSize, 0);

    docPos = new StatusLabel(250);
    addPermanentWidget(docPos, 0);

    eolFormat = new StatusLabel(100);
    addPermanentWidget(eolFormat, 0);
    eolFormat->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(qobject_cast<StatusLabel*>(eolFormat), &StatusLabel::customContextMenuRequested, this, [this](const QPoint &pos) {
        emit customContextMenuRequestedForEOLLabel(eolFormat->mapToGlobal(pos));
    });

    unicodeType = new StatusLabel(110);
    addPermanentWidget(unicodeType, 0);
    unicodeType->setToolTip(tr("Encoding / BOM (Click to view or convert)"));
    connect(qobject_cast<StatusLabel *>(unicodeType), &StatusLabel::clicked, this, [this]() {
        if (const auto *mw = qobject_cast<MainWindow *>(this->window())) {
            if (auto *act = mw->findChild<QAction *>("actionShowEncodingInfo")) {
                act->trigger();
            }
        }
    });

    overType = new StatusLabel(25);
    addPermanentWidget(overType, 0);

    const auto *w = qobject_cast<MainWindow *>(window);

    connect(w, &MainWindow::editorActivated, this, &EditorInfoStatusBar::connectToEditor);

    connect(qobject_cast<StatusLabel*>(overType), &StatusLabel::clicked, w, [w, this]() {
        if (auto *editor = w->currentEditor()) {
            editor->editToggleOvertype();
            updateOverType(editor);
        }
    });
}

void EditorInfoStatusBar::refresh(ScintillaNext *editor)
{
    if (!editor) {
        if (docSize) docSize->clear();
        if (docPos) docPos->clear();
        if (docType) docType->clear();
        if (eolFormat) eolFormat->clear();
        if (unicodeType) unicodeType->clear();
        if (overType) overType->clear();
        return;
    }

    updateDocumentSize(editor);
    updateSelectionInfo(editor);
    updateLanguage(editor);
    updateEol(editor);
    updateEncoding(editor);
    updateOverType(editor);
}

void EditorInfoStatusBar::connectToEditor(ScintillaNext *editor)
{
    // Remove any previous connections
    disconnect(editorUiUpdated);
    disconnect(documentLexerChanged);

    if (!editor) {
        refresh(nullptr);
        return;
    }

    // Connect to the new editor
    editorUiUpdated = connect(editor, &ScintillaNext::updateUi, this, &EditorInfoStatusBar::editorUpdated);
    documentLexerChanged = connect(editor, &ScintillaNext::lexerChanged, this, [this, editor]() { updateLanguage(editor); });

    refresh(editor);
}

void EditorInfoStatusBar::editorUpdated(Scintilla::Update updated)
{
    auto *editor = qobject_cast<ScintillaNext *>(sender());
    if (!editor) {
        return;
    }

    using enum Scintilla::Update;
    if (Scintilla::FlagSet(updated, Text)) {
        updateDocumentSize(editor);
    }

    if (Scintilla::FlagSet(updated, Text) || Scintilla::FlagSet(updated, Selection)) {
        updateSelectionInfo(editor);
    }
}

void EditorInfoStatusBar::updateDocumentSize(const ScintillaNext *editor)
{
    QString sizeText = tr("Length: %L1    Lines: %L2").arg(editor->length()).arg(editor->lineCount());
    docSize->setText(sizeText);
}

void EditorInfoStatusBar::updateSelectionInfo(ScintillaNext *editor)
{
    if (!editor) {
        return;
    }

    QString selectionText;

    if (editor->selections() > 1) {
        selectionText = tr("Sel: N/A");
    }
    else {
        const auto start = static_cast<int>(editor->selectionStart());
        const auto end = static_cast<int>(editor->selectionEnd());
        int lines = 0;
        int chars = 0;

        if (end > start) {
            lines = static_cast<int>(editor->lineFromPosition(end) - editor->lineFromPosition(start)) + 1;
            const int byteLen = end - start;
            if (byteLen > 65536) {
                chars = byteLen;
            } else {
                chars = static_cast<int>(editor->countCharacters(start, end));
            }
        }

        selectionText = tr("Sel: %L1 | %L2").arg(chars).arg(lines);
    }

    const auto pos = static_cast<int>(editor->currentPos());
    QString positionText = tr("Ln: %L1    Col: %L2    ").arg(editor->lineFromPosition(pos) + 1).arg(editor->column(pos) + 1);
    docPos->setText(positionText + selectionText);
}

void EditorInfoStatusBar::updateLanguage(const ScintillaNext *editor)
{
    docType->setText(editor->languageName);
}


void EditorInfoStatusBar::updateEol(const ScintillaNext *editor)
{
    // No good way to keep these in sync with the Main Window menu items :(

    switch(editor->eOLMode()) {
    case SC_EOL_CR:
        eolFormat->setText(tr("Macintosh (CR)"));
        break;
    case SC_EOL_CRLF:
        eolFormat->setText(tr("Windows (CR LF)"));
        break;
    case SC_EOL_LF:
        eolFormat->setText(tr("Unix (LF)"));
        break;
    default:
        break;
    }
}

void EditorInfoStatusBar::updateEncoding(const ScintillaNext *editor)
{
    if (!editor || !unicodeType) {
        return;
    }
    unicodeType->setText(EncodingDetector::formatDisplayName(editor->encodingName(), editor->bom()));
}

void EditorInfoStatusBar::updateOverType(const ScintillaNext *editor)
{
    bool overtype = editor->overtype();
    if (overtype) {
        //: This is a short abbreviation to indicate characters will be replaced when typing
        overType->setText(tr("OVR"));
    }
    else {
        //: This is a short abbreviation to indicate characters will be inserted when typing
        overType->setText(tr("INS"));
    }
}
