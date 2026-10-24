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
#include <utility>

#include "EditorInspectorDock.h"
#include "ui_EditorInspectorDock.h"

#include "MainWindow.h"


static inline QString toBool(int b) {
    return b ? QStringLiteral("True") : QStringLiteral("False");
}

EditorInspectorDock::EditorInspectorDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(std::make_unique<Ui::EditorInspectorDock>())
{
    ui->setupUi(this);

    auto *positionInfo = new QTreeWidgetItem(ui->treeWidget);
    positionInfo->setText(0, tr("Position Information"));
    positionInfo->setExpanded(true);

    newItem(positionInfo, tr("Current Position"), [](const ScintillaNext *editor) { return QString::number(editor->currentPos()); });
    newItem(positionInfo, tr("Current Position (x, y)"), [](ScintillaNext *editor) { return QString("(%1, %2)").arg(editor->pointXFromPosition(editor->currentPos())).arg(editor->pointYFromPosition(editor->currentPos())); });
    newItem(positionInfo, tr("Column"), [](const ScintillaNext *editor) { return QString::number(editor->column(editor->currentPos())); });
    // SCI_GETCHARAT(position pos) → int
    newItem(positionInfo, tr("Current Style"), [](const ScintillaNext *editor) { return QString::number(editor->styleAt(editor->currentPos())); });
    newItem(positionInfo, tr("Current Line"), [](ScintillaNext *editor) { return QString::number(editor->lineFromPosition(editor->currentPos()) + 1); });
    newItem(positionInfo, tr("Line Length"), [](ScintillaNext *editor) { return QString::number(editor->lineLength(editor->lineFromPosition(editor->currentPos()))); });
    newItem(positionInfo, tr("Line End Position"), [](ScintillaNext *editor) { return QString::number(editor->lineEndPosition(editor->lineFromPosition(editor->currentPos()))); });
    newItem(positionInfo, tr("Line Indentation"), [](ScintillaNext *editor) { return QString::number(editor->lineIndentation(editor->lineFromPosition(editor->currentPos()))); });
    newItem(positionInfo, tr("Line Indent Position"), [](ScintillaNext *editor) { return QString::number(editor->lineIndentPosition(editor->lineFromPosition(editor->currentPos()))); });


    auto *selInfo = new QTreeWidgetItem(ui->treeWidget);
    selInfo->setText(0, tr("Selection Information"));
    selInfo->setExpanded(true);

    newItem(selInfo, tr("Mode"), [](const ScintillaNext *editor) {
        switch (editor->selectionMode()) {
            case SC_SEL_STREAM:
                return QStringLiteral("SC_SEL_STREAM");
            case SC_SEL_RECTANGLE:
                return QStringLiteral("SC_SEL_RECTANGLE");
            case SC_SEL_LINES:
                return QStringLiteral("SC_SEL_LINES");
            case SC_SEL_THIN:
                return QStringLiteral("SC_SEL_THIN");
            default:
                return QStringLiteral("");
        }
    });
    newItem(selInfo, tr("Is Rectangle"), [](const ScintillaNext *editor) { return toBool(editor->selectionIsRectangle()); });
    newItem(selInfo, tr("Selection Empty"), [](const ScintillaNext *editor) { return toBool(editor->selectionEmpty()); });
    newItem(selInfo, tr("Main Selection"), [](const ScintillaNext *editor) { return QString::number(editor->mainSelection()); });
    newItem(selInfo, tr("# of Selections"), [](const ScintillaNext *editor) { return QString::number(editor->selections()); });

    selectionsInfo = new QTreeWidgetItem(selInfo);
    selectionsInfo->setText(0, tr("Multiple Selections"));
    selectionsInfo->setExpanded(true);


    auto *documentInfo = new QTreeWidgetItem(ui->treeWidget);
    documentInfo->setText(0, tr("Document Information"));
    documentInfo->setExpanded(true);

    newItem(documentInfo, tr("Length"), [](const ScintillaNext *editor) { return QString::number(editor->length()); });
    newItem(documentInfo, tr("Line Count"), [](const ScintillaNext *editor) { return QString::number(editor->lineCount()); });


    auto *viewInfo = new QTreeWidgetItem(ui->treeWidget);
    viewInfo->setText(0, tr("View Information"));
    viewInfo->setExpanded(true);

    newItem(viewInfo, tr("Lines on Screen"), [](const ScintillaNext *editor) { return QString::number(editor->linesOnScreen()); });
    newItem(viewInfo, tr("First Visible Line"), [](const ScintillaNext *editor) { return QString::number(editor->firstVisibleLine() + 1); });
    newItem(viewInfo, tr("X Offset"), [](const ScintillaNext *editor) { return QString::number(editor->xOffset()); });


    auto *foldInfo = new QTreeWidgetItem(ui->treeWidget);
    foldInfo->setText(0, tr("Fold Information"));
    foldInfo->setExpanded(true);

    newItem(foldInfo, tr("Visible From Doc Line"), [](ScintillaNext *editor) { return QString::number(editor->visibleFromDocLine(editor->lineFromPosition(editor->currentPos())) + 1); });
    newItem(foldInfo, tr("Doc Line From Visible"), [](ScintillaNext *editor) { return QString::number(editor->docLineFromVisible(editor->lineFromPosition(editor->currentPos())) + 1); });
    newItem(foldInfo, tr("Fold Level"), [](ScintillaNext *editor) { return QString::number((editor->foldLevel(editor->lineFromPosition(editor->currentPos())) & SC_FOLDLEVELNUMBERMASK) - SC_FOLDLEVELBASE); });
    newItem(foldInfo, tr("Is Fold Header"), [](ScintillaNext *editor) { return toBool((editor->foldLevel(editor->lineFromPosition(editor->currentPos())) & SC_FOLDLEVELHEADERFLAG) == SC_FOLDLEVELHEADERFLAG); });
    newItem(foldInfo, tr("Fold Parent"), [](ScintillaNext *editor) { return QString::number(editor->foldParent(editor->lineFromPosition(editor->currentPos())) + 1); });
    newItem(foldInfo, tr("Last Child"), [](ScintillaNext *editor) { return QString::number(editor->lastChild(editor->lineFromPosition(editor->currentPos()), -1) + 1); });
    newItem(foldInfo, tr("Contracted Fold Next"), [](ScintillaNext *editor) { return QString::number(editor->contractedFoldNext(editor->lineFromPosition(editor->currentPos())) + 1); });

    connect(this, &QDockWidget::visibilityChanged, this, [parent, this](bool visible) {
        if (visible) {
            connectToEditor(parent->currentEditor());
            connect(parent, &MainWindow::editorActivated, this, &EditorInspectorDock::connectToEditor);
        }
        else {
            disconnectFromEditor();
            disconnect(parent, &MainWindow::editorActivated, this, &EditorInspectorDock::connectToEditor);
        }
    });
}

