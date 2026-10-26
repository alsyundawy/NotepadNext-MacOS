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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGlobal>

#ifndef QT_VERSION_CHECK
#define QT_VERSION_CHECK(major, minor, patch) (((major)<<16)|((minor)<<8)|(patch))
#endif

#include <QMainWindow>
#include <QLabel>
#include <QActionGroup>
#include <memory>

#include "DockedEditor.h"

#include "MacroManager.h"
#include "ScintillaNext.h"
#include "NppImporter.h"
#include "SearchResultsCollector.h"

namespace Ui {
class MainWindow;
}

class NotepadNextApplication;
class QSystemTrayIcon;
class Macro;
class Settings;
class QuickFindWidget;
class ZoomEventWatcher;
class ShiftMiddleClickBlocker;
class ShiftWheelToHorizontalScrollFilter;
class Converter;
class DefaultDirectoryManager;
class TabsQuickActionsBar;

class MainWindow : public QMainWindow // NOSONAR
{
    Q_OBJECT

    friend class ThemeManager;

public:
    explicit MainWindow(NotepadNextApplication *app);
    ~MainWindow() override;

    bool isAnyUnsaved() const;

    void setupLanguageMenu();
    void setupThemeMenu();
    ScintillaNext *currentEditor() const;
    int editorCount() const;
    QVector<ScintillaNext *> editors() const;
    DockedEditor *getDockedEditor() const { return dockedEditor; }

public slots:
    void newFile() const;
    void newWindow();

    void openFileDialog();
    void openFile(const QString &filePath);

    void openFolderAsWorkspaceDialog();
    void setFolderAsWorkspacePath(const QString &dir) const;

    void reloadFile();

    void closeCurrentFile();
    void closeFile(ScintillaNext *editor);
    void closeAllFiles();
    void closeAllExceptActive();
    void closeAllToLeft();
    void closeAllToRight();

    bool saveCurrentFile();
    bool saveFile(ScintillaNext *editor);

    bool saveCurrentFileAsDialog();
    bool saveCurrentFileAs(const QString &fileName);
    bool saveFileAs(ScintillaNext *editor, const QString &fileName);

    bool saveCopyAsDialog();
    bool saveCopyAs(const QString &fileName);
    bool saveAll();
    bool saveAllEditors(const QVector<ScintillaNext *> &editors);

    void exportAsFormat(Converter *converter, const QString &filter);
    void copyAsFormat(Converter *converter, const QString &mimeType) const;

    void renameFile();

    void moveCurrentFileToTrash();
    void moveFileToTrash(const ScintillaNext *editor);

    void print();

    void convertEOLs(int eolMode);
    void titleCase() const;
    void sentenceCase() const;
    void invertCase() const;
    void pasteWithoutFormat() const;

    void sortLinesNatural(bool ascending = true, bool numericMode = true) const;
    void evaluateMathExpression() const; // actionEvaluateMath inline math evaluator
    void compareOpenBuffers();
    void toggleHexView() const;
    void setupSystemTray();

    void showFindReplaceDialog(int index);

    void updateFileStatusBasedUi(const ScintillaNext *editor);
    void updateEOLBasedUi(const ScintillaNext *editor);
    void updateDocumentBasedUi(Scintilla::Update updated);
    void updateSelectionBasedUi(ScintillaNext *editor);
    void updateContentBasedUi(ScintillaNext *editor);
    void updateSaveStatusBasedUi(const ScintillaNext *editor);
    void updateEditorPositionBasedUi();
    void updateLanguageBasedUi(const ScintillaNext *editor) const;
    void updateGui(ScintillaNext *editor);

    void detectLanguage(ScintillaNext *editor) const;

    void setLanguage(ScintillaNext *editor, const QString &languageName) const;

    void bringWindowToForeground();
    void focusIn();

    void addEditor(ScintillaNext *editor);

    void checkForUpdates(bool silent = false);

    void restoreWindowState();

    void switchToEditor(const ScintillaNext *editor);

signals:
    void editorActivated(ScintillaNext *editor);
    void aboutToClose();
    void fileDialogAccepted(const QString &filePath);

protected:
    void closeEvent(QCloseEvent *closeEvent) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void changeEvent(QEvent *changeEvent) override;

private slots:
    void tabBarRightClicked(const ScintillaNext *editor);
    void languageMenuTriggered() const;
    void checkForUpdatesFinished(const QString &url);
    void activateEditor(ScintillaNext *editor);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    NotepadNextApplication *app = Q_NULLPTR;
    DockedEditor *dockedEditor = Q_NULLPTR;

    QScopedPointer<SearchResultsCollector> searchResults;

    template <typename Method>
    void connectEditorAction(QAction* action, Method method) {
        connect(action, &QAction::triggered, this, [this, method]() {
            if (currentEditor()) {
                (currentEditor()->*method)();
            }
        });
    }
    template <typename Method, typename Arg>
    void connectEditorAction(QAction* action, Method method, Arg value) {
        connect(action, &QAction::triggered, this, [this, method, value]() {
            if (currentEditor()) {
                (currentEditor()->*method)(value);
            }
        });
    }
    void applyStyleSheet();
    void applyCustomShortcuts();
    void initUpdateCheck();
    ScintillaNext *getInitialEditor();
    void openFileList(const QStringList &fileNames);
    bool checkEditorsBeforeClose(const QVector<ScintillaNext *> &editorList);
    bool checkFileForModification(ScintillaNext *editor);
    void showSaveErrorMessage(const ScintillaNext *editor, QFileDevice::FileError error);
    void showEditorZoomLevelIndicator() const;

    enum class UserSaveAction { SaveAll, DiscardAll, Cancel };
    UserSaveAction promptForSave(const QVector<ScintillaNext *> &editorList);

    void saveSettings() const;
    void restoreSettings();
    QStringList currentDockObjectNames() const;

    ISearchResultsHandler *determineSearchResultsHandler();

    QActionGroup *languageActionGroup;

    TabsQuickActionsBar *tabsQuickActionsBar = Q_NULLPTR;

    MacroManager macroManager;
    DefaultDirectoryManager *defaultDirectoryManager;

    ZoomEventWatcher *zoomEventWatcher;
    ShiftMiddleClickBlocker *shiftMiddleClickBlocker;
    ShiftWheelToHorizontalScrollFilter *shiftWheelToHorizontalScrollFilter;
    int zoomLevel = 0;
    int contextMenuPos = 0;
    QMenu *buildMenu(const QStringList &actionNames);

    QMenu *menuTheme = Q_NULLPTR;
    QAction *actionThemeSwitcher = Q_NULLPTR;
    QAction *actionThemeLight = Q_NULLPTR;
    QAction *actionThemeDark = Q_NULLPTR;
    QAction *actionToggleTheme = Q_NULLPTR;
    QSystemTrayIcon *m_systemTrayIcon = Q_NULLPTR;
};

#endif // MAINWINDOW_H
