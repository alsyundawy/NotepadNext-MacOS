/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Implements Monitoring / Tail Mode (tail -f).
 * Adapted from Notepad++ and Notepad4 monitoring architectures.
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "TailMonitoringManager.h"
#include "ScintillaNext.h"

#include <QFileInfo>
#include <QFile>

TailMonitoringManager *TailMonitoringManager::s_instance = nullptr;

TailMonitoringManager *TailMonitoringManager::instance() {
    if (!s_instance) {
        s_instance = new TailMonitoringManager();
    }
    return s_instance;
}

TailMonitoringManager::TailMonitoringManager(QObject *parent)
    : QObject(parent), m_fileWatcher(new QFileSystemWatcher(this)), m_timer(new QTimer(this)) {
    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &TailMonitoringManager::onFileSystemChanged);

    connect(m_timer, &QTimer::timeout, this, &TailMonitoringManager::onHeartbeatTimer);
    m_timer->setInterval(m_pollIntervalMs);
}

bool TailMonitoringManager::isMonitoring(const ScintillaNext *editor) const {
    return editor && m_monitored.contains(const_cast<ScintillaNext *>(editor));
}

bool TailMonitoringManager::toggleMonitoring(ScintillaNext *editor) {
    if (isMonitoring(editor)) {
        stopMonitoring(editor);
        return false;
    } else {
        return startMonitoring(editor);
    }
}

bool TailMonitoringManager::startMonitoring(ScintillaNext *editor) {
    if (!editor)
        return false;

    const QString filePath = editor->getFilePath();
    if (filePath.isEmpty() || !QFile::exists(filePath)) {
        emit statusMessage(tr("Cannot monitor: File is not saved to disk."));
        return false;
    }

    QFileInfo info(filePath);
    MonitoredDocState state;
    state.filePath = filePath;
    state.lastSize = info.size();
    state.lastModified = info.lastModified();
    state.wasReadOnly = editor->readOnly();
    state.autoScrollToEnd = true;

    if (m_readOnlyLock) {
        editor->setReadOnly(true);
    }

    editor->gotoLine(editor->lineCount());
    editor->scrollCaret();

    m_monitored.insert(editor, state);
    m_fileWatcher->addPath(filePath);

    connect(editor, &QObject::destroyed, this, &TailMonitoringManager::onEditorDestroyed, Qt::UniqueConnection);

    if (!m_timer->isActive()) {
        m_timer->start();
    }

    emit monitoringStateChanged(editor, true);
    emit statusMessage(tr("👁️ Monitoring (tail -f) started on %1").arg(info.fileName()));
    return true;
}

void TailMonitoringManager::stopMonitoring(ScintillaNext *editor) {
    if (!editor || !m_monitored.contains(editor))
        return;

    auto state = m_monitored.take(editor);

    bool pathStillUsed = false;
    for (auto it = m_monitored.constBegin(); it != m_monitored.constEnd(); ++it) {
        if (it.value().filePath == state.filePath) {
            pathStillUsed = true;
            break;
        }
    }
    if (!pathStillUsed) {
        m_fileWatcher->removePath(state.filePath);
    }

    if (m_readOnlyLock) {
        editor->setReadOnly(state.wasReadOnly);
    }

    if (m_monitored.isEmpty()) {
        m_timer->stop();
    }

    emit monitoringStateChanged(editor, false);
    QFileInfo info(state.filePath);
    emit statusMessage(tr("Monitoring (tail -f) stopped for %1").arg(info.fileName()));
}

void TailMonitoringManager::stopAll() {
    const auto editors = m_monitored.keys();
    for (auto *ed : editors) {
        stopMonitoring(ed);
    }
}

bool TailMonitoringManager::autoScrollToEnd(const ScintillaNext *editor) const {
    if (!editor || !m_monitored.contains(const_cast<ScintillaNext *>(editor)))
        return true;
    return m_monitored.value(const_cast<ScintillaNext *>(editor)).autoScrollToEnd;
}

