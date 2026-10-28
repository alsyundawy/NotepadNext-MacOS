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

#include "MainWindow.h"
#include <QtGlobal>
#include "AboutDialog.h"
#include "SaveModifiedDialog.h"
#include "EncodingSelectionDialog.h"
#include "EncodingDetector.h"
#include "ThemeManager.h"
#include "ThemePaletteData.h"
#include "ThemeSwitcherDialog.h"
#include "BookMarkDecorator.h"
#include "DefaultDirectoryManager.h"
#include "MarkerAppDecorator.h"
#include "ScintillaSorter.h"
#include "URLFinder.h"
#include "XmlFormatter.h"
#include "SessionManager.h"
#include "UndoAction.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QClipboard>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QWindow>
#include <QPushButton>
#include <QTimer>
#include <QInputDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QDir>
#include <QDirIterator>
#include <QProcess>
#include <QScreen>
#include <QDockWidget>
#include <QFontDatabase>
#include <QSysInfo>
#include <functional>
#include <algorithm>
#include <QCollator>
#include <QSystemTrayIcon>

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
#include <QSimpleUpdater.h>
#include "UpdateDialog.h"
#endif
#ifdef Q_OS_MACOS
#include <objc/message.h>
#endif
#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "DockAreaWidget.h"

#include "NotepadNextApplication.h"
#include "ApplicationSettings.h"
#include "LuaState.h"

#include "ScintillaNext.h"

#include "RecentFilesListManager.h"
#include "RecentFilesListMenuBuilder.h"
#include "EditorManager.h"

#include "LuaConsoleDock.h"
#include "LanguageInspectorDock.h"
#include "EditorInspectorDock.h"
#include "FolderAsWorkspaceDock.h"
#include "SearchResultsDock.h"
#include "DebugLogDock.h"
#include "FileListDock.h"

#include "FindReplaceDialog.h"
#include "MacroRunDialog.h"
#include "MacroSaveDialog.h"
#include "PreferencesDialog.h"
#include "ColumnEditorDialog.h"

#include "TabsQuickActionsBar.h"

#include "QuickFindWidget.h"

#include "EditorPrintPreviewRenderer.h"
#include "MacroEditorDialog.h"

#include "ZoomEventWatcher.h"
#include "ShiftMiddleClickBlocker.h"
#include "ShiftWheelToHorizontalScrollFilter.h"

#include "FileDialogHelpers.h"

#include "HtmlConverter.h"
#include "RtfConverter.h"

#include "FadingIndicator.h"

#include "ActionUtils.h"


MainWindow::MainWindow(NotepadNextApplication *app) : // NOSONAR
    ui(std::make_unique<Ui::MainWindow>()),
    app(app),
    zoomEventWatcher(new ZoomEventWatcher(this)),
    shiftMiddleClickBlocker(new ShiftMiddleClickBlocker(this)),
    shiftWheelToHorizontalScrollFilter(new ShiftWheelToHorizontalScrollFilter(this))
{
    qInfo(Q_FUNC_INFO);

    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    applyCustomShortcuts();

    qInfo("setupUi Completed");

    defaultDirectoryManager = new DefaultDirectoryManager(this, app->getSettings());

    connect(this, &MainWindow::aboutToClose, this, &MainWindow::saveSettings);

    // Create and set up the connections to the docked editor
    dockedEditor = new DockedEditor(this);
    connect(dockedEditor, &DockedEditor::editorCloseRequested, this, &MainWindow::closeFile);
    connect(dockedEditor, &DockedEditor::editorActivated, this, &MainWindow::activateEditor);
    connect(dockedEditor, &DockedEditor::contextMenuRequestedForEditor, this, &MainWindow::tabBarRightClicked);
    connect(dockedEditor, &DockedEditor::titleBarDoubleClicked, this, &MainWindow::newFile);

    // React immediately (instead of only on tab-switch/window-focus) when an
    // open file is changed by another program - but only if NotepadNext is
    // actually the foreground window and the affected editor is the one
    // currently visible, matching Notepad++'s behavior. Otherwise, do
    // nothing here: the existing activateEditor() (tab switch) and
    // focusIn() (window regains focus) code paths already re-check the
    // file's timestamp and will surface the dialog at the right time.
    connect(app->getEditorManager(), &EditorManager::editorFileChangedOnDisk, this, [this](ScintillaNext *editor) {
        if (!isActiveWindow()) {
            return;
        }

        if (editor != currentEditor()) {
            return;
        }

        if (checkFileForModification(editor)) {
            updateGui(currentEditor());
        }
    });

    // Set up the menus
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionNewWindow, &QAction::triggered, this, &MainWindow::newWindow);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFileDialog);
    connect(ui->actionReload, &QAction::triggered, this, &MainWindow::reloadFile);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeCurrentFile);
    connect(ui->actionCloseAll, &QAction::triggered, this, &MainWindow::closeAllFiles);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    // Split editor actions
    connect(ui->actionSplitHorizontal, &QAction::triggered, this, [this]() {
        newFile();
        ScintillaNext *newEditor = currentEditor();
        if (newEditor) {
            dockedEditor->splitToRight(newEditor);
        }
    });
    connect(ui->actionSplitVertical, &QAction::triggered, this, [this]() {
        newFile();
        ScintillaNext *newEditor = currentEditor();
        if (newEditor) {
            dockedEditor->splitToBottom(newEditor);
        }
    });

#ifdef Q_OS_WIN
    ui->actionExit->setShortcut(QKeySequence("Alt+F4"));
#else
    ui->actionExit->setShortcut(QKeySequence::Quit);
#endif

    connect(ui->actionOpenFolderasWorkspace, &QAction::triggered, this, &MainWindow::openFolderAsWorkspaceDialog);

    connect(ui->actionCloseAllExceptActive, &QAction::triggered, this, &MainWindow::closeAllExceptActive);
    connect(ui->actionCloseAllToLeft, &QAction::triggered, this, &MainWindow::closeAllToLeft);
    connect(ui->actionCloseAllToRight, &QAction::triggered, this, &MainWindow::closeAllToRight);

    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveCurrentFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveCurrentFileAsDialog);
    connect(ui->actionSaveCopyAs, &QAction::triggered, this, &MainWindow::saveCopyAsDialog);
    connect(ui->actionSaveAll, &QAction::triggered, this, &MainWindow::saveAll);
    connect(ui->actionRename, &QAction::triggered, this, &MainWindow::renameFile);

    connect(ui->actionExportHtml, &QAction::triggered, this, [this]() {
        HtmlConverter html(currentEditor());
        exportAsFormat(&html, QStringLiteral("HTML files (*.html)"));
    });

    connect(ui->actionExportRtf, &QAction::triggered, this, [this]() {
        RtfConverter rtf(currentEditor());
        exportAsFormat(&rtf, QStringLiteral("RTF Files (*.rtf)"));
    });

    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::print);

    connectEditorAction(ui->actionToggleSingleLineComment, &ScintillaNext::toggleCommentSelection);
    connectEditorAction(ui->actionSingleLineComment, &ScintillaNext::commentLineSelection);
    connectEditorAction(ui->actionSingleLineUncomment, &ScintillaNext::uncommentLineSelection);

    connect(ui->actionBase64Encode, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        const QByteArray selection = editor->getSelText();
        editor->replaceSel(selection.toBase64().constData());
    });
    connect(ui->actionURLEncode, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        const QByteArray selection = editor->getSelText();
        editor->replaceSel(selection.toPercentEncoding().constData());
    });
    connect(ui->actionBase64Decode, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        const QByteArray selection = editor->getSelText();
        if (auto result = QByteArray::fromBase64Encoding(selection)) {
            editor->replaceSel((*result).constData());
        }
    });
    connect(ui->actionURLDecode,&QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        const QByteArray selection = editor->getSelText();
        editor->replaceSel(QByteArray::fromPercentEncoding(selection).constData());
    });
    connect(ui->actionXmlPrettyPrint, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        if (!editor) {
            return;
        }

        const auto len = static_cast<int>(editor->length());
        const QByteArray input = editor->get_text_range(0, len);

        QByteArray output;
        if (QString errorString; !XmlFormatter::format(input, output, errorString)) {
            QMessageBox::warning(this, tr("Pretty Print XML"), errorString);
            return;
        }

        const UndoAction ua(editor);
        editor->targetWholeDocument();
        editor->replaceTarget(output.length(), output.constData());
    });
    connect(ui->actionJsonFormat, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        if (!editor) {
            return;
        }

        const auto len = static_cast<int>(editor->length());
        const QByteArray input = editor->get_text_range(0, len);

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(input, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            QMessageBox::warning(this, tr("Format JSON"), tr("JSON Error: %1 (offset %2)").arg(parseError.errorString()).arg(parseError.offset));
            return;
        }

        QByteArray output = doc.toJson(QJsonDocument::Indented);
        const UndoAction ua(editor);
        editor->targetWholeDocument();
        editor->replaceTarget(output.length(), output.constData());
    });
    connect(ui->actionJsonMinify, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        if (!editor) {
            return;
        }

        const auto len = static_cast<int>(editor->length());
        const QByteArray input = editor->get_text_range(0, len);

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(input, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            QMessageBox::warning(this, tr("Minify JSON"), tr("JSON Error: %1 (offset %2)").arg(parseError.errorString()).arg(parseError.offset));
            return;
        }

        QByteArray output = doc.toJson(QJsonDocument::Compact);
        const UndoAction ua(editor);
        editor->targetWholeDocument();
        editor->replaceTarget(output.length(), output.constData());
    });
    connectEditorAction(ui->actionConvertTabsToSpaces, &ScintillaNext::convertTabsToSpaces);
    connectEditorAction(ui->actionConvertSpacesToTabs, &ScintillaNext::convertSpacesToTabs);
    connect(ui->actionCopyURL, &QAction::triggered, this, [this]() {
        const ScintillaNext *editor = currentEditor();
        if (!editor) {
            return;
        }
        const URLFinder *urlFinder = editor->findChild<URLFinder *>(QString(), Qt::FindDirectChildrenOnly);
        if (urlFinder && urlFinder->isEnabled()) {
            urlFinder->copyURLToClipboard(contextMenuPos);
        }
    });

#ifdef Q_OS_MACOS
    auto *actionEmojiSymbols = ui->menuEdit->addAction(tr("Emoji & Symbols"));
    actionEmojiSymbols->setShortcut(QKeySequence(Qt::META | Qt::CTRL | Qt::Key_Space));
    connect(actionEmojiSymbols, &QAction::triggered, this, []() {
        // [NSApp orderFrontCharacterPalette:nil];
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        auto msgSendFn = reinterpret_cast<id (*)(id, SEL, ...)>(objc_msgSend); // NOSONAR
        id app = msgSendFn(reinterpret_cast<id>(objc_getClass("NSApplication")), sel_registerName("sharedApplication")); // NOSONAR
        msgSendFn(app, sel_registerName("orderFrontCharacterPalette:"), nullptr);
    });
