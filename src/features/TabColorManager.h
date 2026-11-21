#pragma once
/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <QColor>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>

struct TabColorPreset {
    QString name;
    QColor color;
};

class TabColorManager : public QObject {
    Q_OBJECT

public:
    explicit TabColorManager(QObject *parent = nullptr);
    ~TabColorManager() override = default;

    static TabColorManager *instance();

    [[nodiscard]] static QList<QColor> paletteColors();
    [[nodiscard]] static QList<TabColorPreset> presets();

    void setTabColor(const QString &filePath, const QColor &color);
    [[nodiscard]] QColor tabColor(const QString &filePath) const;
    void clearTabColor(const QString &filePath);
    [[nodiscard]] bool hasTabColor(const QString &filePath) const;

    [[nodiscard]] const QMap<QString, QColor> &allTabColors() const;

    void loadSettings();
    void saveSettings() const;

signals:
    void tabColorChanged(const QString &filePath, const QColor &color);

private:
    QMap<QString, QColor> m_tabColors;
};
