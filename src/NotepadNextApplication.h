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

#ifndef NOTEPADNEXTAPPLICATION_H
#define NOTEPADNEXTAPPLICATION_H

#include "ApplicationSettings.h"

#include "SingleApplication"

#include <QCommandLineParser>
#include <QPointer>
#include <QTimer>
#include <memory>


class MainWindow;
class LuaState;
class EditorManager;
class RecentFilesListManager;
class ScintillaNext;
class SessionManager;
class TranslationManager;


class NotepadNextApplication : public SingleApplication
{
    Q_OBJECT

public:
    NotepadNextApplication(int &argc, char **argv);
    ~NotepadNextApplication() override;

    bool init();

    RecentFilesListManager *getRecentFilesListManager() const { return recentFilesListManager; }
    EditorManager *getEditorManager() const { return editorManager; }
    SessionManager *getSessionManager() const;
    TranslationManager *getTranslationManager() const { return translationManager; }

    LuaState *getLuaState() const { return luaState.get(); }
    QString getFileDialogFilter() const;
    QString getFileDialogFilterForLanguage(const QString &language) const;
    ApplicationSettings *getSettings() const { return settings; }

    QStringList getLanguages() const;
    void setEditorLanguage(ScintillaNext *editor, const QString &languageName) const;
    QStringList getLanguageKeywords(const QString &languageName) const;

    QString detectLanguage(ScintillaNext *editor) const;
    QString detectLanguageFromExtension(const QString &extension) const;
    QString detectLanguageFromContents(ScintillaNext *editor) const;

    void sendInfoToPrimaryInstance();

    bool isRunningAsAdmin() const;

    QStringList debugInfo() const;

    MainWindow *createNewWindow();
    MainWindow *activeMainWindow() const;

protected:
    bool event(QEvent *e) override;

private slots:
    void saveSettings() const;
    void receiveInfoFromSecondaryInstance(quint32 instanceId, QByteArray message);
    void saveSession();

private:
    void openFiles(const QStringList &files);

    void loadSettings();

    EditorManager *editorManager = nullptr;
    RecentFilesListManager *recentFilesListManager = nullptr;
    ApplicationSettings *settings = nullptr;
    SessionManager *sessionManager = nullptr;
    TranslationManager *translationManager = nullptr;

    std::unique_ptr<LuaState> luaState;

    QList<QPointer<MainWindow>> m_windows;
    QPointer<QWidget> currentlyFocusedWidget; // Keep a weak pointer to the QWidget since we don't own it

    QCommandLineParser parser;

    QTimer autoSaveTimer; //save automatically the session
};

#endif // NOTEPADNEXTAPPLICATION_H