#endif

    connect(ui->actionClearRecentFilesList, &QAction::triggered, app->getRecentFilesListManager(), &RecentFilesListManager::clear);

    connect(ui->actionMoveToTrash, &QAction::triggered, this, &MainWindow::moveCurrentFileToTrash);

    auto *recentFileListMenuBuilder = new RecentFilesListMenuBuilder(app->getRecentFilesListManager());
    connect(ui->menuRecentFiles, &QMenu::aboutToShow, this, [this, recentFileListMenuBuilder]() {
        // NOTE: its unfortunate that this has to be hard coded, but there's no way
        // to easily determine what should or shouldn't be there
        while (ui->menuRecentFiles->actions().size() > 4) {
            if (auto *act = ui->menuRecentFiles->actions().takeLast()) {
                act->deleteLater();
            }
        }

        recentFileListMenuBuilder->populateMenu(ui->menuRecentFiles);
    });

    connect(ui->actionRestoreRecentlyClosedFile, &QAction::triggered, this, [this]() {
        if (this->app->getRecentFilesListManager()->count() > 0) {
            openFileList(QStringList() << this->app->getRecentFilesListManager()->mostRecentFile());
        }
    });

    connect(ui->actionOpenAllRecentFiles, &QAction::triggered, this, [this]() {
        openFileList(this->app->getRecentFilesListManager()->fileList());
    });

    connect(recentFileListMenuBuilder, &RecentFilesListMenuBuilder::fileOpenRequest, this, &MainWindow::openFile);

    auto *eolActionGroup = new QActionGroup(this);
    eolActionGroup->addAction(ui->actionWindows);
    eolActionGroup->addAction(ui->actionUnix);
    eolActionGroup->addAction(ui->actionMacintosh);

    ui->actionWindows->setData(SC_EOL_CRLF);
    ui->actionUnix->setData(SC_EOL_LF);
    ui->actionMacintosh->setData(SC_EOL_CR);

    auto handleEolTrigger = [this]() {
        // qobject_cast lets us look at which specific action was clicked
        if (const auto* action = qobject_cast<const QAction*>(sender())) {
            int eolMode = action->data().toInt();
            convertEOLs(eolMode);
        }
    };

    // Connect all three to the same handler
    connect(ui->actionWindows,   &QAction::triggered, this, handleEolTrigger);
    connect(ui->actionUnix,      &QAction::triggered, this, handleEolTrigger);
    connect(ui->actionMacintosh, &QAction::triggered, this, handleEolTrigger);


    connectEditorAction(ui->actionUpperCase, &ScintillaNext::upperCase);
    connectEditorAction(ui->actionLowerCase, &ScintillaNext::lowerCase);
    connect(ui->actionTitleCase, &QAction::triggered, this, &MainWindow::titleCase);
    connect(ui->actionSentenceCase, &QAction::triggered, this, &MainWindow::sentenceCase);
    connect(ui->actionInvertCase, &QAction::triggered, this, &MainWindow::invertCase);

    connectEditorAction(ui->actionDuplicateCurrentLine, &ScintillaNext::lineDuplicate);
    connectEditorAction(ui->actionMoveSelectedLinesUp, &ScintillaNext::moveSelectedLinesUp);
    connectEditorAction(ui->actionMoveSelectedLinesDown, &ScintillaNext::moveSelectedLinesDown);

    connect(ui->actionSplitLines, &QAction::triggered, this, [this]() {
        currentEditor()->targetFromSelection();
        currentEditor()->linesSplit(0);
    });

    connect(ui->actionJoinLines, &QAction::triggered, this, [this]()  {
        currentEditor()->targetFromSelection();
        currentEditor()->linesJoin();
    });

    connect(ui->actionRemoveEmptyLines, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        Finder f(editor);
        const UndoAction ua(editor);

        f.options().text = QStringLiteral("\\R\\R+");
        f.options().flags = Scintilla::FindOption::RegExp;
        f.replaceAll(editor->eolString());

        // The regex will not entirely remove a blank first line
        editor->deleteLeadingEmptyLines();

        // Regex will also not delete the final blank line
        editor->deleteTrailingEmptyLines();
    });

    connectEditorAction(ui->actionTrimTrailingSpace, &ScintillaNext::trimTrailingWhitespace);
    connectEditorAction(ui->actionRemoveDuplicateLines, &ScintillaNext::removeDuplicateLines);
    connectEditorAction(ui->actionRemoveConsecutiveDuplicateLines, &ScintillaNext::removeConsecutiveDuplicateLines);

    connect(ui->actionReverseLineOrder, &QAction::triggered, this, [this]() {
        ScintillaSorter scintillaSorter(currentEditor());
        scintillaSorter.sort(ReverseSorter(Sorter::Direction::Ascending));
    });
    connect(ui->actionSortLinesAsc, &QAction::triggered, this, [this]() {
        ScintillaSorter scintillaSorter(currentEditor());
        scintillaSorter.sort(CaseSensitiveSorter(Sorter::Direction::Ascending));
    });
    connect(ui->actionSortLinesAscCaseInsensitive, &QAction::triggered, this, [this]() {
        ScintillaSorter scintillaSorter(currentEditor());
        scintillaSorter.sort(CaseInsensitiveSorter(Sorter::Direction::Ascending));
    });
    connect(ui->actionSortLinesbyLengthAsc, &QAction::triggered, this, [this]() {
        ScintillaSorter scintillaSorter(currentEditor());
        scintillaSorter.sort(LineLengthSorter(Sorter::Direction::Ascending));
    });
    connect(ui->actionSortLinesDesc, &QAction::triggered, this, [this]() {
        ScintillaSorter scintillaSorter(currentEditor());
        scintillaSorter.sort(CaseSensitiveSorter(Sorter::Direction::Descending));
    });
    connect(ui->actionSortLinesDescCaseInsensitive, &QAction::triggered, this, [this]() {
        ScintillaSorter scintillaSorter(currentEditor());
        scintillaSorter.sort(CaseInsensitiveSorter(Sorter::Direction::Descending));
    });
    connect(ui->actionSortLinesbyLengthDesc, &QAction::triggered, this, [this]() {
        ScintillaSorter scintillaSorter(currentEditor());
        scintillaSorter.sort(LineLengthSorter(Sorter::Direction::Descending));
    });
    connect(ui->actionSortLinesNaturalAsc, &QAction::triggered, this, [this]() {
        sortLinesNatural(true, true);
    });
    connect(ui->actionSortLinesNaturalDesc, &QAction::triggered, this, [this]() {
        sortLinesNatural(false, true);
    });
    connect(ui->actionEvaluateMath, &QAction::triggered, this, &MainWindow::evaluateMathExpression);
    connect(ui->actionCompareActiveFiles, &QAction::triggered, this, &MainWindow::compareOpenBuffers);
    connect(ui->actionToggleHexView, &QAction::triggered, this, &MainWindow::toggleHexView);

    connect(ui->actionColumnMode, &QAction::triggered, this, [this]() {
        auto *columnEditor = findChild<ColumnEditorDialog *>(QString(), Qt::FindDirectChildrenOnly);

        if (columnEditor == Q_NULLPTR) {
            columnEditor = new ColumnEditorDialog(this);
        }

        columnEditor->show();
        columnEditor->raise();
        columnEditor->activateWindow();
    });

    connectEditorAction(ui->actionUndo, &ScintillaNext::undo);
    connectEditorAction(ui->actionRedo, &ScintillaNext::redo);
    connectEditorAction(ui->actionCut, &ScintillaNext::cutAllowLine);
    connectEditorAction(ui->actionCopy, &ScintillaNext::copyAllowLine);
    connectEditorAction(ui->actionDelete, &ScintillaNext::clear);
    connectEditorAction(ui->actionPaste, &ScintillaNext::paste);
    connect(ui->actionPasteWithoutFormat, &QAction::triggered, this, &MainWindow::pasteWithoutFormat);
    connectEditorAction(ui->actionSelectAll, &ScintillaNext::selectAll);
    connect(ui->actionSelectNext, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();

        editor->setSearchFlags(SCFIND_NONE);
        editor->targetWholeDocument();
        editor->multipleSelectAddNext();
    });
    connect(ui->actionCopyFullPath, &QAction::triggered, this, [this]() {
        auto editor = currentEditor();
        if (editor->isFile()) {
            QApplication::clipboard()->setText(editor->getFilePath());
        }
    });
    connect(ui->actionCopyFileName, &QAction::triggered, this, [this]() {
        QApplication::clipboard()->setText(currentEditor()->getName());
    });
    connect(ui->actionCopyFileDirectory, &QAction::triggered, this, [this]() {
        auto editor = currentEditor();
        if (editor->isFile()) {
            QApplication::clipboard()->setText(editor->getPath());
        }
    });

    connect(ui->actionCopyAsHtml, &QAction::triggered, this, [this]() {
        HtmlConverter html(currentEditor());
        copyAsFormat(&html, "text/html");
    });

    connect(ui->actionCopyAsRtf, &QAction::triggered, this, [this]() {
        RtfConverter rtf(currentEditor());
        copyAsFormat(&rtf, "Rich Text Format");
    });

    connectEditorAction(ui->actionIncreaseIndent, &ScintillaNext::tab);
    connectEditorAction(ui->actionDecreaseIndent, &ScintillaNext::backTab);

    addAction(ui->actionToggleOverType);
    connect(ui->actionToggleOverType, &QAction::triggered, this, [this]() {
        currentEditor()->editToggleOvertype();
        ui->statusBar->refresh(currentEditor());
    });

    auto *srDock = new SearchResultsDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, srDock);
    srDock->toggleViewAction()->setShortcut(Qt::Key_F7);
    ui->menuView->addAction(srDock->toggleViewAction());

    connect(srDock, &SearchResultsDock::searchResultActivated, this, [this](ScintillaNext *editor, int lineNumber, int startPositionFromBeginning, int endPositionFromBeginning) { // NOLINT(bugprone-easily-swappable-parameters)
        dockedEditor->switchToEditor(editor);

        const auto linePos = static_cast<int>(editor->positionFromLine(lineNumber));
        editor->goToRange({linePos + startPositionFromBeginning, linePos + endPositionFromBeginning});
        editor->verticalCentreCaret();

        editor->grabFocus();
    });

    connect(ui->actionFind, &QAction::triggered, this, [this]() {
        showFindReplaceDialog(FindReplaceDialog::FIND_TAB);
    });

    connect(ui->actionFindNext, &QAction::triggered, this, [this]() {
        auto *f = findChild<FindReplaceDialog *>(QString(), Qt::FindDirectChildrenOnly);

        if (f) {
            f->performNextSearch();
        }
    });

    connect(ui->actionFindPrevious, &QAction::triggered, this, [this]() {
        auto *f = findChild<FindReplaceDialog *>(QString(), Qt::FindDirectChildrenOnly);

        if (f) {
            f->performPrevSearch();
        }
    });

    auto selectAndFind = [this](bool forward) {
        auto editor = currentEditor();
        auto range = editor->getContextText();

        if (range.cpMin == range.cpMax) {
            return;
        }

        auto text = editor->get_text_range(static_cast<int>(range.cpMin), static_cast<int>(range.cpMax));

        Finder f(editor);
        f.options().text = QString::fromUtf8(text);
        f.options().wrapAround = true;

        FindResult result = forward ? f.findNext() : f.findPrev();

        if (result) {
            editor->goToRange(result.range);
        }
    };

    connect(ui->actionSelectandFindNext, &QAction::triggered, this, [selectAndFind]() {
        selectAndFind(true);
    });

    connect(ui->actionSelectandFindPrevious, &QAction::triggered, this, [selectAndFind]() {
        selectAndFind(false);
    });

    connect(ui->actionQuickFind, &QAction::triggered, this, [this]() {
        auto *quickFind = findChild<QuickFindWidget *>(QString(), Qt::FindDirectChildrenOnly);

        if (quickFind == Q_NULLPTR) {
            quickFind = new QuickFindWidget(this);
        }

        quickFind->setEditor(currentEditor());
        quickFind->setFocus();
        quickFind->show();
    });

    connect(ui->actionReplace, &QAction::triggered, this, [this]() {
        showFindReplaceDialog(FindReplaceDialog::REPLACE_TAB);
    });

    connect(ui->actionSearchAndBookmark, &QAction::triggered, this, [this]() {
        showFindReplaceDialog(FindReplaceDialog::MARK_TAB);
    });

    connect(ui->actionGoToLine, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        const auto currentLine = static_cast<int>(editor->lineFromPosition(editor->currentPos())) + 1;
        const auto maxLine = static_cast<int>(editor->lineCount());
        bool ok;

        auto d = QInputDialog(this);
        Qt::WindowFlags flags = d.windowFlags() & ~Qt::WindowContextHelpButtonHint;
        int lineToGoTo = QInputDialog::getInt(this, tr("Go to line"), tr("Line Number (1 - %1)").arg(maxLine), currentLine, 1, maxLine, 1, &ok, flags);

        if (ok) {
            editor->ensureVisible(lineToGoTo - 1);
            editor->gotoLine(lineToGoTo - 1);
            editor->verticalCentreCaret();
        }
    });

    // Style all actions that have a MarkerNumber and interpret that as the color needed
    const auto *markerAppDecorator = app->findChild<MarkerAppDecorator*>(QString(), Qt::FindDirectChildrenOnly);
    for (auto *action : findChildren<QAction*>()) {
        if (action->property("MarkerNumber").isValid()) {
            int markerNumber = action->property("MarkerNumber").toInt();
            action->setIcon(ActionUtils::createSolidIcon(markerAppDecorator->markerColor(markerNumber)));
        }
    }

    auto mark_callback = [this]() {
        auto *decorator = this->app->findChild<MarkerAppDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (decorator && decorator->isEnabled()) {
            const QObject *snd = sender();
            if (snd && snd->property("MarkerNumber").isValid()) {
                ScintillaNext *editor = currentEditor();
                decorator->mark(editor, snd->property("MarkerNumber").toInt());
            }
        }
    };

    auto clear_mark_callback = [this]() {
        auto *decorator = this->app->findChild<MarkerAppDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (decorator && decorator->isEnabled()) {
            const QObject *snd = sender();
            if (snd && snd->property("MarkerNumber").isValid()) {
                ScintillaNext *editor = currentEditor();
                decorator->clear(editor, snd->property("MarkerNumber").toInt());
            }
        }
    };

    connect(ui->actionMarkStyle1, &QAction::triggered, this, mark_callback);
    connect(ui->actionMarkStyle2, &QAction::triggered, this, mark_callback);
    connect(ui->actionMarkStyle3, &QAction::triggered, this, mark_callback);

    connect(ui->actionClearStyle1, &QAction::triggered, this, clear_mark_callback);
    connect(ui->actionClearStyle2, &QAction::triggered, this, clear_mark_callback);
    connect(ui->actionClearStyle3, &QAction::triggered, this, clear_mark_callback);

    connect(ui->actionClearAllStyles, &QAction::triggered, this, [this]() {
        auto *clearDecorator = this->app->findChild<MarkerAppDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (clearDecorator && clearDecorator->isEnabled()) {
            clearDecorator->clearAll(currentEditor());
        }
    });

    connect(ui->actionToggleBookmark, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        auto *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            editor->forEachLineInSelection(static_cast<int>(editor->mainSelection()), [bookMarkDecorator](int line) {
                bookMarkDecorator->toggleBookmark(line);
            });
        }
    });

    connect(ui->actionNextBookmark, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        auto *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            const auto currentLine = static_cast<int>(editor->lineFromPosition(editor->currentPos()));
            int nextBookmarkedLine = bookMarkDecorator->nextBookmarkAfter(currentLine + 1);

            if (nextBookmarkedLine != -1) {
                editor->ensureVisibleEnforcePolicy(nextBookmarkedLine);
                editor->gotoLine(nextBookmarkedLine);
            }
        }
    });

    connect(ui->actionClearBookmarks, &QAction::triggered, this, [this]() {
        const ScintillaNext *editor = currentEditor();
        auto *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            bookMarkDecorator->clearAllBookmarks();
        }
    });

    connect(ui->actionInvertBookmarks, &QAction::triggered, this, [this]() {
        const ScintillaNext *editor = currentEditor();
        auto *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            for (int line = 0; line < editor->lineCount(); line++) {
                bookMarkDecorator->toggleBookmark(line);
            }
        }
    });

    connect(ui->actionPreviousBookmark, &QAction::triggered, this, [this]() {
        ScintillaNext *editor = currentEditor();
        auto *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            const auto currentLine = static_cast<int>(editor->lineFromPosition(editor->currentPos()));
            int prevBookmarkedLine = bookMarkDecorator->previousBookMarkBefore(currentLine - 1);

            if (prevBookmarkedLine != -1) {
                editor->ensureVisibleEnforcePolicy(prevBookmarkedLine);
                editor->gotoLine(prevBookmarkedLine);
            }
        }
    });

    connect(ui->actionCutBookmarkedLines, &QAction::triggered, this, [this]() {
        const ScintillaNext *editor = currentEditor();
        auto *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            QString s = bookMarkDecorator->cutBookMarkedLines();

            if (!s.isEmpty()) {
                QApplication::clipboard()->setText(s);
            }
        }
    });

    connect(ui->actionCopyBookmarkedLines, &QAction::triggered, this, [this]() {
        const ScintillaNext *editor = currentEditor();
        auto *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            QString s = bookMarkDecorator->copyBookMarkedLines();

            if (!s.isEmpty()) {
                QApplication::clipboard()->setText(s);
            }
        }
    });

    connect(ui->actionDeleteBookmarkedLines, &QAction::triggered, this, [this]() {
        const ScintillaNext *editor = currentEditor();
        auto *bookMarkDecorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);

        if (bookMarkDecorator && bookMarkDecorator->isEnabled()) {
            bookMarkDecorator->deleteBookMarkedLines();
        }
    });


    // The action needs added to the window so it can be triggered via the keyboard
    addAction(ui->actionNextTab);
    ui->actionNextTab->setShortcuts(ui->actionNextTab->shortcuts() << QKeySequence(Qt::CTRL | Qt::Key_PageDown));
    connect(ui->actionNextTab, &QAction::triggered, this, [this]() {
        int index = dockedEditor->currentDockArea()->currentIndex();
        int total = dockedEditor->currentDockArea()->dockWidgetsCount();

        index++;
        dockedEditor->currentDockArea()->setCurrentIndex(index < total ? index : 0);
    });

    // The action needs added to the window so it can be triggered via the keyboard
    addAction(ui->actionPreviousTab);
    ui->actionPreviousTab->setShortcuts(ui->actionPreviousTab->shortcuts() << QKeySequence(Qt::CTRL | Qt::Key_PageUp));
    connect(ui->actionPreviousTab, &QAction::triggered, this, [this]() {
        int index = dockedEditor->currentDockArea()->currentIndex();
        int total = dockedEditor->currentDockArea()->dockWidgetsCount();

        index--;
        dockedEditor->currentDockArea()->setCurrentIndex(index >= 0 ? index : total - 1);
    });

    ui->pushExitFullScreen->setParent(this); // This is important
    ui->pushExitFullScreen->setVisible(false);
    connect(ui->pushExitFullScreen, &QPushButton::clicked, ui->actionFullScreen, &QAction::trigger);
    auto enterFullScreen = [this](bool wasMaximized) {
        ui->menuBar->setMaximumHeight(0);
        ui->mainToolBar->setMaximumHeight(0);
        if (wasMaximized) {
            setGeometry(screen()->availableGeometry());
        }
        showFullScreen();
        ui->pushExitFullScreen->setGeometry(width() - 20, 0, 20, 20);
        ui->pushExitFullScreen->show();
        ui->pushExitFullScreen->raise();
    };

    auto exitFullScreen = [this](bool wasMaximized) {
        ui->menuBar->setMaximumHeight(QWIDGETSIZE_MAX);
        ui->mainToolBar->setMaximumHeight(QWIDGETSIZE_MAX);
        if (wasMaximized) {
            showMaximized();
        } else {
            showNormal();
        }
        ui->pushExitFullScreen->hide();
    };

    connect(ui->actionFullScreen, &QAction::triggered, this, [this, enterFullScreen, exitFullScreen](bool b) {
        static bool wasMaximized;
        if (b) {
            wasMaximized = isMaximized();
            enterFullScreen(wasMaximized);
        } else {
            exitFullScreen(wasMaximized);
        }
    });


    // Show All Characters is just a short cut to toggle whitespace and EOL on
    ui->actionShowAllCharacters->setChecked(app->getSettings()->showWhitespace() && app->getSettings()->showEndOfLine());
    connect(ui->actionShowAllCharacters, &QAction::triggered, app->getSettings(), &ApplicationSettings::setShowWhitespace);
    connect(ui->actionShowAllCharacters, &QAction::triggered, app->getSettings(), &ApplicationSettings::setShowEndOfLine);

    // Show White Space
    ui->actionShowWhitespace->setChecked(app->getSettings()->showWhitespace());
    connect(app->getSettings(), &ApplicationSettings::showWhitespaceChanged, ui->actionShowWhitespace, &QAction::setChecked);
    connect(ui->actionShowWhitespace, &QAction::toggled, app->getSettings(), &ApplicationSettings::setShowWhitespace);
    // Update the "Show All Character" action
    connect(ui->actionShowWhitespace, &QAction::toggled, this, [this](bool b) {
        ui->actionShowAllCharacters->setChecked(b && ui->actionShowEndofLine->isChecked());
    });

    // Show EOL
    ui->actionShowEndofLine->setChecked(app->getSettings()->showEndOfLine());
    connect(app->getSettings(), &ApplicationSettings::showEndOfLineChanged, ui->actionShowEndofLine, &QAction::setChecked);
    connect(ui->actionShowEndofLine, &QAction::toggled, app->getSettings(), &ApplicationSettings::setShowEndOfLine);
    // Update the "Show All Character" action
    connect(ui->actionShowEndofLine, &QAction::toggled, this, [this](bool b) {
        ui->actionShowAllCharacters->setChecked(b && ui->actionShowWhitespace->isChecked());
    });

    // Show Wrap Symbol
    ui->actionShowWrapSymbol->setChecked(app->getSettings()->showWrapSymbol());
    connect(app->getSettings(), &ApplicationSettings::showWrapSymbolChanged, ui->actionShowWrapSymbol, &QAction::setChecked);
    connect(ui->actionShowWrapSymbol, &QAction::toggled, app->getSettings(), &ApplicationSettings::setShowWrapSymbol);

    // Show Indentation Guide
    ui->actionShowIndentGuide->setChecked(app->getSettings()->showIndentGuide());
    connect(app->getSettings(), &ApplicationSettings::showIndentGuideChanged, ui->actionShowIndentGuide, &QAction::setChecked);
    connect(ui->actionShowIndentGuide, &QAction::toggled, app->getSettings(), &ApplicationSettings::setShowIndentGuide);

    // Word Wrap
    ui->actionWordWrap->setChecked(app->getSettings()->wordWrap());
    connect(app->getSettings(), &ApplicationSettings::wordWrapChanged, ui->actionWordWrap, &QAction::setChecked);
    connect(ui->actionWordWrap, &QAction::toggled, app->getSettings(), &ApplicationSettings::setWordWrap);

    // Zooming controls all editors simulaneously
    connect(ui->actionZoomIn, &QAction::triggered, this, [this]() {
        for (ScintillaNext *editor : editors()) {
            editor->zoomIn();
        }
        zoomLevel = static_cast<int>(currentEditor()->zoom());

        showEditorZoomLevelIndicator();
    });
    connect(ui->actionZoomOut, &QAction::triggered, this, [this]() {
        // Scintilla can zoom out to "-10" but on a screen with fractional scaling it throws a lot of Qt warnings
        if (zoomLevel == -9) {
            return;
        }

        for (ScintillaNext *editor : editors()) {
            editor->zoomOut();
        }
        zoomLevel = static_cast<int>(currentEditor()->zoom());

        showEditorZoomLevelIndicator();
    });
    connect(ui->actionZoomReset, &QAction::triggered, this, [this]() {
        for (ScintillaNext *editor : editors()) {
            editor->setZoom(0);
        }
        zoomLevel = 0;

        showEditorZoomLevelIndicator();
    });

    // Zoom watcher has detected a zoom event, so just trigger the UI action
    connect(zoomEventWatcher, &ZoomEventWatcher::zoomIn, ui->actionZoomIn, &QAction::trigger);
    connect(zoomEventWatcher, &ZoomEventWatcher::zoomOut, ui->actionZoomOut, &QAction::trigger);

    connectEditorAction(ui->actionFoldAll, &ScintillaNext::foldAll, static_cast<int>(static_cast<unsigned int>(SC_FOLDACTION_CONTRACT) | static_cast<unsigned int>(SC_FOLDACTION_CONTRACT_EVERY_LEVEL)));
    connectEditorAction(ui->actionUnfoldAll, &ScintillaNext::foldAll, static_cast<int>(static_cast<unsigned int>(SC_FOLDACTION_EXPAND) | static_cast<unsigned int>(SC_FOLDACTION_CONTRACT_EVERY_LEVEL)));

    connectEditorAction(ui->actionFoldLevel1, &ScintillaNext::foldAllLevels, 0);
    connectEditorAction(ui->actionFoldLevel2, &ScintillaNext::foldAllLevels, 1);
    connectEditorAction(ui->actionFoldLevel3, &ScintillaNext::foldAllLevels, 2);
    connectEditorAction(ui->actionFoldLevel4, &ScintillaNext::foldAllLevels, 3);
    connectEditorAction(ui->actionFoldLevel5, &ScintillaNext::foldAllLevels, 4);
    connectEditorAction(ui->actionFoldLevel6, &ScintillaNext::foldAllLevels, 5);
    connectEditorAction(ui->actionFoldLevel7, &ScintillaNext::foldAllLevels, 6);
    connectEditorAction(ui->actionFoldLevel8, &ScintillaNext::foldAllLevels, 7);
    connectEditorAction(ui->actionFoldLevel9, &ScintillaNext::foldAllLevels, 8);

    connectEditorAction(ui->actionUnfoldLevel1, &ScintillaNext::unFoldAllLevels, 0);
    connectEditorAction(ui->actionUnfoldLevel2, &ScintillaNext::unFoldAllLevels, 1);
    connectEditorAction(ui->actionUnfoldLevel3, &ScintillaNext::unFoldAllLevels, 2);
    connectEditorAction(ui->actionUnfoldLevel4, &ScintillaNext::unFoldAllLevels, 3);
    connectEditorAction(ui->actionUnfoldLevel5, &ScintillaNext::unFoldAllLevels, 4);
    connectEditorAction(ui->actionUnfoldLevel6, &ScintillaNext::unFoldAllLevels, 5);
    connectEditorAction(ui->actionUnfoldLevel7, &ScintillaNext::unFoldAllLevels, 6);
    connectEditorAction(ui->actionUnfoldLevel8, &ScintillaNext::unFoldAllLevels, 7);
    connectEditorAction(ui->actionUnfoldLevel9, &ScintillaNext::unFoldAllLevels, 8);

    languageActionGroup = new QActionGroup(this);
    languageActionGroup->setExclusive(true);

    connect(ui->actionPreferences, &QAction::triggered, this, [this] {
        auto *pd = findChild<PreferencesDialog *>(QString(), Qt::FindDirectChildrenOnly);

        if (pd == Q_NULLPTR) {
            pd = new PreferencesDialog(this->app->getSettings(), this);
        }

        pd->show();
        pd->raise();
        pd->activateWindow();
    });

    // The macro manager has already loaded any saved macros, so it might have some already
    ui->actionRunMacroMultipleTimes->setEnabled(!macroManager.availableMacros().isEmpty());
    ui->actionEditMacros->setEnabled(!macroManager.availableMacros().isEmpty());

    connect(ui->actionMacroRecording, &QAction::triggered, this, [this](bool b) {
        if (b) {
            macroManager.startRecording(currentEditor());
        }
        else {
            macroManager.stopRecording();
        }
    });

    connect(&macroManager, &MacroManager::recordingStarted, this, [this]() {
        ui->actionMacroRecording->setText(tr("Stop Recording"));

        // A macro is being recorded so disable some macro options
        ui->actionPlayback->setEnabled(false);
        ui->actionRunMacroMultipleTimes->setEnabled(false);
        ui->actionSaveCurrentRecordedMacro->setEnabled(false);
    });

    connect(&macroManager, &MacroManager::recordingStopped, this, [this]() {
        ui->actionMacroRecording->setText(tr("Start Recording"));

        // Only enable these if the macro manager recorded a valid macro
        ui->actionPlayback->setEnabled(macroManager.hasCurrentUnsavedMacro());
        ui->actionSaveCurrentRecordedMacro->setEnabled(macroManager.hasCurrentUnsavedMacro());

        // The macro manager might have other macros
        ui->actionRunMacroMultipleTimes->setEnabled(!macroManager.availableMacros().isEmpty() || macroManager.hasCurrentUnsavedMacro());
    });

    connect(ui->actionPlayback, &QAction::triggered, this, [this]() {
        macroManager.replayCurrentMacro(currentEditor());
    });

    connect(ui->actionSaveCurrentRecordedMacro, &QAction::triggered, this, [this]() {
        MacroSaveDialog macroSaveDialog;

        macroSaveDialog.show();
        macroSaveDialog.raise();
        macroSaveDialog.activateWindow();

        if (macroSaveDialog.exec() == QDialog::Accepted) {
            // We have at least 1 saved macro at this point
            ui->actionEditMacros->setEnabled(true);

            // The macro has been saved so disable save option
            ui->actionSaveCurrentRecordedMacro->setEnabled(false);

            // Note: Macro persistence overwrites matching macro name or appends if unique
            macroManager.saveCurrentMacro(macroSaveDialog.getName());

            // Note: Custom shortcut binding placeholder for macro triggers
            if (!macroSaveDialog.getShortcut().isEmpty()) {
                // Reserved for future shortcut key binding
            }
        }
    });

    connect(ui->actionRunMacroMultipleTimes, &QAction::triggered, this, [this]() {
        auto *macroRunDialog = findChild<MacroRunDialog *>(QString(), Qt::FindDirectChildrenOnly);

        if (macroRunDialog == Q_NULLPTR) {
            macroRunDialog = new MacroRunDialog(this, &macroManager);

            connect(macroRunDialog, &MacroRunDialog::execute, this, [this](const Macro *macro, int times) {
                if (times > 0) {
                    macro->replay(currentEditor(), times);
                } else if (times == -1) {
                    macro->replayTillEndOfFile(currentEditor());
                }
            });
        }

        macroRunDialog->show();
        macroRunDialog->raise();
        macroRunDialog->activateWindow();
    });

    connect(ui->actionEditMacros, &QAction::triggered, this, [this]() {
        MacroEditorDialog med(this, &macroManager);

        med.show();
        med.raise();
        med.activateWindow();

        med.exec();

        ui->actionEditMacros->setEnabled(!macroManager.availableMacros().isEmpty());
    });

    connect(ui->menuMacro, &QMenu::aboutToShow, this, [this]() {
        // NOTE: its unfortunate that this has to be hard coded, but there's no way
        // to easily determine what should or shouldn't be there
        while (ui->menuMacro->actions().size() > 6) {
            if (auto *act = ui->menuMacro->actions().takeLast()) {
                act->deleteLater();
            }
        }

        for (const Macro *m : macroManager.availableMacros()) {
            ui->menuMacro->addAction(m->getName(), [this, m]() { m->replay(currentEditor()); });
        }
    });

    // Encoding actions
    connect(ui->actionShowEncodingInfo, &QAction::triggered, this, [this]() {
        if (auto *ed = currentEditor()) {
            EncodingSelectionDialog dlg(ed, this);
            if (dlg.exec() == QDialog::Accepted) {
                ui->statusBar->refresh(ed);
            }
        }
    });

    connect(ui->actionConvertToUtf8, &QAction::triggered, this, [this]() {
        if (auto *ed = currentEditor()) {
            ed->convertToEncoding(QStringLiteral("UTF-8"), ScintillaNext::BomType::None);
            ui->statusBar->refresh(ed);
        }
    });
    connect(ui->actionConvertToUtf8Bom, &QAction::triggered, this, [this]() {
        if (auto *ed = currentEditor()) {
            ed->convertToEncoding(QStringLiteral("UTF-8"), ScintillaNext::BomType::Utf8);
            ui->statusBar->refresh(ed);
        }
    });
    connect(ui->actionConvertToAnsi, &QAction::triggered, this, [this]() {
        if (auto *ed = currentEditor()) {
            ed->convertToEncoding(QStringLiteral("windows-1252"), ScintillaNext::BomType::None);
            ui->statusBar->refresh(ed);
        }
    });
    connect(ui->actionConvertToUtf16Le, &QAction::triggered, this, [this]() {
        if (auto *ed = currentEditor()) {
            ed->convertToEncoding(QStringLiteral("UTF-16LE"), ScintillaNext::BomType::Utf16LE);
            ui->statusBar->refresh(ed);
        }
    });
    connect(ui->actionConvertToUtf16Be, &QAction::triggered, this, [this]() {
        if (auto *ed = currentEditor()) {
            ed->convertToEncoding(QStringLiteral("UTF-16BE"), ScintillaNext::BomType::Utf16BE);
            ui->statusBar->refresh(ed);
        }
    });

    auto reopenWith = [this](const QString &enc) {
        if (auto *ed = currentEditor()) {
            if (!ed->isFile()) {
                QMessageBox::information(this, tr("Notepad Next"),
                                         tr("Cannot reopen unsaved new buffer from disk."));
                return;
            }
            ed->reloadWithEncoding(enc);
            ui->statusBar->refresh(ed);
        }
    };

    connect(ui->actionReopenUtf8, &QAction::triggered, this, [reopenWith]() { reopenWith(QStringLiteral("UTF-8")); });
    connect(ui->actionReopenAnsi, &QAction::triggered, this,
            [reopenWith]() { reopenWith(QStringLiteral("windows-1252")); });
    connect(ui->actionReopenUtf16Le, &QAction::triggered, this,
            [reopenWith]() { reopenWith(QStringLiteral("UTF-16LE")); });
    connect(ui->actionReopenUtf16Be, &QAction::triggered, this,
            [reopenWith]() { reopenWith(QStringLiteral("UTF-16BE")); });
    connect(ui->actionReopenShiftJis, &QAction::triggered, this,
            [reopenWith]() { reopenWith(QStringLiteral("Shift-JIS")); });
    connect(ui->actionReopenGbk, &QAction::triggered, this, [reopenWith]() { reopenWith(QStringLiteral("GB18030")); });
    connect(ui->actionReopenBig5, &QAction::triggered, this, [reopenWith]() { reopenWith(QStringLiteral("Big5")); });
    connect(ui->actionReopenEucKr, &QAction::triggered, this, [reopenWith]() { reopenWith(QStringLiteral("EUC-KR")); });
    connect(ui->actionReopenWin1251, &QAction::triggered, this,
            [reopenWith]() { reopenWith(QStringLiteral("windows-1251")); });
    connect(ui->actionReopenWin1250, &QAction::triggered, this,
            [reopenWith]() { reopenWith(QStringLiteral("windows-1250")); });
    connect(ui->actionReopenCp437, &QAction::triggered, this, [reopenWith]() { reopenWith(QStringLiteral("IBM850")); });
    connect(ui->actionReopenMore, &QAction::triggered, this, [this]() {
        if (auto *ed = currentEditor()) {
            EncodingSelectionDialog dlg(ed, this);
            if (dlg.exec() == QDialog::Accepted) {
                ui->statusBar->refresh(ed);
            }
        }
    });

    ui->actionAboutQt->setIcon(QPixmap(QLatin1String(":/qt-project.org/qmessagebox/images/qtlogo-64.png")));
    connect(ui->actionAboutQt, &QAction::triggered, &QApplication::aboutQt);

    ui->actionAboutNotepadNext->setShortcut(QKeySequence::HelpContents);
    connect(ui->actionAboutNotepadNext, &QAction::triggered, this, [this]() {
        AboutDialog dlg(this);
        dlg.exec();
    });

    connect(ui->actionDebugInfo, &QAction::triggered, this, [this]() {
        QMessageBox mb(QMessageBox::Information, tr("Debug Info"), this->app->debugInfo().join('\n'), QMessageBox::Ok, this);

        mb.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont).family());
        mb.setTextInteractionFlags(Qt::TextSelectableByMouse);

        mb.exec();
    });

    tabsQuickActionsBar = new TabsQuickActionsBar(ui->menuBar);
    ui->menuBar->setCornerWidget(tabsQuickActionsBar, Qt::TopRightCorner);
    connect(tabsQuickActionsBar, &TabsQuickActionsBar::createNewTabClicked, this, &MainWindow::newFile);
    connect(tabsQuickActionsBar, &TabsQuickActionsBar::closeCurrentTabClicked, this, &MainWindow::closeCurrentFile);
    connect(tabsQuickActionsBar, &TabsQuickActionsBar::tabsMenuAboutToShow, this, [this](QMenu *editorsMenu) {
        const auto editorsList = editors();

        editorsMenu->clear();

        for (const auto editor : editorsList) {
            const auto iconPath = editor->isSavedToDisk() ? ":/icons/saved.png" : ":/icons/unsaved.png";
            const auto action = editorsMenu->addAction(QIcon(iconPath), editor->getName());
            action->setIconVisibleInMenu(true);

            if (editor->isActiveWindow()) {
                auto font = action->font();
                font.setBold(true);
                action->setFont(font);
            }

            connect(action, &QAction::triggered, this, [this, editor]() { switchToEditor(editor); });
        }
    });

