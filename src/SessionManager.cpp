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
#include "ScintillaNext.h"
#include "MainWindow.h"
#include "SessionManager.h"
#include "EditorManager.h"
#include "NotepadNextApplication.h"

#include <QDir>
#include <QStandardPaths>
#include <QUuid>


static QString RandomSessionFileName()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

// QList<int> cannot be automatically serialized to/from QSettings (i.e. QVariant) so turn it to a QVariantList
static QVariantList QListToQVariantList(const QList<int> &intList)
{
    QVariantList vl;
    for (const int i : intList){
        vl.append(i);
    }
    return vl;
}

// Do the opposite of the function above
static QList<int> QVariantListToQList(const QVariantList &variantList) {
    QList<int> intList;
    for (const QVariant &variant : variantList) {
        intList.append(variant.toInt());
    }
    return intList;
}

SessionManager::SessionManager(NotepadNextApplication *app, SessionFileTypes types)
    : app(app)
{
    setSessionFileTypes(types);
}

void SessionManager::setSessionFileTypes(SessionFileTypes types)
{
    fileTypes = types;
}

QDir SessionManager::sessionDirectory()
{
    QDir d(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    if (d.mkpath("session")) {
        (void)d.cd("session");
    }

    return d;
}

void SessionManager::saveIntoSessionDirectory(const ScintillaNext *editor, const QString &sessionFileName)
{
    editor->saveCopyAs(sessionDirectory().filePath(sessionFileName));
}

SessionManager::SessionFileType SessionManager::determineType(const ScintillaNext *editor)
{
    if (editor->isFile()) {
        if (editor->isSavedToDisk()) {
            return SessionManager::SessionFileType::SavedFile;
        }
        else {
            return SessionManager::SessionFileType::UnsavedFile;
        }
    }
    else {
        if (!editor->isSavedToDisk()) {
            return SessionManager::SessionFileType::TempFile;
        }
        else {
            return SessionManager::SessionFileType::None;
        }
    }
}

void SessionManager::clear()
{
    clearSettings();
    clearDirectory();
}

void SessionManager::clearSettings()
{
    ApplicationSettings settings;

    // Clear everything out. There can be left over entries that are no longer needed
    settings.beginGroup("CurrentSession");
    settings.remove("");
}

void SessionManager::clearDirectory()
{
    QDir d = sessionDirectory();

    for (const QString &f : d.entryList()) {
        d.remove(f);
    }
}

void SessionManager::saveSession(const MainWindow *window) const
{
    qInfo(Q_FUNC_INFO);

    clear();

    // Early out if no flags are set
    if (fileTypes == SessionManager::SessionFileType::None) {
        return;
    }

    const ScintillaNext *currentEditor = window->currentEditor();
    int currentEditorIndex = 0;
    ApplicationSettings settings;

    settings.beginGroup("CurrentSession");

    settings.beginWriteArray("OpenedFiles");

    int index = 0;
    for (const auto &editor : window->editors()) {
        SessionFileType editorType = determineType(editor);

        if (fileTypes.testFlag(editorType)) {
            settings.setArrayIndex(index);

            if (editorType == SessionManager::SessionFileType::SavedFile) {
                storeFileDetails(editor, settings);
            }
            else if (editorType == SessionManager::SessionFileType::UnsavedFile) {
                storeUnsavedFileDetails(editor, settings);
            }
            else if (editorType == SessionManager::SessionFileType::TempFile) {
                storeTempFile(editor, settings);
            }
            else {
                qWarning("Unknown SessionFileType %d", static_cast<int>(editorType));
            }

            if (currentEditor == editor) {
                currentEditorIndex = index;
            }

            ++index;
        }
    }

    settings.endArray();

    settings.setValue("CurrentEditorIndex", currentEditorIndex);

    settings.endGroup();
}

void SessionManager::loadSession(MainWindow *window)
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;

    settings.beginGroup("CurrentSession");

    const ScintillaNext *currentEditor = Q_NULLPTR;
    const int currentEditorIndex = settings.value("CurrentEditorIndex").toInt();
    const int size = settings.beginReadArray("OpenedFiles");

    // NOTE: In theory the fileTypes should determine what is loaded, however if the session fileTypes
    // change from the last time it was saved then it means the settings were manually altered outside of the app,
    // which is non-standard behavior, so just load anything in the file

    for (int index = 0; index < size; ++index) {
        settings.setArrayIndex(index);

        const ScintillaNext *editor = Q_NULLPTR;

        if (settings.contains("Type")) {
            if (const QString type = settings.value("Type").toString(); type == QStringLiteral("File")) {
                editor = loadFileDetails(settings);
            }
            else if (type == QStringLiteral("UnsavedFile")) {
                editor = loadUnsavedFileDetails(settings);
            }
            else if (type == QStringLiteral("Temp")) {
                editor = loadTempFile(settings);
            }
            else {
                qDebug("Unknown session entry type: %s", qUtf8Printable(type));
            }

            if (editor && currentEditorIndex == index) {
                currentEditor = editor;
            }
        }
        else {
            qDebug("Unknown session entry type for index %d", index);
        }
    }

    settings.endArray();

    settings.endGroup();

    if (currentEditor) {
        window->switchToEditor(currentEditor);
    }
}

