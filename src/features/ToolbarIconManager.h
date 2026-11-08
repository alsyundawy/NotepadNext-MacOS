#pragma once
/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef TOOLBARICONMANAGER_H
#define TOOLBARICONMANAGER_H

#include <cmath>
#ifdef isinf
#undef isinf
#endif
#ifdef isnan
#undef isnan
#endif

#include <QHash>
#include <QIcon>
#include <QList>
#include <QObject>
#include <QSet>
#include <QSize>
#include <QString>
#include <QStringList>

class QToolBar;
class QWidget;
class QAction;
class QMenu;
class ApplicationSettings;

struct ToolbarIconPack {
    QString id;
    QString displayName;
    QString description;
    QString basePath;
    bool isBuiltIn{true};
    bool isPngBased{false};
};

class ToolbarIconManager : public QObject {
    Q_OBJECT

public:
    static ToolbarIconManager *instance();

    void initialize(ApplicationSettings *settings);

    [[nodiscard]] const QList<ToolbarIconPack> &availablePacks() const;
    [[nodiscard]] const ToolbarIconPack *findPackById(const QString &packId) const;
    [[nodiscard]] const QString &currentPackId() const { return m_currentPackId; }
    void setPack(const QString &packId, QToolBar *toolBar = nullptr);

    [[nodiscard]] QSize currentIconSize() const { return m_currentIconSize; }
    [[nodiscard]] QSize iconSize() const { return m_currentIconSize; }
    void setIconSize(const QSize &size, QToolBar *toolBar = nullptr);

    [[nodiscard]] QIcon getActionIcon(const QString &actionName, const QString &packId = QString()) const;
    [[nodiscard]] QIcon actionIcon(const QString &actionName, const QString &packId = QString()) const {
        return getActionIcon(actionName, packId);
    }
    [[nodiscard]] bool isDarkMode() const { return m_isDarkMode; }
    void setDarkMode(bool dark) {
        m_isDarkMode = dark;
    }
    void clearCache() const { m_iconCache.clear(); }

    void applyToToolbar(QToolBar *toolBar) const;
    void applyToWindow(const QWidget *window, QToolBar *toolBar = nullptr) const;
    void applyToMenu(const QMenu *menu) const;

    [[nodiscard]] static QString customPacksPath();
    void scanCustomPacks();

    static QString canonicalNameForAction(const QString &actionName);
    static QStringList aliasesForCanonical(const QString &canonical);

signals:
    void packChanged(const QString &packId);
    void iconSizeChanged(const QSize &size);

private:
    explicit ToolbarIconManager(QObject *parent = nullptr);
    ~ToolbarIconManager() override = default;

    void registerBuiltInPacks();
    [[nodiscard]] static QString resolveIconFilePath(const ToolbarIconPack &pack, const QString &canonical);

    ApplicationSettings *m_settings{nullptr};
    QString m_currentPackId{QStringLiteral("notepadNext")};
    QSize m_currentIconSize{24, 24};
    bool m_isDarkMode{false};
    QList<ToolbarIconPack> m_packs;
    mutable QHash<QString, QIcon> m_iconCache;
};

#endif  // TOOLBARICONMANAGER_H
