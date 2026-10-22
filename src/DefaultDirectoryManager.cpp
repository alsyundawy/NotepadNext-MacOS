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

#include "DefaultDirectoryManager.h"
#include "ApplicationSettings.h"

DefaultDirectoryManager::DefaultDirectoryManager(MainWindow *window, ApplicationSettings *settings)
    : QObject{window}, window(window), settings(settings)
{
    connect(window, &MainWindow::editorActivated, this, &DefaultDirectoryManager::editorActivated);
    connect(window, &MainWindow::fileDialogAccepted, this, &DefaultDirectoryManager::fileDialogAccepted);
}

QString DefaultDirectoryManager::getDefaultDirectory() const
{
    ApplicationSettings::DefaultDirectoryBehaviorEnum e = settings->defaultDirectoryBehavior();

    if (e == ApplicationSettings::FollowCurrentDocument){
        const ScintillaNext *editor = window->currentEditor();
        return editor->isFile() ? editor->getPath() : settings->defaultDirectory();
    }
    else if (e == ApplicationSettings::RememberLastUsed) {
        return settings->defaultDirectory();
    }
    else if (e == ApplicationSettings::HardCoded) {
        return settings->defaultDirectory();
    }

    return QString();
}

void DefaultDirectoryManager::editorActivated(ScintillaNext *editor)
{
    if (settings->defaultDirectoryBehavior() == ApplicationSettings::FollowCurrentDocument && editor->isFile()) {
        settings->setDefaultDirectory(editor->getPath());
    }
}

void DefaultDirectoryManager::fileDialogAccepted(const QString &filePath)
{
    if (settings->defaultDirectoryBehavior() == ApplicationSettings::RememberLastUsed && !filePath.isEmpty()) {
        settings->setDefaultDirectory(QFileInfo(filePath).absolutePath());
    }
}
