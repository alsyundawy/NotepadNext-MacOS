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

#include "ToolbarTooltipFilter.h"

#include <QAction>
#include <QEvent>
#include <QHelpEvent>
#include <QToolBar>
#include <QToolButton>
#include <QToolTip>

ToolbarTooltipFilter::ToolbarTooltipFilter(QObject *parent) : QObject(parent) {}

bool ToolbarTooltipFilter::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::ToolTip) {
    auto *button = qobject_cast<QToolButton *>(obj);
    if (button && qobject_cast<QToolBar *>(button->parentWidget())) {
      QAction *action = button->defaultAction();
      QString tip;
      if (action) {
        tip = action->toolTip();
        if (tip.isEmpty() || tip == action->text()) {
          tip = action->text();
        }
      } else {
        tip = button->toolTip();
        if (tip.isEmpty()) {
          tip = button->text();
        }
      }

      // Remove accelerator mnemonics (e.g., "&Replace..." -> "Replace...")
      tip.remove(QLatin1Char('&'));

      if (!tip.isEmpty()) {
        auto *helpEvent = static_cast<QHelpEvent *>(event);
        QToolTip::showText(helpEvent->globalPos(), tip, button);
        return true;
      }
    }
  }

  return QObject::eventFilter(obj, event);
}
