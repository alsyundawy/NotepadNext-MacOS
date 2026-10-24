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

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QList>
#include "ThemePaletteData.h"

class MainWindow;
class ScintillaNext;
class ApplicationSettings;

class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum class ThemeMode { Auto = 0, Light = 1, Dark = 2 };
    Q_ENUM(ThemeMode)

    // Helper to construct Scintilla BGR colour from RGB components
    static constexpr quint32 scColor(int red, int green, int blue) {
        return static_cast<quint32>((red & 0xFF) | ((green & 0xFF) << 8) | ((blue & 0xFF) << 16));
    }
    static inline quint32 scFromRgb(QRgb rgb) { return scColor(qRed(rgb), qGreen(rgb), qBlue(rgb)); }
    static inline quint32 scFromHex(const char *hex) {
        QColor c(hex);
        return scFromRgb(c.rgb());
    }

    static ThemeManager *instance();

    void initialize(ApplicationSettings *settings);

    ThemeMode themeMode() const { return m_themeMode; }
    void setThemeMode(ThemeMode mode, MainWindow *window = nullptr);
    void toggleTheme(MainWindow *window = nullptr);

    bool isDarkActive() const;

    const QString &currentPaletteId() const { return m_currentPaletteId; }
    const ThemePalette *currentPalette() const;
    void setPalette(const QString &paletteId, MainWindow *window = nullptr);

    void applyTheme(MainWindow *window);
    void applyThemeToEditor(ScintillaNext *editor) const;
    QString getThemeStyleSheet() const;

signals:
    void themeChanged(bool isDark);
    void paletteChanged(const QString &paletteId);

private slots:
    void onSystemColorSchemeChanged();

private:
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager() override;

    ThemeMode m_themeMode = ThemeMode::Light;
    QString m_currentPaletteId = QStringLiteral("notepadNext");
    ApplicationSettings *m_settings = nullptr;
    MainWindow *m_activeWindow = nullptr;
};

#endif  // THEMEMANAGER_H
