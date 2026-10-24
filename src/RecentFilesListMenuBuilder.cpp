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

#include "RecentFilesListMenuBuilder.h"

#include <QAction>
#include <QMenu>
#include <QDir>

RecentFilesListMenuBuilder::RecentFilesListMenuBuilder(RecentFilesListManager *manager) :
    QObject(manager),
    manager(manager)
{
}

void RecentFilesListMenuBuilder::populateMenu(QMenu *menu) const
{
    int i = 0;

    QList<QAction *> recentFileListActions;
    for (const QString &file : manager->fileList()) {
        ++i;
        auto *action = new QAction(QString("%1%2: %3").arg(i < 10 ? "&" : "").arg(i).arg(QDir::toNativeSeparators(file)), menu);

        action->setData(file);
        connect(action, &QAction::triggered, const_cast<RecentFilesListMenuBuilder *>(this), &RecentFilesListMenuBuilder::recentFileActionTriggered);

        recentFileListActions.append(action);
    }

    menu->addActions(recentFileListActions);
}

void RecentFilesListMenuBuilder::recentFileActionTriggered()
{
    qInfo(Q_FUNC_INFO);

    const QAction *action = qobject_cast<const QAction *>(sender());
    emit fileOpenRequest(action->data().toString());
}
