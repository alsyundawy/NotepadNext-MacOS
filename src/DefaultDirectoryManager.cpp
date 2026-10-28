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
    using enum ApplicationSettings::DefaultDirectoryBehaviorEnum;

    if (const auto behavior = settings->defaultDirectoryBehavior(); behavior == FollowCurrentDocument) {
        if (const ScintillaNext *editor = window->currentEditor(); editor && editor->isFile()) {
            return editor->getPath();
        }
        return settings->defaultDirectory();
    }
    else if (behavior == RememberLastUsed || behavior == HardCoded) {
        return settings->defaultDirectory();
    }

    return QString();
}

void DefaultDirectoryManager::editorActivated(const ScintillaNext *editor)
{
    using enum ApplicationSettings::DefaultDirectoryBehaviorEnum;
    if (settings->defaultDirectoryBehavior() == FollowCurrentDocument && editor && editor->isFile()) {
        settings->setDefaultDirectory(editor->getPath());
    }
}

void DefaultDirectoryManager::fileDialogAccepted(const QString &filePath)
{
    using enum ApplicationSettings::DefaultDirectoryBehaviorEnum;
    if (settings->defaultDirectoryBehavior() == RememberLastUsed && !filePath.isEmpty()) {
        settings->setDefaultDirectory(QFileInfo(filePath).absolutePath());
    }
}