bool SessionManager::willFileGetStoredInSession(const ScintillaNext *editor) const
{
    SessionFileType editorType = determineType(editor);

    // See if the editor type is in the currently supported file types
    return fileTypes.testFlag(editorType);
}

void SessionManager::storeFileDetails(const ScintillaNext *editor, QSettings &settings) const
{
    settings.setValue("Type", "File");
    settings.setValue("FilePath", editor->getFilePath());

    storeEditorViewDetails(editor, settings);
}

ScintillaNext* SessionManager::loadFileDetails(const QSettings &settings)
{
    qInfo(Q_FUNC_INFO);

    const QString filePath = settings.value("FilePath").toString();

    qDebug("Session file: \"%s\"", qUtf8Printable(filePath));

    ScintillaNext *editor = app->getEditorManager()->getEditorByFilePath(filePath);
    if (editor != Q_NULLPTR) {
        qDebug("  file is already open, ignoring");
        return Q_NULLPTR;
    }

    if (QFileInfo::exists(filePath)) {
        editor = ScintillaNext::fromFile(filePath);

        if (editor == Q_NULLPTR) {
            qWarning("  could not be read from disk, ignoring this file for session loading");
            return Q_NULLPTR;
        }

        app->getEditorManager()->manageEditor(editor);

        loadEditorViewDetails(editor, settings);

        return editor;
    }
    else {
        qDebug("  no longer exists on disk, ignoring");
        return Q_NULLPTR;
    }
}

void SessionManager::storeUnsavedFileDetails(const ScintillaNext *editor, QSettings &settings) const
{
    const QString sessionFileName = RandomSessionFileName();

    settings.setValue("Type", "UnsavedFile");
    settings.setValue("FilePath", editor->getFilePath());
    settings.setValue("SessionFileName", sessionFileName);

    storeEditorViewDetails(editor, settings);

    saveIntoSessionDirectory(editor, sessionFileName);
}

ScintillaNext *SessionManager::loadUnsavedFileDetails(const QSettings &settings)
{
    qInfo(Q_FUNC_INFO);

    const QString filePath = settings.value("FilePath").toString();
    const QString sessionFileName = settings.value("SessionFileName").toString();
    const QString sessionFilePath = sessionDirectory().filePath(sessionFileName);

    qDebug("Session file: \"%s\"", qUtf8Printable(filePath));
    qDebug("  temp loc: \"%s\"", qUtf8Printable(sessionFilePath));

    if (const ScintillaNext *existingEditor = app->getEditorManager()->getEditorByFilePath(filePath); existingEditor != Q_NULLPTR) {
        qDebug("  file is already open, ignoring");
        return Q_NULLPTR;
    }

    if (QFileInfo::exists(filePath) && QFileInfo::exists(sessionFilePath)) {
        ScintillaNext *editor = ScintillaNext::fromFile(sessionFilePath);

        if (editor == Q_NULLPTR) {
            qWarning("  could not be read from disk, ignoring this file for session loading");
            return Q_NULLPTR;
        }

        // Since this editor has different file path info, treat this as a temporary buffer
        editor->setFileInfo(filePath);
        editor->setTemporary(true);

        app->getEditorManager()->manageEditor(editor);

        loadEditorViewDetails(editor, settings);

        return editor;
    }
    else {
        // What if just filePath exists?
        qDebug("  no longer exists on disk, ignoring this file for session loading");
        return Q_NULLPTR;
    }
}