EditorInspectorDock::~EditorInspectorDock() = default;

void EditorInspectorDock::connectToEditor(ScintillaNext *editor)
{
    disconnectFromEditor();

    editorConnection = connect(editor, &ScintillaNext::updateUi, this, &EditorInspectorDock::editorUIUpdated);

    updateEditorInfo(editor);
}


void EditorInspectorDock::disconnectFromEditor()
{
    if (editorConnection) {
        disconnect(editorConnection);
    }
}

void EditorInspectorDock::editorUIUpdated(Scintilla::Update updated)
{
    using enum Scintilla::Update;
    if (FlagSet(updated, Content)
            || FlagSet(updated, Selection)
            || FlagSet(updated, VScroll)
            || FlagSet(updated, HScroll)) {
        ScintillaNext *editor = qobject_cast<ScintillaNext*>(sender());

        updateEditorInfo(editor);
    }
}

void EditorInspectorDock::updateEditorInfo(ScintillaNext *editor)
{
    for (const auto &[item, func] : std::as_const(items)) {
        if (item->parent()->isExpanded()) {
            item->setText(1, func(editor));
        }
    }

    qDeleteAll(selectionsInfo->takeChildren());

    for (int i = 0; i < editor->selections(); ++i) {
        auto *selection = new QTreeWidgetItem(selectionsInfo);
        selection->setText(0, QLatin1Char('#') + QString::number(i));
        selection->setExpanded(true);

        auto *caret = new QTreeWidgetItem(selection);
        caret->setText(0, tr("Caret"));
        caret->setText(1, QString::number(editor->selectionNCaret(i)));

        auto *anchor = new QTreeWidgetItem(selection);
        anchor->setText(0, tr("Anchor"));
        anchor->setText(1, QString::number(editor->selectionNAnchor(i)));

        auto *caretVirtual = new QTreeWidgetItem(selection);
        caretVirtual->setText(0, tr("Caret Virtual Space"));
        caretVirtual->setText(1, QString::number(editor->selectionNCaretVirtualSpace(i)));

        auto *anchorVirtual = new QTreeWidgetItem(selection);
        anchorVirtual->setText(0, tr("Anchor Virtual Space"));
        anchorVirtual->setText(1, QString::number(editor->selectionNAnchorVirtualSpace(i)));
    }

    ui->treeWidget->resizeColumnToContents(0);
}

void EditorInspectorDock::newItem(QTreeWidgetItem *parent, const QString &label, EditorFunction func)
{
    auto *item = new QTreeWidgetItem(parent);
    item->setText(0, label);
    items.append(qMakePair(item, func));
}
