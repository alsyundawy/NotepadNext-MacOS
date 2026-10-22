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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H


#include <QDir>
#include <QSettings>


class ScintillaNext;
class MainWindow;
class NotepadNextApplication;

class SessionManager
{
public:
    enum SessionFileType {
        None = 0,
        SavedFile = 1,
        UnsavedFile = 2,
        TempFile = 4,
    };
    Q_DECLARE_FLAGS(SessionFileTypes, SessionFileType)


    explicit SessionManager(NotepadNextApplication *app, SessionFileTypes types = SessionFileTypes());

    void setSessionFileTypes(SessionFileTypes types);

    static void clear();

    void saveSession(MainWindow *window);
    void loadSession(MainWindow *window);

    bool willFileGetStoredInSession(const ScintillaNext *editor) const;

private:
    static QDir sessionDirectory();

    static void saveIntoSessionDirectory(ScintillaNext *editor, const QString &sessionFileName);

    static SessionFileType determineType(const ScintillaNext *editor);

    static void clearSettings();
    static void clearDirectory();

    void storeFileDetails(ScintillaNext *editor, QSettings &settings);
    ScintillaNext *loadFileDetails(const QSettings &settings);

    void storeUnsavedFileDetails(ScintillaNext *editor, QSettings &settings);
    ScintillaNext *loadUnsavedFileDetails(const QSettings &settings);

    void storeTempFile(ScintillaNext *editor, QSettings &settings);
    ScintillaNext *loadTempFile(const QSettings &settings);

    void storeEditorViewDetails(ScintillaNext *editor, QSettings &settings);
    void loadEditorViewDetails(ScintillaNext *editor, const QSettings &settings);

    NotepadNextApplication *app;
    SessionFileTypes fileTypes;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(SessionManager::SessionFileTypes)

#endif // SESSIONMANAGER_H