void SessionManager::storeTempFile(const ScintillaNext *editor, QSettings &settings) const
{
    const QString sessionFileName = RandomSessionFileName();

    settings.setValue("Type", "Temp");
    settings.setValue("FileName", editor->getName());
    settings.setValue("SessionFileName", sessionFileName);
    settings.setValue("Language", editor->languageName);

    storeEditorViewDetails(editor, settings);

    saveIntoSessionDirectory(editor, sessionFileName);
}

ScintillaNext *SessionManager::loadTempFile(const QSettings &settings)
{
    qInfo(Q_FUNC_INFO);

    const QString fileName = settings.value("FileName").toString();
    const QString sessionFileName = settings.value("SessionFileName").toString();
    const QString languageName = settings.value("Language", QString()).toString();
    const QString fullFilePath = sessionDirectory().filePath(sessionFileName);

    qDebug("Session temp file: \"%s\"", qUtf8Printable(fullFilePath));

    if (QFileInfo::exists(fullFilePath)) {
        ScintillaNext *editor = ScintillaNext::fromFile(fullFilePath, false);

        if (editor == Q_NULLPTR) {
            qWarning("  could not be read from disk, ignoring this file for session loading");
            return Q_NULLPTR;
        }

        editor->detachFileInfo(fileName);
        editor->setTemporary(true);

        app->getEditorManager()->manageEditor(editor);

        loadEditorViewDetails(editor, settings);

        if (!languageName.isEmpty()) {
            qDebug("Setting session file language to \"%s\"", qUtf8Printable(languageName));
            app->setEditorLanguage(editor, languageName);
        }

        return editor;
    }
    else {
        qDebug("  no longer exists on disk, ignoring");
        return Q_NULLPTR;
    }
}

void SessionManager::storeEditorViewDetails(const ScintillaNext *editor, QSettings &settings) const
{
    settings.setValue("FirstVisibleLine", static_cast<int>(editor->firstVisibleLine() + 1)); // Keep it 1-based in the settings just for human-readability
    settings.setValue("CurrentPosition", static_cast<int>(editor->currentPos()));

    const auto *decorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);
    if (decorator == nullptr) {
        qWarning("Editor has no BookMarkDecorator, unable to store bookmarks");
        return;
    }

    QList<int> bookMarkedLines = decorator->bookMarkedLines();
    if (!bookMarkedLines.isEmpty()) {
        settings.setValue("BookMarks", QListToQVariantList(bookMarkedLines));
    }
}

void SessionManager::loadEditorViewDetails(ScintillaNext *editor, const QSettings &settings) const
{
    const int firstVisibleLine = settings.value("FirstVisibleLine").toInt() - 1;
    const int currentPosition = settings.value("CurrentPosition").toInt();

    editor->setFirstVisibleLine(firstVisibleLine);
    editor->setEmptySelection(currentPosition);

    if (settings.contains("BookMarks"))
    {
        QList<int> bookMarkedLines = QVariantListToQList(settings.value("BookMarks").toList()); // just using .value<QList<int>>() does not work...possibly a Qt bug?

        auto *decorator = editor->findChild<BookMarkDecorator*>(QString(), Qt::FindDirectChildrenOnly);
        if (decorator != nullptr) {
            decorator->setBookMarkedLines(bookMarkedLines);
        }
        else {
            qWarning("Editor has no BookMarkDecorator, unable to restore bookmarks");
        }
    }
}
