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

#ifndef URLFINDER_H
#define URLFINDER_H

#include "EditorDecorator.h"

class URLFinder : public EditorDecorator
{
    Q_OBJECT

public:
    explicit URLFinder(ScintillaNext *editor);
    bool isURL(int position) const;
    void copyURLToClipboard(int position) const;

private slots:
    void findURLs();
    void clearURLs();

public slots:
    void notify(const Scintilla::NotificationData *pscn) override;

private:
    QTimer *timer;
    int indicator;
};

#endif // URLFINDER_H