void TailMonitoringManager::setAutoScrollToEnd(ScintillaNext *editor, bool enable) {
    if (editor && m_monitored.contains(editor)) {
        m_monitored[editor].autoScrollToEnd = enable;
        if (enable) {
            editor->gotoLine(editor->lineCount());
            editor->scrollCaret();
        }
    }
}

void TailMonitoringManager::setPollIntervalMs(int ms) {
    m_pollIntervalMs = qMax(100, ms);
    m_timer->setInterval(m_pollIntervalMs);
}

void TailMonitoringManager::forceCheck(ScintillaNext *editor) {
    if (editor && m_monitored.contains(editor)) {
        checkEditorFile(editor);
    }
}

void TailMonitoringManager::onHeartbeatTimer() {
    const auto editors = m_monitored.keys();
    for (auto *ed : editors) {
        checkEditorFile(ed);
    }
}

void TailMonitoringManager::onFileSystemChanged(const QString &path) {
    if (QFile::exists(path)) {
        m_fileWatcher->addPath(path);
    }

    for (auto it = m_monitored.begin(); it != m_monitored.end(); ++it) {
        if (it.value().filePath == path) {
            checkEditorFile(it.key());
        }
    }
}

void TailMonitoringManager::onEditorDestroyed(const QObject *obj) {
    const auto *editor = static_cast<const ScintillaNext *>(obj);
    if (m_monitored.contains(const_cast<ScintillaNext *>(editor))) {
        auto state = m_monitored.take(const_cast<ScintillaNext *>(editor));
        bool pathStillUsed = false;
        for (auto it = m_monitored.constBegin(); it != m_monitored.constEnd(); ++it) {
            if (it.value().filePath == state.filePath) {
                pathStillUsed = true;
                break;
            }
        }
        if (!pathStillUsed) {
            m_fileWatcher->removePath(state.filePath);
        }
    }
    if (m_monitored.isEmpty()) {
        m_timer->stop();
    }
}

void TailMonitoringManager::checkEditorFile(ScintillaNext *editor) {
    if (!editor || !m_monitored.contains(editor))
        return;

    auto &state = m_monitored[editor];
    QFileInfo info(state.filePath);
    if (!info.exists()) {
        return;
    }

    const qint64 currentSize = info.size();
    const QDateTime currentMtime = info.lastModified();

    if (currentSize == state.lastSize && currentMtime == state.lastModified) {
        return;
    }

    if (currentSize < state.lastSize || (currentSize - state.lastSize > m_maxChunkBytes)) {
        state.lastSize = 0;
        state.lastModified = currentMtime;
        editor->setReadOnly(false);
        editor->reload();
        if (m_readOnlyLock) {
            editor->setReadOnly(true);
        }
        state.lastSize = info.size();
        if (state.autoScrollToEnd) {
            editor->gotoLine(editor->lineCount());
            editor->scrollCaret();
        }
        emit fileRotatedOrTruncated(editor);
        emit statusMessage(tr("👁️ Tail: File rotated or truncated, reloaded (%1)").arg(info.fileName()));
        return;
    }

    QFile file(state.filePath);
    if (file.open(QIODevice::ReadOnly)) {
        if (file.seek(state.lastSize)) {
            const QByteArray newBytes = file.readAll();
            file.close();

            if (!newBytes.isEmpty()) {
                const int beforeLines = editor->lineCount();
                editor->setReadOnly(false);
                editor->appendText(newBytes.length(), newBytes.constData());
                if (m_readOnlyLock) {
                    editor->setReadOnly(true);
                }

                state.lastSize = currentSize;
                state.lastModified = currentMtime;

                const int addedLines = editor->lineCount() - beforeLines;

                if (state.autoScrollToEnd) {
                    editor->gotoLine(editor->lineCount());
                    editor->scrollCaret();
                }

                emit fileTailed(editor, newBytes.length(), addedLines);
                emit statusMessage(tr("👁️ Tail: +%1 lines streamed (%2 bytes)").arg(addedLines).arg(newBytes.length()));
            }
        }
    }
}