#ifdef Q_OS_WIN
    connect(ui->actionShowInExplorer, &QAction::triggered, this, [this]() {
        QString filePath = QDir::toNativeSeparators(currentEditor()->getFileInfo().canonicalFilePath());
        QStringList arguments = {"/select,", filePath};
        QProcess::startDetached("explorer", arguments);
    });

    QString terminalName = app->getSettings()->value("App/TerminalName", "Command Prompt").toString();
    ui->actionOpenTerminalHere->setText(ui->actionOpenTerminalHere->text().arg(terminalName));

    connect(ui->actionOpenTerminalHere, &QAction::triggered, this, [this]() {
        QString command = app->getSettings()->value("App/TerminalCommand", "cmd").toString();
        QString filePath = QDir::toNativeSeparators(currentEditor()->getFileInfo().dir().canonicalPath());
        QStringList arguments = {"/c", "start", "/d", filePath, command};
        QProcess::startDetached("cmd", arguments);
    });
#endif

    auto *editorInspectorDock = new EditorInspectorDock(this);
    editorInspectorDock->hide();
    addDockWidget(Qt::RightDockWidgetArea, editorInspectorDock);

    auto *languageInspectorDock = new LanguageInspectorDock(this);
    languageInspectorDock->hide();
    addDockWidget(Qt::RightDockWidgetArea, languageInspectorDock);

    auto *luaConsoleDock = new LuaConsoleDock(app->getLuaState(), this);
    luaConsoleDock->hide();
    addDockWidget(Qt::BottomDockWidgetArea, luaConsoleDock);

    auto *debugLogDock = new DebugLogDock(this);
    debugLogDock->hide();
    addDockWidget(Qt::RightDockWidgetArea, debugLogDock);

    ui->menuHelp->insertActions(ui->menuHelp->actions().at(0), {
                                    luaConsoleDock->toggleViewAction(),
                                    languageInspectorDock->toggleViewAction(),
                                    editorInspectorDock->toggleViewAction(),
                                    debugLogDock->toggleViewAction()
                                });

    auto *fawDock = new FolderAsWorkspaceDock(this);
    fawDock->hide();
    addDockWidget(Qt::LeftDockWidgetArea, fawDock);
    ui->menuView->addAction(fawDock->toggleViewAction());
    connect(fawDock, &FolderAsWorkspaceDock::fileDoubleClicked, this, &MainWindow::openFile);

    auto *fileListDock = new FileListDock(this);
    fileListDock->hide();
    addDockWidget(Qt::LeftDockWidgetArea, fileListDock);
    ui->menuView->addAction(fileListDock->toggleViewAction());

    connect(app->getSettings(), &ApplicationSettings::showMenuBarChanged, this, [this](bool showMenuBar) {
        // Don't 'hide' it, else the actions won't be enabled
        ui->menuBar->setMaximumHeight(showMenuBar ? QWIDGETSIZE_MAX : 0);
    });
    connect(app->getSettings(), &ApplicationSettings::showToolBarChanged, ui->mainToolBar, &QToolBar::setVisible);
    connect(app->getSettings(), &ApplicationSettings::showStatusBarChanged, ui->statusBar, &QStatusBar::setVisible);
    connect(ui->statusBar, &EditorInfoStatusBar::customContextMenuRequestedForEOLLabel, this, [this](const QPoint &pos){
        ui->menuEOLConversion->popup(pos);
    });

    // It seems restoreState() does not affect the status bar so set it manually
    ui->statusBar->setVisible(app->getSettings()->showStatusBar());

    setupLanguageMenu();
    ThemeManager::instance()->initialize(app->getSettings());
    setupThemeMenu();

    applyStyleSheet();

    restoreSettings();

    initUpdateCheck();
    setupSystemTray();

    const auto allActions = findChildren<QAction *>();
    for (QAction *action : allActions) {
        action->setIconVisibleInMenu(true);
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::applyCustomShortcuts()
{
    auto *settings = app->getSettings();
    settings->beginGroup("Shortcuts");

    for (const QString &actionName : settings->childKeys()) {
        auto *action = findChild<QAction *>(QStringLiteral("action") + actionName, Qt::FindDirectChildrenOnly);

        if (!action) {
            qWarning() << "CustomShortcut: Cannot find action" << actionName;
            continue;
        }

        const QVariant value = settings->value(actionName);
        if (!value.canConvert<QStringList>()) {
            qWarning() << "CustomShortcut: Invalid shortcut format for" << actionName;
            continue;
        }

        QList<QKeySequence> shortcuts;
        for (const QString &shortcutString : value.toStringList()) {
            auto sequence = QKeySequence(shortcutString);

#ifndef QT_VERSION_CHECK
#define QT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))
#endif
#if defined(QT_VERSION) && (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
            if (!sequence.isEmpty() && sequence[0].key() != Qt::Key_unknown) {
#else
            if (!sequence.isEmpty() && (sequence[0] & ~Qt::KeyboardModifierMask) != Qt::Key_unknown) {
#endif
                shortcuts.append(sequence);
            }
            else {
                qWarning() << "CustomShortcut: Cannot create QKeySequence(" << shortcutString << ") for " << actionName;
            }
        }

        if (!shortcuts.empty()) {
            action->setShortcuts(shortcuts);
        }
    }

    settings->endGroup();

#ifdef Q_OS_MACOS
    // On macOS, the Alt modifier maps to Option (⌥), which is essential for entering
    // accented characters, currency symbols (€ = Option+2), hash (# = Option+3),
    // and brackets on various European and international keyboard layouts (Issues #814 & #667).
    // Single Option-key shortcuts (e.g. Alt+1 .. Alt+0) must be cleared so they don't
    // hijack character entry in the editor.
    const QList actionsWithAlt = {
        ui->actionFoldLevel1, ui->actionFoldLevel2, ui->actionFoldLevel3, ui->actionFoldLevel4,
        ui->actionFoldLevel5, ui->actionFoldLevel6, ui->actionFoldLevel7, ui->actionFoldLevel8,
        ui->actionFoldLevel9, ui->actionFoldAll,
        ui->actionUnfoldLevel1, ui->actionUnfoldLevel2, ui->actionUnfoldLevel3, ui->actionUnfoldLevel4,
        ui->actionUnfoldLevel5, ui->actionUnfoldLevel6, ui->actionUnfoldLevel7, ui->actionUnfoldLevel8,
        ui->actionUnfoldLevel9, ui->actionUnfoldAll
    };

    for (QAction *action : actionsWithAlt) {
        if (action) {
            action->setShortcut(QKeySequence());
        }
    }
#endif
}

void MainWindow::setupLanguageMenu()
{
    qInfo(Q_FUNC_INFO);

    QStringList language_names = app->getLanguages();

    int i = 0;
    while (i < language_names.size()) {
        QList<QAction *> actions;
        int j = i;

        // Get all consecutive names that start with the same letter
        // NOTE: this loop always runs once since i == j the first time
        while (j < language_names.size() && language_names[i][0].toUpper() == language_names[j][0].toUpper()) {
            const QString &key = language_names[j];
            auto *action = new QAction(key, this);
            action->setCheckable(true);
            action->setData(key);
            connect(action, &QAction::triggered, this, &MainWindow::languageMenuTriggered);
            languageActionGroup->addAction(action);
            actions.append(action);

            ++j;
        }

        if (actions.size() == 1) {
            ui->menuLanguage->addActions(actions);
        }
        else {
            // Create a sub menu with the actions
            auto *compactMenu = new QMenu(actions[0]->text().at(0).toUpper(), ui->menuLanguage);
            compactMenu->addActions(actions);
            ui->menuLanguage->addMenu(compactMenu);
        }
        i = j;
    }
}

namespace ThemeMenuHelpers {
void populateThemeSubmenu(QMenu *menu, const QList<const ThemePalette *> &palettes, MainWindow *window)
{
    for (const ThemePalette *pal : palettes) {
        auto *act = menu->addAction(QString::fromUtf8(pal->displayName));
        act->setCheckable(true);
        act->setData(QString::fromLatin1(pal->id));
        QObject::connect(act, &QAction::triggered, window, [window, pal]() {
            ThemeManager::instance()->setPalette(QString::fromLatin1(pal->id), window);
            window->statusBar()->showMessage(QCoreApplication::translate("MainWindow", "Theme applied: %1").arg(QString::fromUtf8(pal->displayName)), 3000);
        });
    }
}

void populateThemeFamilySubmenus(QMenu *familiesMenu, MainWindow *window)
{
    const QStringList families = getThemeFamiliesList();
    for (const QString &family : families) {
        QMenu *famMenu = familiesMenu->addMenu(family);
        const QList<const ThemePalette *> palettes = getThemePalettesByFamily(family);
        for (const ThemePalette *pal : palettes) {
            QString itemLabel = QString::fromUtf8(pal->displayName);
            itemLabel += pal->isDark ? QStringLiteral(" (Dark)") : QStringLiteral(" (Light)");
            if (famMenu) {
                auto *act = famMenu->addAction(itemLabel);
                act->setCheckable(true);
                act->setData(QString::fromLatin1(pal->id));
                QObject::connect(act, &QAction::triggered, window, [window, pal]() {
                    ThemeManager::instance()->setPalette(QString::fromLatin1(pal->id), window);
                    window->statusBar()->showMessage(QCoreApplication::translate("MainWindow", "Theme applied: %1").arg(QString::fromUtf8(pal->displayName)), 3000);
                });
            }
        }
    }
}

void updateThemeMenuChecks(const QMenu *menu, const QString &curId)
{
    if (!menu) {
        return;
    }
    for (QAction *action : menu->actions()) {
        if (action->menu()) {
            for (QAction *subAction : action->menu()->actions()) {
                subAction->setChecked(subAction->data().toString() == curId);
            }
        } else if (action->isCheckable()) {
            action->setChecked(action->data().toString() == curId);
        }
    }
}
} // namespace ThemeMenuHelpers
using namespace ThemeMenuHelpers;

void MainWindow::setupThemeMenu()
{
    if (menuTheme) {
        return;
    }

    menuTheme = new QMenu(tr("&Themes"), this);
    if (ui->menuLanguage) {
        ui->menuBar->insertMenu(ui->menuLanguage->menuAction(), menuTheme);
    } else {
        ui->menuBar->addMenu(menuTheme);
    }

    // 1. Open Theme Switcher... (Ctrl+Shift+T / Cmd+Shift+T)
    actionThemeSwitcher = new QAction(tr("Open Theme Switcher..."), this);
    actionThemeSwitcher->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_T));
    connect(actionThemeSwitcher, &QAction::triggered, this, [this]() {
        ThemeSwitcherDialog dlg(this);
        dlg.exec();
    });
    menuTheme->addAction(actionThemeSwitcher);

    menuTheme->addSeparator();

    // 2. Light Themes Only (Submenu)
    QMenu *menuLightOnly = menuTheme->addMenu(tr("Light Themes Only"));
    populateThemeSubmenu(menuLightOnly, getThemesByDarkMode(false), this);

    // 3. Dark Themes Only (Submenu)
    QMenu *menuDarkOnly = menuTheme->addMenu(tr("Dark Themes Only"));
    populateThemeSubmenu(menuDarkOnly, getThemesByDarkMode(true), this);

    menuTheme->addSeparator();

    // 4. All 25 Families grouped neatly in a dedicated Submenu
    QMenu *menuFamilies = menuTheme->addMenu(tr("Themes by Family"));
    populateThemeFamilySubmenus(menuFamilies, this);

    connect(menuTheme, &QMenu::aboutToShow, this, [menuLightOnly, menuDarkOnly, menuFamilies]() {
        const QString curId = ThemeManager::instance()->currentPaletteId();
        updateThemeMenuChecks(menuLightOnly, curId);
        updateThemeMenuChecks(menuDarkOnly, curId);
        updateThemeMenuChecks(menuFamilies, curId);
    });

    menuTheme->addSeparator();

    // 5. Reset and Mode Toggles
    actionThemeLight = new QAction(tr("Reset to Default (Light Mode - NotepadNext)"), this);
    connect(actionThemeLight, &QAction::triggered, this, [this]() {
        ThemeManager::instance()->setPalette(QStringLiteral("notepadNext"), this);
        ui->statusBar->showMessage(tr("Theme reset to default Light Mode (NotepadNext)"), 3000);
    });
    menuTheme->addAction(actionThemeLight);

    actionThemeDark = new QAction(tr("Default Dark Mode (Horizon Dark)"), this);
    connect(actionThemeDark, &QAction::triggered, this, [this]() {
        ThemeManager::instance()->setPalette(QStringLiteral("horizonDark"), this);
        ui->statusBar->showMessage(tr("Theme set to Dark Mode (Horizon Dark)"), 3000);
    });
    menuTheme->addAction(actionThemeDark);

    actionToggleTheme = new QAction(tr("Toggle Dark / Light Mode"), this);
    actionToggleTheme->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_T));
    connect(actionToggleTheme, &QAction::triggered, this, [this]() {
        ThemeManager::instance()->toggleTheme(this);
        bool dark = ThemeManager::instance()->isDarkActive();
        ui->statusBar->showMessage(dark ? tr("Switched to Dark Mode") : tr("Switched to Light Mode"), 3000);
    });
    menuTheme->addAction(actionToggleTheme);
}

