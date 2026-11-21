#pragma once
/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Implements Monitoring / Tail Mode (tail -f).
 * Adapted from Notepad++ and Notepad4 monitoring architectures.
 * Provides live disk monitoring, zero-modal log streaming, atomic chunk appending,
 * and automatic scrolling for log files.
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef TAILMONITORINGMANAGER_H
#define TAILMONITORINGMANAGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QMap>
#include <QPointer>
#include <QTimer>
#include <QFileSystemWatcher>

class ScintillaNext;

class TailMonitoringManager : public QObject {
    Q_OBJECT

public:
    struct MonitoredDocState {
        QString filePath;
        qint64 lastSize{0};
        QDateTime lastModified;
        bool wasReadOnly{false};
        bool autoScrollToEnd{true};
    };

    static TailMonitoringManager *instance();

    bool isMonitoring(const ScintillaNext *editor) const;
    bool toggleMonitoring(ScintillaNext *editor);
    bool startMonitoring(ScintillaNext *editor);
    void stopMonitoring(ScintillaNext *editor);
    void stopAll();

    bool autoScrollToEnd(const ScintillaNext *editor) const;
    void setAutoScrollToEnd(ScintillaNext *editor, bool enable);

    int pollIntervalMs() const { return m_pollIntervalMs; }
    void setPollIntervalMs(int ms);

    bool readOnlyLock() const { return m_readOnlyLock; }
    void setReadOnlyLock(bool lock) { m_readOnlyLock = lock; }

    qint64 maxChunkBytes() const { return m_maxChunkBytes; }
    void setMaxChunkBytes(qint64 bytes) { m_maxChunkBytes = bytes; }

    void forceCheck(ScintillaNext *editor);

signals:
    void monitoringStateChanged(const ScintillaNext *editor, bool isMonitoring);
    void fileTailed(ScintillaNext *editor, qint64 bytesAdded, int newLines);
    void fileRotatedOrTruncated(ScintillaNext *editor);
    void statusMessage(const QString &message);

private slots:
    void onHeartbeatTimer();
    void onFileSystemChanged(const QString &path);
    void onEditorDestroyed(const QObject *obj);

private:
    explicit TailMonitoringManager(QObject *parent = nullptr);
    void checkEditorFile(ScintillaNext *editor);

    static TailMonitoringManager *s_instance;

    QMap<ScintillaNext *, MonitoredDocState> m_monitored;
    QFileSystemWatcher *m_fileWatcher;
    QTimer *m_timer;

    int m_pollIntervalMs{750};
    bool m_readOnlyLock{true};
    qint64 m_maxChunkBytes{10LL * 1024 * 1024};
};

#endif  // TAILMONITORINGMANAGER_H
