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

#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <memory>
#include <QDialog>
#include <QEvent>
#include <QStatusBar>
#include <QTabBar>

#include "Finder.h"
#include "ISearchResultsHandler.h"


class ScintillaNext;
class MainWindow;
class BookMarkDecorator;

namespace Ui {
class FindReplaceDialog;
}

class FindReplaceDialog : public QDialog // NOSONAR
{
    Q_OBJECT

public:
    enum class TabIndex : int {
        Find = 0,
        Replace = 1,
        Mark = 2
    };
    static constexpr int FIND_TAB = static_cast<int>(TabIndex::Find);
    static constexpr int REPLACE_TAB = static_cast<int>(TabIndex::Replace);
    static constexpr int MARK_TAB = static_cast<int>(TabIndex::Mark);

    enum class SearchDirection
    {
        Forwards,
        Backwards
    };

    explicit FindReplaceDialog(ISearchResultsHandler *searchResults, MainWindow *window = nullptr);
    ~FindReplaceDialog() override;

    void setFindString(const QString &string);
    void setTab(int tab);

    QString replaceString() const;

    void setSearchResultsHandler(ISearchResultsHandler *searchResultsHandler);

protected:
    bool event(QEvent *e) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void windowActivated();
    void windowDeactivated();

public slots:
    void performNextSearch();
    void performPrevSearch();

    void find();
    void findAllInCurrentDocument();
    void findAllInDocuments();
    void count();
    void replace();
    void replaceAll();
    void markAll();
    void clearAllMarks();
    void copyMarkedText();

private slots:
    void setEditor(ScintillaNext *targetEditor);
    void adjustOpacity(int value);
    void transparencyToggled(bool on);
    void adjustOpacityWhenLosingFocus(bool checked);
    void adjustOpacityAlways(bool on);

    void changeTab(int index);

private:
    QString findString() const;
    void performFind(SearchDirection direction);
    void prepareToPerformSearch(bool isReplace=false);
    void loadSettings();
    void saveSettings() const;

    void savePosition();
    void restorePosition();

    FindOptions findOptions() const;

    void showMessage(const QString &message, const QString &color);
    int ensureMarkIndicator();
    BookMarkDecorator *bookMarkDecorator() const;
    void clearAllBookmarks() const;

    void updateFindList(const QString &text);
    void updateReplaceList(const QString &text);

    void updateInSelectionCheckbox();

    bool isFirstTime = true;
    QPoint lastClosedPosition;
    std::unique_ptr<Ui::FindReplaceDialog> ui;

    ScintillaNext *editor = nullptr;
    QStatusBar *statusBar = nullptr;
    QTabBar *tabBar = nullptr;
    ISearchResultsHandler *searchResultsHandler = nullptr;
    std::unique_ptr<Finder> finder;

    Sci_CharacterRange capturedSelectionRange = {0, 0};
};

#endif // FINDREPLACEDIALOG_H