ScintillaNext *MainWindow::currentEditor() const
{
    return dockedEditor->getCurrentEditor();
}

int MainWindow::editorCount() const
{
    return dockedEditor->count();
}

QVector<ScintillaNext *> MainWindow::editors() const
{
    // NOTE: this will need re-evaluated in the future.
    // So far it has been assumed 1 ScintillaNext instance is 1 DockedEditor widget instance.
    // If in the future a ScintillaNext can be cloned then the DockedEditor could return
    // the same ScintillaNext instance multiple times since 1 ScintillaNext could mean >= 1 DockedEditor widget instance
    return dockedEditor->editors();
}

void MainWindow::newFile() const
{
    qInfo(Q_FUNC_INFO);

    // NOTE: in theory need to check all editors in the editorManager to future proof this.
    // If there is another window it would need to check those too to see if New X exists. The editor
    // manager would encompass all editors

    int count = 1;
    forever {
        const QString newFileName = tr("New %1").arg(count);
        bool canUseName = true;

        for (const ScintillaNext *editor : editors()) {
            if (!editor->isFile() && editor->getName() == newFileName) {
                canUseName = false;
                break;
            }
        }

        if (canUseName) {
            ScintillaNext *editor = app->getEditorManager()->createEditor(newFileName);
            editor->grabFocus();
            break;
        }

        count++;
    }
}

