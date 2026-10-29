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
#include <QApplication>
#include <QEvent>
#include <QHelpEvent>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QToolTip>
#include <QWidget>

#ifdef __APPLE__
#include <objc/message.h>
#include <objc/runtime.h>

static void makeNativeWindowTranslucent(QWidget *w) {
  if (!w) return;
  auto view = reinterpret_cast<id>(w->winId());
  if (!view) return;

  SEL windowSel = sel_registerName("window");
  using WindowSend = id (*)(id, SEL);
  id window = reinterpret_cast<WindowSend>(objc_msgSend)(view, windowSel);
  if (!window) return;

  SEL setOpaqueSel = sel_registerName("setOpaque:");
  using SetOpaqueSend = void (*)(id, SEL, BOOL);
  reinterpret_cast<SetOpaqueSend>(objc_msgSend)(window, setOpaqueSel, static_cast<BOOL>(0));

  SEL clearColorSel = sel_registerName("clearColor");
  Class nsColorClass = objc_getClass("NSColor");
  if (nsColorClass) {
    using ClearColorSend = id (*)(Class, SEL);
    id clearColor = reinterpret_cast<ClearColorSend>(objc_msgSend)(nsColorClass, clearColorSel);

    SEL setBgSel = sel_registerName("setBackgroundColor:");
    using SetBgSend = void (*)(id, SEL, id);
    reinterpret_cast<SetBgSend>(objc_msgSend)(window, setBgSel, clearColor);
  }

  SEL invShadowSel = sel_registerName("invalidateShadow");
  using InvShadowSend = void (*)(id, SEL);
  reinterpret_cast<InvShadowSend>(objc_msgSend)(window, invShadowSel);
}
#endif

static void configureTooltipWidget(QWidget *widget) {
  if (!widget) return;
  widget->setAttribute(Qt::WA_TranslucentBackground, true);
#ifdef __APPLE__
  makeNativeWindowTranslucent(widget);
#endif
  if (auto *lbl = qobject_cast<QLabel *>(widget)) {
    lbl->setMargin(0);
  }
}

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

        // Ensure macOS top-level tooltip window is translucent and margin-free
        // so that rounded corners clip cleanly and the box tightly fits the font size.
        const auto topWidgets = QApplication::topLevelWidgets();
        for (QWidget *topWidget : topWidgets) {
          if (topWidget && (topWidget->inherits("QTipLabel") ||
              (topWidget->windowFlags() & Qt::WindowType_Mask) == Qt::ToolTip)) {
            configureTooltipWidget(topWidget);
          }
        }
        return true;
      }
    }
  } else if (event->type() == QEvent::Show || event->type() == QEvent::Polish) {
    if (auto *widget = qobject_cast<QWidget *>(obj)) {
      if (widget->inherits("QTipLabel") ||
          (widget->windowFlags() & Qt::WindowType_Mask) == Qt::ToolTip) {
        configureTooltipWidget(widget);
      }
    }
  }

  return QObject::eventFilter(obj, event);
}


