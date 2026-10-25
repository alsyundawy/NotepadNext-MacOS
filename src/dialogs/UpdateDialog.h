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

#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QString>

class QLabel;
class QPushButton;
class QTextBrowser;
class QWidget;

struct UpdateManifest {
    bool updateAvailable{false};
    QString latestVersion;
    QString currentVersion;
    QString downloadUrl;
    QString changelogUrl;
    QString openUrl;
    QString expectedSha256;
};

class UpdateDialog : public QDialog {
    Q_OBJECT

public:
    explicit UpdateDialog(const UpdateManifest &manifest, QWidget *parent = nullptr);

    explicit UpdateDialog(bool updateAvailable,
                          const QString &latestVersion,
                          const QString &currentVersion,
                          const QString &downloadUrl,
                          const QString &changelogUrl,
                          const QString &openUrl,
                          QWidget *parent = nullptr);
    ~UpdateDialog() override = default;

    static bool verifyPackageHash(const QString &filePath, const QString &expectedSha256); // NOLINT(bugprone-easily-swappable-parameters)

protected:
    void changeEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupUi();
    void updateThemeColors();

    bool m_updateAvailable;
    bool m_isError;
    QString m_latestVersion;
    QString m_currentVersion;
    QString m_downloadUrl;
    QString m_changelogUrl;
    QString m_openUrl;
    QString m_expectedSha256;

    // UI elements
    QLabel *m_iconLabel{nullptr};
    QLabel *m_badgeLabel{nullptr};
    QLabel *m_headingLabel{nullptr};
    QLabel *m_subLabel{nullptr};
    QWidget *m_cardWidget{nullptr};
    QTextBrowser *m_changelogBrowser{nullptr};
    QPushButton *m_primaryButton{nullptr};
    QPushButton *m_viewReleaseButton{nullptr};
    QPushButton *m_closeButton{nullptr};
};

#endif // UPDATEDIALOG_H