void MainWindow::newWindow()
{
    if (MainWindow *newWin = app->createNewWindow()) {
        newWin->restoreWindowState();
        newWin->newFile();
        newWin->show();
        newWin->bringWindowToForeground();
    }
}

// One unedited, new blank document
ScintillaNext *MainWindow::getInitialEditor()
{
    if (editorCount() == 1) {
        ScintillaNext *editor = currentEditor();
        if (!editor) {
            return Q_NULLPTR;
        }

        // If the editor:
        //   is a temporary file
        //   is a 'real' file (or a 'missing' file)
        //   can undo any actions
        //   can redo any actions
        // Then do not treat it as an 'initial editor' that can be transparently closed for the user
        if (editor->isTemporary() || editor->isFile() || editor->canUndo() || editor->canRedo()) {
            return Q_NULLPTR;
        }

        return editor;
    }

    return Q_NULLPTR;
}

void MainWindow::openFileList(const QStringList &fileNames)
{
    qInfo(Q_FUNC_INFO);

    if (fileNames.isEmpty()) {
        return;
    }

    QList<ScintillaNext *> openedEditors;
    ScintillaNext *initialEditor = getInitialEditor();

    auto loadOrCreateFile = [this](const QString &filePath) -> ScintillaNext * {
        if (auto *existing = app->getEditorManager()->getEditorByFilePath(filePath)) {
            return existing;
        }
        if (const QFileInfo fileInfo(filePath); !fileInfo.isFile()) {
            if (const auto reply = QMessageBox::question(this, tr("Create File"), tr("<b>%1</b> does not exist. Do you want to create it?").arg(filePath)); reply == QMessageBox::Yes) {
                return app->getEditorManager()->createEditorFromFile(filePath, true);
            }
            app->getRecentFilesListManager()->removeFile(filePath);
            return nullptr;
        }
        return app->getEditorManager()->createEditorFromFile(filePath);
    };

    for (const QString &filePath : fileNames) {
        qInfo("%s", qUtf8Printable(filePath));
        if (auto *editor = loadOrCreateFile(filePath)) {
            openedEditors.append(editor);
        }
    }

    // If any were successful
    if (!openedEditors.empty()) {
        dockedEditor->switchToEditor(openedEditors.last());

        if (initialEditor) {
            initialEditor->close();
        }
    }

}

bool MainWindow::checkEditorsBeforeClose(const QVector<ScintillaNext *> &editorList)
{
    QVector<ScintillaNext *> unsaved;
    for (auto *e : editorList) {
        if (!e) {
            continue;
        }
        if (!e->isSavedToDisk()) {
            unsaved.append(e);
        }
    }

    if (unsaved.isEmpty()) {
        return true;
    }

    // Focus the user's attention on the first unsaved file
    dockedEditor->switchToEditor(unsaved.first());

    // Single point of interaction
    UserSaveAction action = promptForSave(unsaved);

    switch (action) {
        using enum UserSaveAction;
        case DiscardAll:
            return true;
        case SaveAll:
            return saveAllEditors(unsaved);
        case Cancel:
        default:
            return false;
    }
}

void MainWindow::openFileDialog()
{
    const QString filter = app->getFileDialogFilter();

    QStringList fileNames = FileDialogHelpers::getOpenFileNames(this, QString(), defaultDirectoryManager->getDefaultDirectory(), filter);

    if (!fileNames.empty()) {
        emit fileDialogAccepted(fileNames.last());
    }

    openFileList(fileNames);
}

void MainWindow::openFile(const QString &filePath)
{
    openFileList(QStringList() << filePath);
}

void MainWindow::openFolderAsWorkspaceDialog()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Folder as Workspace"), defaultDirectoryManager->getDefaultDirectory(), QFileDialog::ShowDirsOnly);

    setFolderAsWorkspacePath(dir);
}

void MainWindow::setFolderAsWorkspacePath(const QString &dir) const
{
    if (!dir.isEmpty()) {
        if (auto *fawDock = findChild<FolderAsWorkspaceDock *>()) {
            fawDock->setRootPath(dir);
            fawDock->setVisible(true);
        }
    }
}

void MainWindow::reloadFile()
{
    auto editor = currentEditor();

    if (!editor->isFile() && !editor->isSavedToDisk()) {
        return;
    }

    const QString filePath = editor->getFilePath();
    auto reply = QMessageBox::question(this, tr("Reload File"), tr("Are you sure you want to reload <b>%1</b>? Any unsaved changes will be lost.").arg(filePath));

    if (reply == QMessageBox::Yes) {
        editor->reload();
    }
}

void MainWindow::closeCurrentFile()
{
    closeFile(currentEditor());
}

void MainWindow::closeFile(ScintillaNext *editor)
{
    if (!editor) {
        return;
    }

    // Early out. If we aren't exiting on last tab closed, and it exists, there's no point in continuing
    if (!app->getSettings()->exitOnLastTabClosed() && getInitialEditor() != Q_NULLPTR) {
        return;
    }

    QPointer<ScintillaNext> pEditor = editor;
    if (!checkEditorsBeforeClose({editor})) {
        return;
    }

    if (pEditor) {
        pEditor->close();
    }

    // If the last document was closed, figure out what to do next. Defer to next event loop iteration
    // to allow Qt-Advanced-Docking-System tab-close signal handler to finish unwinding.
    QTimer::singleShot(0, this, [this]() {
        if (editorCount() == 0) {
            if (app->getSettings()->exitOnLastTabClosed()) {
                close();
            } else {
                newFile();
            }
        }
    });
}

void MainWindow::closeAllFiles()
{
    if (!checkEditorsBeforeClose(editors())) {
        return;
    }

    // Ask the manager to close the editors the dockedEditor knows about
    for (ScintillaNext *editor : editors()) {
        if (editor) {
            editor->close();
        }
    }

    QTimer::singleShot(0, this, [this]() {
        if (editorCount() == 0) {
            newFile();
        }
    });
}

void MainWindow::closeAllExceptActive()
{
    auto e = currentEditor();
    auto editor_list = editors();

    editor_list.removeOne(e);

    if (checkEditorsBeforeClose(editor_list)) {
        for (ScintillaNext *editor : editor_list) {
            if (editor) {
                editor->close();
            }
        }
    }
}

void MainWindow::closeAllToLeft()
{
    ads::CDockAreaWidget *dockArea = dockedEditor->currentDockArea();
    if (!dockArea) {
        return;
    }
    const int index = dockArea->currentIndex();
    QVector<ScintillaNext *> tabEditors;

    for (int i = 0; i < index; ++i) {
        if (const auto *dw = dockArea->dockWidget(i)) {
            if (auto editor = qobject_cast<ScintillaNext *>(dw->widget())) {
                tabEditors.append(editor);
            }
        }
    }

    if (checkEditorsBeforeClose(tabEditors)) {
        for (ScintillaNext *editor : tabEditors) {
            if (editor) {
                editor->close();
            }
        }
    }
}

void MainWindow::closeAllToRight()
{
    ads::CDockAreaWidget *dockArea = dockedEditor->currentDockArea();
    if (!dockArea) {
        return;
    }
    const int index = dockArea->currentIndex();
    const int total = dockArea->dockWidgetsCount();
    QVector<ScintillaNext *> tabEditors;

    for (int i = index + 1; i < total; ++i) {
        if (const auto *dw = dockArea->dockWidget(i)) {
            if (auto editor = qobject_cast<ScintillaNext *>(dw->widget())) {
                tabEditors.append(editor);
            }
        }
    }

    if (checkEditorsBeforeClose(tabEditors)) {
        for (ScintillaNext *editor : tabEditors) {
            if (editor) {
                editor->close();
            }
        }
    }
}

bool MainWindow::saveCurrentFile()
{
    return saveFile(currentEditor());
}

bool MainWindow::saveFile(ScintillaNext *editor)
{
    if (!editor) {
        return false;
    }

    if (editor->isSavedToDisk()) {
        return true;
    }

    if (!editor->isFile()) {
        // Switch to the editor and show the saveas dialog
        dockedEditor->switchToEditor(editor);
        return saveFileAsDialog(editor);
    }
    else {
        QFileDevice::FileError error = editor->save();
        if (error == QFileDevice::NoError) {
            return true;
        }
        else {
            showSaveErrorMessage(editor, error);
            return false;
        }
    }
}

