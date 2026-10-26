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

#ifndef EDITORCONFIGAPPDECORATOR_H
#define EDITORCONFIGAPPDECORATOR_H

#include "ApplicationDecorator.h"

class ScintillaNext;

class EditorConfigAppDecorator : public ApplicationDecorator
{
    Q_OBJECT

public:
    explicit EditorConfigAppDecorator(NotepadNextApplication *app);

private slots:
    void doEditorConfig(ScintillaNext *editor) const;

    void trimTrailingWhitespace() const;
    void ensureFinalNewline() const;
    void ensureNoFinalNewline() const;
};

#endif // EDITORCONFIGAPPDECORATOR_H
