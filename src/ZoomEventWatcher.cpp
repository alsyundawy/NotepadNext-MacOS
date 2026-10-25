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

#include "ZoomEventWatcher.h"
#include <QtGlobal>

#ifndef QT_VERSION_CHECK
#define QT_VERSION_CHECK(major, minor, patch) (((major)<<16)|((minor)<<8)|(patch))
#endif

#include <QEvent>
#include <QWheelEvent>
#include <QApplication>


static bool isWheelEventHorizontal(const QWheelEvent *event) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return event->angleDelta().y() == 0;
#else
    return event->orientation() == Qt::Horizontal;
#endif
}

static int wheelEventYDelta(const QWheelEvent *event) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return event->angleDelta().y();
#else
    return event->delta();
#endif
}


ZoomEventWatcher::ZoomEventWatcher(QObject *parent)
    : QObject{parent}
{
}

bool ZoomEventWatcher::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        const auto *wheelEvent = static_cast<const QWheelEvent *>(event);

        if (!isWheelEventHorizontal(wheelEvent)) {
            if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
                if (wheelEventYDelta(wheelEvent) > 0) {
                    emit zoomIn();
                } else {
                    emit zoomOut();
                }
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, event);
}