bool MainWindow::saveFileAsDialog(ScintillaNext *editor)
{
    if (!editor) {
        return false;
    }

    const QString filter = app->getFileDialogFilter();
    QString selectedFilter = app->getFileDialogFilterForLanguage(editor->languageName);
    QString fileName = FileDialogHelpers::getSaveFileName(this, QString(), defaultDirectoryManager->getDefaultDirectory(), filter, &selectedFilter);

    if (fileName.isEmpty()) {
        return false;
    }

    emit fileDialogAccepted(fileName);

    return saveFileAs(editor, fileName);
}

bool MainWindow::saveCurrentFileAsDialog()
{
    return saveFileAsDialog(currentEditor());
}

bool MainWindow::saveCurrentFileAs(const QString &fileName)
{
    return saveFileAs(currentEditor(), fileName);
}

bool MainWindow::saveFileAs(ScintillaNext *editor, const QString &fileName)
{
    qInfo("saveFileAs(%s)", qUtf8Printable(fileName));

    QFileDevice::FileError error = editor->saveAs(fileName);

    if (error == QFileDevice::NoError) {
        return true;
    }
    else {
        showSaveErrorMessage(editor, error);
        return false;
    }
}

bool MainWindow::saveCopyAsDialog()
{
    const QString filter = app->getFileDialogFilter();
    const QString languageName = currentEditor()->languageName;

    QString selectedFilter = app->getFileDialogFilterForLanguage(languageName);
    const QString fileName = FileDialogHelpers::getSaveFileName(this, tr("Save a Copy As"), defaultDirectoryManager->getDefaultDirectory(), filter, &selectedFilter);

    if (fileName.isEmpty()) {
        return false;
    }

    emit fileDialogAccepted(fileName);

    return saveCopyAs(fileName);
}

bool MainWindow::saveCopyAs(const QString &fileName)
{
    auto editor = currentEditor();

    QFileDevice::FileError error = editor->saveCopyAs(fileName);

    if (error == QFileDevice::NoError) {
        return true;
    }
    else {
        showSaveErrorMessage(editor, error);
        return false;
    }
}

bool MainWindow::saveAll()
{
    return saveAllEditors(editors());
}

bool MainWindow::saveAllEditors(const QVector<ScintillaNext *> &editorList)
{
    for (ScintillaNext *editor : editorList) {
        if (!saveFile(editor)){
            return false;
        }
    }

    return true;
}

void MainWindow::exportAsFormat(Converter *converter, const QString &filter)
{
    const QString fileName = FileDialogHelpers::getSaveFileName(this, tr("Export As"), QString(), filter + ";;All files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile f(fileName);

    if (f.open(QIODevice::WriteOnly)) {
        QTextStream s(&f);
        converter->convert(s);
        f.close();
    }
}

void MainWindow::copyAsFormat(Converter *converter, const QString &mimeType) const
{
    // This is not ideal as we are *assuming* the converter is currently associated with the currentEditor()
    const ScintillaNext *editor = currentEditor();
    QByteArray buffer;
    QTextStream stream(&buffer);

    if (editor->selectionEmpty()) {
        converter->convert(stream);
    } else {
        converter->convertRange(stream, static_cast<int>(editor->selectionStart()), static_cast<int>(editor->selectionEnd()));
    }

    auto mimeData = std::make_unique<QMimeData>();
    mimeData->setData(mimeType, buffer);

    QApplication::clipboard()->setMimeData(mimeData.release());
}

void MainWindow::renameFile()
{
    ScintillaNext *editor = currentEditor();

    if (editor->isFile()) {
        const QString filter = app->getFileDialogFilter();
        QString selectedFilter = app->getFileDialogFilterForLanguage(editor->languageName);
        QString fileName = FileDialogHelpers::getSaveFileName(this, tr("Rename"), editor->getFilePath(), filter, &selectedFilter);

        if (fileName.isEmpty()) {
            return;
        }

        emit fileDialogAccepted(fileName);

        // Note: Target collision checks handled by editor->rename(); warns if destination is locked or unwritable.
        bool renameSuccessful = editor->rename(fileName);
        if (!renameSuccessful) {
            showSaveErrorMessage(editor, QFileDevice::WriteError);
        }
    }
    else {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Rename"), tr("Name:"), QLineEdit::Normal, editor->getName(), &ok);

        if (ok && !text.isEmpty()) {
            editor->setName(text);
        }
    }
}

void MainWindow::moveCurrentFileToTrash()
{
    const ScintillaNext *editor = currentEditor();

    moveFileToTrash(editor);
}

void MainWindow::moveFileToTrash(const ScintillaNext *editor)
{
    Q_ASSERT(editor->isFile());

    const QString filePath = editor->getFilePath();
    auto reply = QMessageBox::question(this, tr("Delete File"), tr("Are you sure you want to move <b>%1</b> to the trash?").arg(filePath));

    if (reply == QMessageBox::Yes) {
        if (editor->moveToTrash()) {
            closeCurrentFile();

            // Since the file no longer exists, specifically remove it from the recent files list
            app->getRecentFilesListManager()->removeFile(editor->getFilePath());
        }
        else {
            QMessageBox::warning(this, tr("Error Deleting File"),  tr("Something went wrong deleting <b>%1</b>?").arg(filePath));
        }
    }
}

void MainWindow::print()
{
    QPrintPreviewDialog printDialog(this, Qt::Window);
    EditorPrintPreviewRenderer renderer(currentEditor());

    connect(&printDialog, &QPrintPreviewDialog::paintRequested, &renderer, &EditorPrintPreviewRenderer::render);

    // Future enhancement: load/save the page layout that was used and reload it next time

    printDialog.printer()->setPageMargins(QMarginsF(.5, .5, .5, .5), QPageLayout::Inch);

    connect(&printDialog, &QPrintPreviewDialog::accepted, this, [&printDialog]() {
        qInfo() << printDialog.printer()->pageLayout();
    });

    printDialog.exec();
}

void MainWindow::convertEOLs(int eolMode)
{
    ScintillaNext *editor = currentEditor();

    // Note: convertEOLs performs document updates with standard Scintilla modification tracking.
    editor->convertEOLs(eolMode);
    editor->setEOLMode(eolMode);

    updateEOLBasedUi(editor);

    // There's no simple Scintilla notification that the EOL mode has changed
    // So tell the status bar to refresh its info
    ui->statusBar->refresh(editor);
}

template <typename TransformFunc>
static void applyCaseTransform(ScintillaNext *editor, TransformFunc &&transformFunc)
{
    if (!editor) {
        return;
    }

    std::vector<std::pair<int, int>> selections;
    const auto num = static_cast<int>(editor->selections());
    for (int i = 0; i < num; ++i) {
        const auto start = static_cast<int>(editor->selectionNStart(i));
        const auto end = static_cast<int>(editor->selectionNEnd(i));
        if (start != end) {
            selections.emplace_back(start, end);
        }
    }

    if (selections.empty()) {
        return;
    }

    std::ranges::sort(selections);

    editor->beginUndoAction();
    editor->clearSelections();

    int offset = 0;
    bool isFirst = true;
    for (const auto &[selStart, selEnd] : selections) {
        const int start = selStart + offset;
        const int end = selEnd + offset;
        editor->setTargetRange(start, end);

        const QByteArray target = editor->targetText();
        const QString text = QString::fromUtf8(target);
        const QString transformed = transformFunc(text);
        const QByteArray transBytes = transformed.toUtf8();

        editor->replaceTarget(static_cast<int>(transBytes.length()), transBytes.constData());

        const int newEnd = start + static_cast<int>(transBytes.length());
        if (isFirst) {
            editor->setSelection(newEnd, start);
            isFirst = false;
        } else {
            editor->addSelection(newEnd, start);
        }

        offset += static_cast<int>(transBytes.length() - target.length());
    }
    editor->endUndoAction();
}

void MainWindow::titleCase() const
{
    applyCaseTransform(currentEditor(), [](const QString &input) {
        QString result = input;
        bool newWord = true;
        for (QChar &ch : result) {
            if (ch.isLetter()) {
                if (newWord) {
                    ch = ch.toUpper();
                    newWord = false;
                } else {
                    ch = ch.toLower();
                }
            } else if (!ch.isMark()) {
                newWord = true;
            }
        }
        return result;
    });
}

void MainWindow::sentenceCase() const
{
    applyCaseTransform(currentEditor(), [](const QString &input) {
        QString result = input;
        bool newSentence = true;
        for (QChar &ch : result) {
            if (ch.isLetter()) {
                if (newSentence) {
                    ch = ch.toUpper();
                    newSentence = false;
                } else {
                    ch = ch.toLower();
                }
            } else if (ch == QLatin1Char('.') || ch == QLatin1Char('!') || ch == QLatin1Char('?') || ch == QLatin1Char('\n') || ch == QLatin1Char('\r')) {
                newSentence = true;
            }
        }
        return result;
    });
}

void MainWindow::invertCase() const
{
    applyCaseTransform(currentEditor(), [](const QString &input) {
        QString result = input;
        for (QChar &ch : result) {
            if (ch.isUpper()) {
                ch = ch.toLower();
            } else if (ch.isLower()) {
                ch = ch.toUpper();
            }
        }
        return result;
    });
}

void MainWindow::pasteWithoutFormat() const
{
    ScintillaNext *editor = currentEditor();
    if (!editor) {
        return;
    }
    const QClipboard *clipboard = QApplication::clipboard();
    if (!clipboard) {
        return;
    }
    const QString text = clipboard->text(QClipboard::Clipboard);
    if (!text.isEmpty()) {
        const QByteArray bytes = text.toUtf8();
        editor->replaceSel(bytes.constData());
    }
}

void MainWindow::showFindReplaceDialog(int index)
{
    ScintillaNext *editor = currentEditor();
    auto *frd = findChild<FindReplaceDialog *>(QString(), Qt::FindDirectChildrenOnly);

    if (frd == Q_NULLPTR) {
        frd = new FindReplaceDialog(determineSearchResultsHandler(), this);
    }
    else {
        frd->setSearchResultsHandler(determineSearchResultsHandler());
    }

    // Note: FindReplaceDialog tracks editor activation dynamically when tabs switch.

    // Get any selected text
    if (const auto range = editor->getContextText(); range.cpMin != range.cpMax) {
        editor->goToRange(range);
        auto text = editor->get_text_range(static_cast<int>(range.cpMin), static_cast<int>(range.cpMax));
        frd->setFindString(QString::fromUtf8(text));
    }

    frd->setTab(index);
    frd->show();
    frd->raise();
    frd->activateWindow();
}

void MainWindow::updateFileStatusBasedUi(const ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    if (!editor) {
        return;
    }

    bool isFile = editor->isFile();
    QString fileName;

    if (isFile) {
        fileName = editor->getFilePath();
    }
    else {
        fileName = editor->getName();
    }

    QString title = QStringLiteral("[*]%1").arg(fileName);
    if (app->isRunningAsAdmin()) {
        title += QStringLiteral(" - [%1]").arg(tr("Administrator"));
    }
    setWindowTitle(title);

    ui->actionReload->setEnabled(isFile);
    ui->actionMoveToTrash->setEnabled(isFile);
    ui->actionCopyFullPath->setEnabled(isFile);
    ui->actionCopyFileDirectory->setEnabled(isFile);
    ui->actionShowInExplorer->setEnabled(isFile);
    ui->actionOpenTerminalHere->setEnabled(isFile);
}

bool MainWindow::isAnyUnsaved() const
{
    const auto allEditors = editors();
    return std::ranges::any_of(allEditors, [](const ScintillaNext *e) {
        return e && !e->isSavedToDisk();
    });
}

void MainWindow::updateEOLBasedUi(const ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    if (!editor) {
        return;
    }

    switch(editor->eOLMode()) {
    case SC_EOL_CR:
        ui->actionMacintosh->setChecked(true);
        break;
    case SC_EOL_CRLF:
        ui->actionWindows->setChecked(true);
        break;
    case SC_EOL_LF:
        ui->actionUnix->setChecked(true);
        break;
    default:
        break;
    }
}

void MainWindow::updateSaveStatusBasedUi(const ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    if (!editor) {
        return;
    }

    bool isDirty = !editor->isSavedToDisk();

    setWindowModified(isDirty);

    ui->actionSave->setEnabled(isDirty);
    ui->actionSaveAll->setEnabled(isDirty || isAnyUnsaved());

    if (editorCount() == 1) {
        bool ableToClose = editor->isFile() || isDirty;
        ui->actionClose->setEnabled(ableToClose);
        ui->actionCloseAll->setEnabled(ableToClose);
    }
    else {
        ui->actionClose->setEnabled(true);
        ui->actionCloseAll->setEnabled(true);
    }
}

void MainWindow::updateEditorPositionBasedUi()
{
    const auto *dockArea = dockedEditor->currentDockArea();
    if (!dockArea) {
        ui->actionCloseAllToLeft->setEnabled(false);
        ui->actionCloseAllToRight->setEnabled(false);
        ui->actionCloseAllExceptActive->setEnabled(false);
        return;
    }

    const int index = dockArea->currentIndex();
    const int total = dockArea->dockWidgetsCount();

    ui->actionCloseAllToLeft->setEnabled(index > 0);
    ui->actionCloseAllToRight->setEnabled(index < (total - 1));
    ui->actionCloseAllExceptActive->setEnabled(editorCount() > 1);
}

void MainWindow::updateLanguageBasedUi(const ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    if (!editor) {
        return;
    }

    const QString language_name = editor->languageName;

    for (QAction *action : languageActionGroup->actions()) {
        if (action->data().toString() == language_name) {
            action->setChecked(true);

            // Found one, so we are completely done
            return;
        }
    }

    // The above loop did not set any action as checked, so make sure they are all unchecked now
    for (QAction *action : languageActionGroup->actions()) {
        if (action->isChecked()) {
            action->setChecked(false);
        }
    }
}

void MainWindow::updateGui(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    if (!editor) {
        return;
    }

    updateFileStatusBasedUi(editor);
    updateSaveStatusBasedUi(editor);
    updateEOLBasedUi(editor);
    updateEditorPositionBasedUi();
    updateSelectionBasedUi(editor);
    updateContentBasedUi(editor);
    updateLanguageBasedUi(editor);
}

void MainWindow::updateDocumentBasedUi(Scintilla::Update updated)
{
    auto *editor = qobject_cast<ScintillaNext *>(sender());

    if (!editor) {
        return;
    }

    using enum Scintilla::Update;
    if (Scintilla::FlagSet(updated, Text)) {
        updateSelectionBasedUi(editor);
    }

    if (Scintilla::FlagSet(updated, Text) || Scintilla::FlagSet(updated, Selection)) {
        updateContentBasedUi(editor);
    }
}

void MainWindow::updateSelectionBasedUi(ScintillaNext *editor)
{
    if (!editor) {
        return;
    }

    ui->actionUndo->setEnabled(editor->canUndo());
    ui->actionRedo->setEnabled(editor->canRedo());
}

void MainWindow::updateContentBasedUi(ScintillaNext *editor)
{
    if (!editor) {
        return;
    }

    bool hasAnySelections = !editor->selectionEmpty();

    ui->actionPaste->setEnabled(editor->canPaste());
    ui->actionPasteWithoutFormat->setEnabled(editor->canPaste());

    ui->actionLowerCase->setEnabled(hasAnySelections);
    ui->actionUpperCase->setEnabled(hasAnySelections);
    ui->actionTitleCase->setEnabled(hasAnySelections);
    ui->actionSentenceCase->setEnabled(hasAnySelections);
    ui->actionInvertCase->setEnabled(hasAnySelections);

    ui->actionBase64Encode->setEnabled(hasAnySelections);
    ui->actionURLEncode->setEnabled(hasAnySelections);
    ui->actionBase64Decode->setEnabled(hasAnySelections);
    ui->actionURLDecode->setEnabled(hasAnySelections);
}

void MainWindow::detectLanguage(ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    if (!editor) {
        return;
    }

    if (!editor->isFile()) {
        // Default to the user's preferred language if it is not a file.
        QString language_name = app->getSettings()->defaultLanguage();

        if (!app->getLanguages().contains(language_name)) {
            qWarning("Unexpected DefaultLanguage: %s", qUtf8Printable(language_name));
            language_name = QStringLiteral("Text");
        }

        setLanguage(editor, language_name);
        return;
    }
    else {
        const QString language_name = app->detectLanguage(editor);

        setLanguage(editor, language_name);
    }

    return;
}

void MainWindow::activateEditor(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    if (!editor) {
        return;
    }

    checkFileForModification(editor);
    updateGui(editor);

    emit editorActivated(editor);
}

void MainWindow::applyStyleSheet()
{
    qInfo(Q_FUNC_INFO);

    QString sheet = ThemeManager::instance()->getThemeStyleSheet();

    // If there is a "custom.css" file where the ini is located, load it as a style sheet addition
    const QString directoryPath = QFileInfo(app->getSettings()->fileName()).absolutePath();
    if (const QString fullPath = QDir(directoryPath).filePath("custom.css"); QFile::exists(fullPath)) {
        QFile custom(fullPath);
        qInfo() << "Loading stylesheet:" << custom.fileName();

        if (custom.open(QFile::ReadOnly)) {
            sheet += custom.readAll();
            custom.close();
        }
    }

    setStyleSheet(sheet);
}

void MainWindow::setLanguage(ScintillaNext *editor, const QString &languageName) const
{
    qInfo(Q_FUNC_INFO);
    qInfo("Language Name: %s", qUtf8Printable(languageName));

    app->setEditorLanguage(editor, languageName);
}

void MainWindow::bringWindowToForeground()
{
    qInfo(Q_FUNC_INFO);

    // There doesn't seem to be a cross platform way to force the window to the foreground

#ifdef Q_OS_WIN
    HWND hWnd = reinterpret_cast<HWND>(effectiveWinId());

    if (hWnd) {
        // I have no idea what this does, but it seems to work on Windows
        // References:
        // https://stackoverflow.com/questions/916259/win32-bring-a-window-to-top
        // https://github.com/notepad-plus-plus/notepad-plus-plus/blob/ebe7648ee1a5a560d4fc65297cbdcf08055e56e3/PowerEditor/src/winmain.cpp#L596

        HWND hCurWnd = GetForegroundWindow();
        DWORD threadId = GetCurrentThreadId();
        DWORD procId = GetWindowThreadProcessId(hCurWnd, NULL);

        int sw = 0;
        if (IsZoomed(hWnd)) {
            sw = SW_MAXIMIZE;
        } else if (IsIconic(hWnd)) {
            sw = SW_RESTORE;
        }

        if (sw != 0) {
            ShowWindow(hWnd, sw);
        }

        AttachThreadInput(procId, threadId, TRUE);
        SetForegroundWindow(hWnd);
        SetFocus(hWnd);
        AttachThreadInput(procId, threadId, FALSE);
    }
#else
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();
    activateWindow();
#endif
}

bool MainWindow::checkFileForModification(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    using enum ScintillaNext::FileStateChange;
    if (const auto state = editor->checkFileForStateChange(); state == NoChange) {
        return false;
    }
    else if (state == Modified) {
        qInfo("ScintillaNext::Modified");
        const QString filePath = editor->getFilePath();
        QString message = tr("<b>%1</b> has been modified by another program. Do you want to reload it?").arg(filePath);

        if (!editor->isSavedToDisk()) {
            message += tr("<br><br><b>Warning:</b> you have unsaved changes in this tab. "
                           "Reloading will discard them permanently.");
        }

        auto reply = QMessageBox::question(this, tr("Reload File"), message);

        if (reply == QMessageBox::Yes) {
            editor->reload();
        }
        else {
            editor->omitModifications();
        }
    }
    else if (state == Deleted) {
        qInfo("ScintillaNext::Deleted");
    }
    else if (state == Restored) {
        qInfo("ScintillaNext::Restored");
    }

    return true;
}

void MainWindow::showSaveErrorMessage(const ScintillaNext *editor, QFileDevice::FileError error)
{
    const QString name = editor->isFile() ? editor->getFilePath() : editor->getName();

    // Map error code to human-readable string
    QString errorString;
    switch (error) {
        case QFileDevice::ReadError:        errorString = tr("Read error"); break;
        case QFileDevice::WriteError:       errorString = tr("Write error"); break;
        case QFileDevice::FatalError:       errorString = tr("Fatal error"); break;
        case QFileDevice::ResourceError:    errorString = tr("Resource error"); break;
        case QFileDevice::OpenError:        errorString = tr("Open error"); break;
        case QFileDevice::AbortError:       errorString = tr("Abort error"); break;
        case QFileDevice::TimeOutError:     errorString = tr("Timeout error"); break;
        case QFileDevice::UnspecifiedError: errorString = tr("Unspecified error"); break;
        case QFileDevice::RemoveError:      errorString = tr("Remove error"); break;
        case QFileDevice::RenameError:      errorString = tr("Rename error"); break;
        case QFileDevice::PositionError:    errorString = tr("Position error"); break;
        case QFileDevice::ResizeError:      errorString = tr("Resize error"); break;
        case QFileDevice::PermissionsError: errorString = tr("Permissions error"); break;
        case QFileDevice::CopyError:        errorString = tr("Copy error"); break;
        default:                            errorString = tr("Unknown error (%1)").arg(static_cast<int>(error)); break;
    }

    QMessageBox::warning(this, tr("Error Saving File"),
        tr("An error occurred when saving <b>%1</b><br><br>Error: %2").arg(name, errorString));
}

void MainWindow::showEditorZoomLevelIndicator() const
{
    const QString msg = tr("Zoom: %1%").arg(zoomLevel * 10 + 100);
    if (statusBar()) {
        statusBar()->showMessage(msg, 2000);
    }
}

MainWindow::UserSaveAction MainWindow::promptForSave(const QVector<ScintillaNext *> &editorList)
{
    SaveModifiedDialog dlg(editorList, this);
    dlg.exec();

    switch (dlg.userAction()) {
        using enum SaveModifiedDialog::UserAction;
        case Save:
            return UserSaveAction::SaveAll;
        case Discard:
            return UserSaveAction::DiscardAll;
        case Cancel:
        default:
            return UserSaveAction::Cancel;
    }
}

QStringList MainWindow::currentDockObjectNames() const
{
    QStringList names;

    for (const QDockWidget *dock : findChildren<QDockWidget *>()) {
        names.append(dock->objectName());
    }

    names.sort();

    return names;
}

void MainWindow::saveSettings() const
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings *settings = app->getSettings();

    settings->setValue("MainWindow/geometry", saveGeometry());
    settings->setValue("MainWindow/windowState", saveState());
    settings->setValue("MainWindow/dockObjectNames", currentDockObjectNames());

    settings->setValue("Editor/ZoomLevel", zoomLevel);
}

void MainWindow::restoreSettings()
{
    qInfo(Q_FUNC_INFO);

    const ApplicationSettings *settings = app->getSettings();

    zoomLevel = settings->value("Editor/ZoomLevel", 0).toInt();

    if (settings->contains("Gui/ToolBar")) {
        QStringList actionNames;
        actionNames = settings->value("Gui/ToolBar").toStringList();

        ui->mainToolBar->clear();

        ActionUtils::populateActionContainer(ui->mainToolBar, this, actionNames);
    }

    // Ensure Preferences and Exit actions are always present on the toolbar
    bool hasPreferences = false;
    bool hasExit = false;
    for (const QAction *act : ui->mainToolBar->actions()) {
        if (act == ui->actionPreferences) {
            hasPreferences = true;
        }
        if (act == ui->actionExit) {
            hasExit = true;
        }
    }
    if (!hasPreferences) {
        ui->mainToolBar->addSeparator();
        ui->mainToolBar->addAction(ui->actionPreferences);
    }
    if (!hasExit) {
        ui->mainToolBar->addSeparator();
        ui->mainToolBar->addAction(ui->actionExit);
    }
}

ISearchResultsHandler *MainWindow::determineSearchResultsHandler()
{
    // Determine what will get the search results
    if (app->getSettings()->combineSearchResults()) {
        searchResults.reset(new SearchResultsCollector(findChild<SearchResultsDock *>()));

        return searchResults.data();
    }
    else {
        return findChild<SearchResultsDock *>();
    }
}

void MainWindow::restoreWindowState()
{
    const ApplicationSettings *settings = app->getSettings();

    restoreGeometry(settings->value("MainWindow/geometry").toByteArray());

    // QMainWindow::restoreState() can crash (QTBUG-111538) when the saved
    // state references a set of dock widgets that doesn't match the ones
    // that exist now, e.g. after an upgrade added/removed/renamed a dock.
    // Only restore it when the dock set is unchanged since it was saved.
    if (settings->value("MainWindow/dockObjectNames").toStringList() == currentDockObjectNames()) {
        restoreState(settings->value("MainWindow/windowState").toByteArray());
    }
    else {
        qWarning() << "Skipping windowState restore: saved dock layout does not match the current dock set";
    }

    // Always hide the dock no matter how the application was closed
    auto *srDock = findChild<SearchResultsDock *>();
    if (srDock != nullptr) {
        srDock->hide();
    }
}

void MainWindow::switchToEditor(const ScintillaNext *editor)
{
    dockedEditor->switchToEditor(editor);
}

void MainWindow::focusIn()
{
    qInfo(Q_FUNC_INFO);

    ScintillaNext *editor = currentEditor();

    if (editor && checkFileForModification(editor)) {
        updateGui(editor);
    }
}

void MainWindow::changeEvent(QEvent *changeEvent)
{
    QMainWindow::changeEvent(changeEvent);

    if (changeEvent->type() == QEvent::ActivationChange) {
        qInfo() << "ActivationChange, isActiveWindow() =" << isActiveWindow();
        if (isActiveWindow()) {
            focusIn();
        }
    }
}

namespace ContextMenuHelpers {
QStringList resolveContextMenuActionNames(const NotepadNextApplication *app, const ScintillaNext *editor, int clickPos) {
    QStringList actionNames = {
        QStringLiteral("Undo"), QStringLiteral("Redo"), QStringLiteral(""),
        QStringLiteral("Cut"), QStringLiteral("Copy"), QStringLiteral("Paste"),
        QStringLiteral("PasteWithoutFormat"), QStringLiteral("Delete"), QStringLiteral(""),
        QStringLiteral("SelectAll"), QStringLiteral(""),
        QStringLiteral("Open"), QStringLiteral("Save"), QStringLiteral("Find"), QStringLiteral(""),
        QStringLiteral("Base64Encode"), QStringLiteral("URLEncode"), QStringLiteral(""),
        QStringLiteral("Base64Decode"), QStringLiteral("URLDecode")
    };

    if (const ApplicationSettings *settings = app->getSettings(); settings->contains("Gui/EditorContextMenu")) {
        const QStringList rawNames = settings->value("Gui/EditorContextMenu").toStringList();
        actionNames.clear();
        for (const QString &rawName : rawNames) {
            actionNames.append(rawName.trimmed());
        }
    }

    if (const URLFinder *urlFinder = editor->findChild<URLFinder *>(QString(), Qt::FindDirectChildrenOnly);
        urlFinder && urlFinder->isEnabled() && urlFinder->isURL(clickPos)) {
        actionNames.prepend("");
        actionNames.prepend("CopyURL");
    }
    return actionNames;
}
} // namespace ContextMenuHelpers
using namespace ContextMenuHelpers;

void MainWindow::addEditor(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    detectLanguage(editor);

    // Note: Save point, rename, and UI notifications are dispatched per active editor instance.
    connect(editor, &ScintillaNext::savePointChanged, this, [this, editor]() { updateSaveStatusBasedUi(editor); });
    connect(editor, &ScintillaNext::renamed, this, [this, editor]() { detectLanguage(editor); });
    connect(editor, &ScintillaNext::renamed, this, [this, editor]() { updateFileStatusBasedUi(editor); });
    connect(editor, &ScintillaNext::updateUi, this, &MainWindow::updateDocumentBasedUi);

    // Scintilla pastes the primary selection on middle-click. Suppress an
    // accidental wheel-button press while Shift is held for horizontal scrolling.
    editor->viewport()->installEventFilter(shiftMiddleClickBlocker);

    // Translate shift+vertical wheel to vertical
    editor->viewport()->installEventFilter(shiftWheelToHorizontalScrollFilter);

    // Watch for any zoom events (Ctrl+Scroll or pinch-to-zoom (Qt translates it as Ctrl+Scroll)) so that the event
    // can be handled before the ScintillaEditBase widget, so that it can be applied to all editors to keep zoom level equal.
    // NOTE: Need to install this on the scroll area's viewport, not on the editor widget itself...that was painful to learn
    editor->viewport()->installEventFilter(zoomEventWatcher);

    editor->setZoom(zoomLevel);

    editor->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(editor, &ScintillaNext::customContextMenuRequested, this, [this, editor](const QPoint &pos) {
        contextMenuPos = static_cast<int>(editor->positionFromPoint(pos.x(), pos.y()));
        if (editor->selectionFromPoint(pos.x(), pos.y()) == -1) {
            editor->setEmptySelection(contextMenuPos);
        }

        auto menu = buildMenu(resolveContextMenuActionNames(app, editor, contextMenuPos));
        menu->addMenu(ui->menuConvertCase);
        menu->addSeparator();
        menu->addMenu(ui->menuMarkAllOccurrences);
        menu->addMenu(ui->menuClearMarks);

        for (QAction *action : menu->actions()) {
            action->setIconVisibleInMenu(true);
            if (action->menu()) {
                for (QAction *subAction : action->menu()->actions()) {
                    subAction->setIconVisibleInMenu(true);
                }
            }
        }

        menu->popup(QCursor::pos());
    });

    // The editor has been entirely configured at this point, so add it to the docked editor
    dockedEditor->addEditor(editor);
}

void MainWindow::checkForUpdates(bool silent)
{
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    qInfo(Q_FUNC_INFO);

    const auto url = QStringLiteral("https://github.com/alsyundawy/NotepadNext-MacOS/raw/master/updates.json");

#if defined(Q_OS_MACOS)
#  if defined(__aarch64__) || defined(_M_ARM64)
    QSimpleUpdater::getInstance()->setPlatformKey(url, QStringLiteral("macos-arm64"));
#  else
    QSimpleUpdater::getInstance()->setPlatformKey(url, QStringLiteral("macos-x64"));
#  endif
#elif defined(_M_ARM64) || defined(__aarch64__)
    QSimpleUpdater::getInstance()->setPlatformKey(url, QStringLiteral("windows-arm64"));
#endif

    QSimpleUpdater::getInstance()->setNotifyOnUpdate(url, false);
    QSimpleUpdater::getInstance()->setNotifyOnFinish(url, false);
    QSimpleUpdater::getInstance()->checkForUpdates(url);

    if (!silent) {
        connect(QSimpleUpdater::getInstance(), &QSimpleUpdater::checkingFinished, this, &MainWindow::checkForUpdatesFinished, Qt::UniqueConnection);
    }
    else {
        connect(QSimpleUpdater::getInstance(), &QSimpleUpdater::checkingFinished, this, [this](const QString &finishUrl) {
            if (QSimpleUpdater::getInstance()->getUpdateAvailable(finishUrl)) {
                checkForUpdatesFinished(finishUrl);
            }
        }, Qt::UniqueConnection);
    }

    app->getSettings()->setValue(QStringLiteral("App/LastUpdateCheck"), QDateTime::currentDateTime());
#else
    Q_UNUSED(silent);
#endif
}

void MainWindow::checkForUpdatesFinished(const QString &url)
{
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    const bool available = QSimpleUpdater::getInstance()->getUpdateAvailable(url);
    const QString latestVer = QSimpleUpdater::getInstance()->getLatestVersion(url);
    const auto currentVer = QStringLiteral("0.15-07");
    const QString downloadUrl = QSimpleUpdater::getInstance()->getDownloadUrl(url);
    const QString changelog = QSimpleUpdater::getInstance()->getChangelog(url);
    const QString openUrl = QSimpleUpdater::getInstance()->getOpenUrl(url);

    UpdateDialog dlg(available, latestVer, currentVer, downloadUrl, changelog, openUrl, this);
    dlg.exec();
#else
    Q_UNUSED(url);
#endif
}

void MainWindow::initUpdateCheck()
{
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    connect(ui->actionCheckForUpdates, &QAction::triggered, this, [this]() {
        checkForUpdates(false);
    });
    ui->actionCheckForUpdates->setEnabled(true);
    ui->actionCheckForUpdates->setVisible(true);

    // A bit after startup, see if we need to automatically check for an update
    QTimer::singleShot(15000, this, [this]() {
        ApplicationSettings settings;
        QDateTime dt = settings.value(QStringLiteral("App/LastUpdateCheck"), QDateTime::currentDateTime()).toDateTime();

        if (dt.isValid()) {
            qInfo("Last checked for updates at: %s", qUtf8Printable(dt.toString()));

            if (dt.addDays(7) < QDateTime::currentDateTime()) {
                checkForUpdates(true);
            }
        }
    });
#else
    ui->actionCheckForUpdates->setDisabled(true);
    ui->actionCheckForUpdates->setVisible(false);
#endif
}

void MainWindow::closeEvent(QCloseEvent *closeEvent)
{
    bringWindowToForeground();

    if (!checkEditorsBeforeClose(editors())) {
        closeEvent->ignore();
        return;
    }

    emit aboutToClose();

    closeEvent->accept();

    QMainWindow::closeEvent(closeEvent);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qInfo(Q_FUNC_INFO);

    // NOTE: for urls these can be dragged anywhere in the application, editor, tabs, menu
    // because the ScintillaNext editor ignores urls so they can be handled by the main
    // application
    // Text dragging within the editor object itself is handled by Scintilla, but if the text
    // is dragged to other parts (tabs, menu, etc) it will be handled by the application to
    // create a new editor from the text.

    // Accept urls and text
    if (event->mimeData()->hasUrls() || event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
    else {
        event->ignore();
    }
}

static void collectFilesFromUrl(const QUrl &url, QStringList &fileNames)
{
    if (!url.isLocalFile()) {
        return;
    }
    const QFileInfo info(url.toLocalFile());
    if (!info.exists()) {
        return;
    }
    if (info.isDir()) {
        QDirIterator it(url.toLocalFile(), QDir::Files, QDirIterator::FollowSymlinks | QDirIterator::Subdirectories);
        while (it.hasNext()) {
            fileNames << it.next();
        }
    } else {
        fileNames.append(url.toLocalFile());
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    qInfo(Q_FUNC_INFO);

    if (event->mimeData()->hasUrls()) {
        QStringList fileNames;
        for (const QUrl &url : event->mimeData()->urls()) {
            collectFilesFromUrl(url, fileNames);
        }

        openFileList(fileNames);
        bringWindowToForeground();
        event->acceptProposedAction();
        return;
    }

    if (event->mimeData()->hasText()) {
        if (event->source()) {
            if (auto *sn = qobject_cast<ScintillaNext *>(event->source())) {
                sn->replaceSel("");
            }
        }

        newFile();
        currentEditor()->setText(event->mimeData()->text().toLocal8Bit().constData());
        bringWindowToForeground();
        event->acceptProposedAction();
        return;
    }

    event->ignore();
}

QMenu *MainWindow::buildMenu(const QStringList &actionNames)
{
    auto *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    ActionUtils::populateActionContainer(menu, this, actionNames);

    for (QAction *action : menu->actions()) {
        action->setIconVisibleInMenu(true);
    }

    return menu;
}

void MainWindow::tabBarRightClicked(const ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    // Focus on the correct tab
    dockedEditor->switchToEditor(editor);

    // Default actions
    QStringList actionNames{
        "Close",
        "CloseAllExceptActive",
        "CloseAllToLeft",
        "CloseAllToRight",
        "",
        "Save",
        "SaveAs",
        "Rename",
        "",
        "Reload",
        "",
#ifdef Q_OS_WIN
        "ShowInExplorer",
        "OpenTerminalHere",
        "",
#endif
        "CopyFullPath",
        "CopyFileName",
        "CopyFileDirectory"
    };

    // If the entry exists in the settings, use that
    if (const ApplicationSettings *settings = app->getSettings(); settings->contains("Gui/TabBarContextMenu")) {
        actionNames = settings->value("Gui/TabBarContextMenu").toStringList();
    }

    buildMenu(actionNames)->popup(QCursor::pos());
}

void MainWindow::languageMenuTriggered() const
{
    const auto *act = qobject_cast<const QAction *>(sender());
    if (!act) {
        return;
    }
    auto *editor = currentEditor();
    const QVariant v = act->data();

    setLanguage(editor, v.toString());
}

void MainWindow::sortLinesNatural(bool ascending, bool numericMode) const
{
    ScintillaNext *editor = currentEditor();
    if (!editor) {
        return;
    }

    int startPos = 0;
    int endPos = 0;
    if (editor->selectionEmpty()) {
        startPos = 0;
        endPos = static_cast<int>(editor->length());
    } else {
        const auto selStart = editor->selectionStart();
        const auto selEnd = editor->selectionEnd();
        const auto startLine = editor->lineFromPosition(selStart);
        const auto endLine = editor->lineFromPosition(selEnd);
        startPos = static_cast<int>(editor->positionFromLine(startLine));
        endPos = static_cast<int>(editor->lineEndPosition(endLine));
    }

    if (startPos >= endPos) {
        return;
    }

    const QByteArray textBytes = editor->textRange(startPos, endPos);
    QString text = QString::fromUtf8(textBytes);
    const QString eol = text.contains(QStringLiteral("\r\n")) ? QStringLiteral("\r\n") : QStringLiteral("\n");
    QStringList lines = text.split(eol);

    QCollator collator;
    collator.setNumericMode(numericMode);
    collator.setCaseSensitivity(Qt::CaseInsensitive);

    std::ranges::stable_sort(lines, [&collator, ascending](const QString &a, const QString &b) {
        int cmp = collator.compare(a, b);
        return ascending ? (cmp < 0) : (cmp > 0);
    });

    const QString sortedText = lines.join(eol);
    const QByteArray sortedBytes = sortedText.toUtf8();

    editor->setTargetStart(startPos);
    editor->setTargetEnd(endPos);
    editor->replaceTarget(sortedBytes.size(), sortedBytes.constData());
}

void MainWindow::evaluateMathExpression() const
{
    ScintillaNext *editor = currentEditor();
    if (!editor) {
        return;
    }

    QString expr;
    int endPos = 0;

    if (editor->selectionEmpty()) {
        const auto curPos = editor->currentPos();
        const auto line = editor->lineFromPosition(curPos);
        const auto startPos = static_cast<int>(editor->positionFromLine(line));
        endPos = static_cast<int>(editor->lineEndPosition(line));
        expr = QString::fromUtf8(editor->textRange(startPos, endPos)).trimmed();
    } else {
        const auto startPos = static_cast<int>(editor->selectionStart());
        endPos = static_cast<int>(editor->selectionEnd());
        expr = QString::fromUtf8(editor->textRange(startPos, endPos)).trimmed();
    }

    if (expr.isEmpty()) {
        return;
    }

    QString cleanExpr = expr;
    if (cleanExpr.endsWith(QLatin1Char('='))) {
        cleanExpr.chop(1);
        cleanExpr = cleanExpr.trimmed();
    }

    const QString luaScript = QStringLiteral("return tostring(%1)").arg(cleanExpr);
    const auto result = app->getLuaState()->executeAndReturn<QString>(luaScript);

    if (!result.isEmpty()) {
        const QString insertion = expr.endsWith(QLatin1Char('=')) ? QStringLiteral(" ") + result : QStringLiteral(" = ") + result;
        editor->insertText(endPos, insertion.toUtf8().constData());
        editor->setSelection(endPos, endPos + insertion.toUtf8().size());
    }
}

void MainWindow::compareOpenBuffers()
{
    const auto allEditors = editors();
    if (allEditors.size() < 2) {
        return;
    }

    ScintillaNext *ed1 = currentEditor();
    ScintillaNext *ed2 = (allEditors[0] == ed1) ? allEditors[1] : allEditors[0];

    if (!ed1 || !ed2) {
        return;
    }

    // Split side-by-side view to compare files
    dockedEditor->splitToRight(ed2);

    const auto lines1 = ed1->lineCount();
    const auto lines2 = ed2->lineCount();
    const auto maxLines = std::max(lines1, lines2);

    for (sptr_t line = 0; line < maxLines; ++line) {
        const QByteArray t1 = (line < lines1) ? ed1->getLine(line) : QByteArray();
        const QByteArray t2 = (line < lines2) ? ed2->getLine(line) : QByteArray();
        if (t1 != t2) {
            if (line < lines1) {
                const auto pStart = static_cast<int>(ed1->positionFromLine(line));
                const auto pEnd = static_cast<int>(ed1->lineEndPosition(line));
                ed1->setIndicatorCurrent(INDICATOR_CONTAINER);
                ed1->indicatorFillRange(pStart, pEnd - pStart);
            }
            if (line < lines2) {
                const auto pStart = static_cast<int>(ed2->positionFromLine(line));
                const auto pEnd = static_cast<int>(ed2->lineEndPosition(line));
                ed2->setIndicatorCurrent(INDICATOR_CONTAINER);
                ed2->indicatorFillRange(pStart, pEnd - pStart);
            }
        }
    }
}

void MainWindow::toggleHexView() const
{
    if (ScintillaNext *editor = currentEditor()) {
        editor->toggleHexView();
    }
}

void MainWindow::setupSystemTray()
{
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        m_systemTrayIcon = new QSystemTrayIcon(this);
        m_systemTrayIcon->setIcon(windowIcon());
        m_systemTrayIcon->setToolTip(QStringLiteral("Notepad Next (macOS Edition)"));
        auto *trayMenu = new QMenu(this);
        trayMenu->addAction(ui->actionNew);
        trayMenu->addAction(ui->actionOpen);
        trayMenu->addSeparator();
        const auto *actionToggleWindow = trayMenu->addAction(tr("Show / Hide Notepad Next"));
        connect(actionToggleWindow, &QAction::triggered, this, [this]() {
            if (isVisible()) {
                hide();
            } else {
                show();
                raise();
                activateWindow();
            }
        });
        trayMenu->addSeparator();
        trayMenu->addAction(ui->actionExit);
        m_systemTrayIcon->setContextMenu(trayMenu);
        m_systemTrayIcon->show();
    }
}
