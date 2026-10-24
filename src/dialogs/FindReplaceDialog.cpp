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

#include "FindReplaceDialog.h"
#include "ApplicationSettings.h"
#include "features/ThemeManager.h"
#include "ui_FindReplaceDialog.h"

#include <QStatusBar>
#include <QLineEdit>
#include <QShortcut>
#include <QClipboard>
#include <QGuiApplication>
#include <QScreen>

#include "ScintillaNext.h"
#include "MainWindow.h"
#include "BookMarkDecorator.h"


static void convertToExtended(QString &str)
{
    str.replace("\\r", "\r");
    str.replace("\\n", "\n");
    str.replace("\\t", "\t");
    str.replace("\\0", "\0");
    str.replace(R"(\\)", "\\");
    // Extended escape sequence processing complete
}

FindReplaceDialog::FindReplaceDialog(ISearchResultsHandler *searchResults, MainWindow *window) :
    QDialog(window, Qt::Dialog),
    ui(std::make_unique<Ui::FindReplaceDialog>()),
    searchResultsHandler(searchResults),
    finder(std::make_unique<Finder>(window->currentEditor()))
{
    qInfo(Q_FUNC_INFO);

    // Turn off the help button on the dialog
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    ui->setupUi(this);

    // Get the current editor, and keep up the reference
    setEditor(window->currentEditor());
    connect(window, &MainWindow::editorActivated, this, &FindReplaceDialog::setEditor);

    setStyleSheet(ThemeManager::instance()->getThemeStyleSheet());
    connect(ThemeManager::instance(), &ThemeManager::themeChanged, this,
            [this]() { setStyleSheet(ThemeManager::instance()->getThemeStyleSheet()); });
    connect(ThemeManager::instance(), &ThemeManager::paletteChanged, this,
            [this]() { setStyleSheet(ThemeManager::instance()->getThemeStyleSheet()); });

    tabBar = new QTabBar(this);
    tabBar->setDrawBase(false);
    tabBar->addTab(tr("Find"));
    tabBar->addTab(tr("Replace"));
    tabBar->addTab(tr("Mark"));
    tabBar->setExpanding(false);
    qobject_cast<QVBoxLayout *>(layout())->insertWidget(0, tabBar, 0, Qt::AlignLeft);
    connect(tabBar, &QTabBar::currentChanged, this, &FindReplaceDialog::changeTab);

    statusBar = new QStatusBar(this);
    statusBar->setSizeGripEnabled(false); // the dialog has one already
    qobject_cast<QVBoxLayout *>(layout())->insertWidget(-1, statusBar);

    // Disable auto completion
    ui->comboFind->setCompleter(nullptr);
    ui->comboReplace->setCompleter(nullptr);

    // If the selection changes highlight the text
    connect(ui->comboFind, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), ui->comboFind->lineEdit(), &QLineEdit::selectAll);
    connect(ui->comboReplace, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), ui->comboReplace->lineEdit(), &QLineEdit::selectAll);

    // Support multi-line clipboard paste with auto-escaping
    if (ui->comboFind->lineEdit()) {
        ui->comboFind->lineEdit()->installEventFilter(this);
    }
    if (ui->comboReplace->lineEdit()) {
        ui->comboReplace->lineEdit()->installEventFilter(this);
    }

    // Force focus on the find text box
    connect(this, &FindReplaceDialog::windowActivated, [this]() {
        ui->comboFind->setFocus();
        ui->comboFind->lineEdit()->selectAll();
    });

    connect(this, &QDialog::rejected, [this]() {
        statusBar->clearMessage();
        savePosition();
    });

    connect(ui->radioRegexSearch, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            ui->checkBoxBackwardsDirection->setChecked(false);
            ui->checkBoxMatchWholeWord->setChecked(false);
        }

        ui->checkBoxBackwardsDirection->setDisabled(checked);
        ui->checkBoxMatchWholeWord->setDisabled(checked);
        ui->checkBoxRegexMatchesNewline->setEnabled(checked);
    });

    connect(ui->checkBoxInSelection, &QCheckBox::toggled, this, [this](bool checked) {
        if (checked && editor) {
            capturedSelectionRange = {static_cast<Sci_PositionCR>(editor->selectionStart()),
                                      static_cast<Sci_PositionCR>(editor->selectionEnd())};
        }
    });

    connect(ui->radioOnLosingFocus, &QRadioButton::toggled, this, &FindReplaceDialog::adjustOpacityWhenLosingFocus);
    connect(ui->radioAlways, &QRadioButton::toggled, this, &FindReplaceDialog::adjustOpacityAlways);
    connect(ui->transparency, &QGroupBox::toggled, this, &FindReplaceDialog::transparencyToggled);

    connect(ui->buttonFind, &QPushButton::clicked, this, &FindReplaceDialog::find);
    connect(ui->buttonCount, &QPushButton::clicked, this, &FindReplaceDialog::count);
    connect(ui->buttonFindAllInCurrent, &QPushButton::clicked, this, [this]() {
        prepareToPerformSearch();

        searchResultsHandler->newSearch(findString());

        findAllInCurrentDocument();

        searchResultsHandler->completeSearch();

        close();
    });
    connect(ui->buttonFindAllInDocuments, &QPushButton::clicked, this, [this]() {
        prepareToPerformSearch();

        searchResultsHandler->newSearch(findString());

        findAllInDocuments();

        searchResultsHandler->completeSearch();

        close();
    });
    connect(ui->buttonReplace, &QPushButton::clicked, this, &FindReplaceDialog::replace);
    connect(ui->buttonReplaceAll, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);
    connect(ui->buttonReplaceAllInDocuments, &QPushButton::clicked, this, [this]() {
        prepareToPerformSearch(true);

        QString replaceText = replaceString();

        if (ui->radioExtendedSearch->isChecked()) {
            convertToExtended(replaceText);
        }

        int totalReplaced = 0;
        ScintillaNext *current_editor = editor;

        if (const auto *mainWin = qobject_cast<MainWindow *>(parent()); mainWin != nullptr) {
            for (ScintillaNext *targetEd : mainWin->editors()) {
                setEditor(targetEd);
                totalReplaced += finder->replaceAll(replaceText);
            }
        }

        setEditor(current_editor);

        showMessage(tr("Replaced %Ln matches", "", totalReplaced), "green");
    });
    connect(ui->buttonClose, &QPushButton::clicked, this, &FindReplaceDialog::close);
    connect(ui->buttonMarkAll, &QPushButton::clicked, this, &FindReplaceDialog::markAll);
    connect(ui->buttonClearAllMarks, &QPushButton::clicked, this, &FindReplaceDialog::clearAllMarks);
    connect(ui->buttonCopyMarkedText, &QPushButton::clicked, this, &FindReplaceDialog::copyMarkedText);

    const auto findPrevious = [this]() {
        const int curTab = tabBar->currentIndex();
        if (curTab == FIND_TAB || curTab == REPLACE_TAB) {
            // Always default to forward if regex search is checked
            const bool regex = ui->radioRegexSearch->isChecked();
            performFind(regex ? SearchDirection::Forwards : SearchDirection::Backwards);
        }
    };
    new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Return), this, this, findPrevious, Qt::WidgetWithChildrenShortcut);
    new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Enter), this, this, findPrevious, Qt::WidgetWithChildrenShortcut);

    loadSettings();

    changeTab(tabBar->currentIndex());
}

FindReplaceDialog::~FindReplaceDialog() = default;

void FindReplaceDialog::setFindString(const QString &string)
{
    ui->comboFind->setCurrentText(string);
    ui->comboFind->lineEdit()->selectAll();
}

void FindReplaceDialog::setTab(int tab)
{
    tabBar->setCurrentIndex(tab);
}

bool FindReplaceDialog::event(QEvent *e)
{
    if (e->type() == QEvent::WindowActivate) {
        emit windowActivated();
    }
    else if (e->type() == QEvent::WindowDeactivate) {
        emit windowDeactivated();
    }

    return QDialog::event(e);
}

void FindReplaceDialog::showEvent(QShowEvent *event)
{
    qInfo(Q_FUNC_INFO);

    if (!isFirstTime)
        restorePosition();

    isFirstTime = false;

    updateInSelectionCheckbox();

    QDialog::showEvent(event);
}

void FindReplaceDialog::closeEvent(QCloseEvent *event)
{
    saveSettings();

    QDialog::closeEvent(event);
}

static void updateComboList(QComboBox *comboBox, const QString &text)
{
    // Block the signals while it is manipulated
    const QSignalBlocker blocker(comboBox);

    // Remove it if it is in the list, add it to beginning, and select it
    comboBox->removeItem(comboBox->findText(text));
    comboBox->insertItem(0, text);
    comboBox->setCurrentIndex(0);
}

void FindReplaceDialog::updateFindList(const QString &text)
{
    if (!text.isEmpty())
        updateComboList(ui->comboFind, text);
}

void FindReplaceDialog::updateReplaceList(const QString &text)
{
    updateComboList(ui->comboReplace, text);
}

void FindReplaceDialog::find()
{
    performFind(ui->checkBoxBackwardsDirection->isChecked() ? SearchDirection::Backwards : SearchDirection::Forwards);
}

void FindReplaceDialog::performFind(SearchDirection direction)
{
    qInfo(Q_FUNC_INFO);

    prepareToPerformSearch();

    FindResult result = direction == SearchDirection::Forwards ? finder->findNext() : finder->findPrev();

    if (result) {
        if (result.wrapped) {
            showMessage(tr("The end of the document has been reached. Found 1st occurrence from the top."), "green");
        }

        // Note: Zero length matches are tracked and logged
        if (result.range.cpMin == result.range.cpMax) {
            qWarning() << "0 length match at" << result.range.cpMin;
        }

        editor->goToRange(result.range);
    }
    else {
        showMessage(tr("No matches found."), "red");
    }
}

void FindReplaceDialog::findAllInCurrentDocument()
{
    qInfo(Q_FUNC_INFO);

    bool firstMatch = true;

    QString text = findString();

    finder->options().text = text;
    auto matchHandler = [&firstMatch, this](int start, int end) {
        // Only add the file entry if there was a valid search result
        if (firstMatch) {
            searchResultsHandler->newFileEntry(editor);
            firstMatch = false;
        }

        const auto line = static_cast<int>(editor->lineFromPosition(start));
        const auto lineStartPosition = static_cast<int>(editor->positionFromLine(line));
        const auto lineEndPosition = static_cast<int>(editor->lineEndPosition(line));
        const int startPositionFromBeginning = start - lineStartPosition;
        const int endPositionFromBeginning = end - lineStartPosition;
        QString lineText = editor->get_text_range(lineStartPosition, lineEndPosition);

        searchResultsHandler->newResultsEntry(lineText, line, startPositionFromBeginning, endPositionFromBeginning);

        return end;
    };

    if (finder->hasSelectionRange()) {
        finder->forEachMatchInRange(matchHandler, capturedSelectionRange);
    } else {
        finder->forEachMatch(matchHandler);
    }
}

void FindReplaceDialog::findAllInDocuments()
{
    qInfo(Q_FUNC_INFO);

    ScintillaNext *current_editor = editor;
    const MainWindow *window = qobject_cast<MainWindow *>(parent());

    for (ScintillaNext *targetEd : window->editors()) {
        setEditor(targetEd);
        findAllInCurrentDocument();
    }

    setEditor(current_editor);
}

void FindReplaceDialog::replace()
{
    qInfo(Q_FUNC_INFO);

    prepareToPerformSearch();

    QString replaceText = replaceString();

    if (ui->radioExtendedSearch->isChecked()) {
        convertToExtended(replaceText);
    }

    const bool inSelection = ui->checkBoxInSelection->isChecked() && ui->checkBoxInSelection->isEnabled();
    const auto originalDocLength = inSelection ? static_cast<int>(editor->length()) : 0;

    if (finder->replaceSelectionIfMatch(replaceText)) {
        if (inSelection) {
            capturedSelectionRange.cpMax += static_cast<int>(editor->length()) - originalDocLength;
        }

        showMessage(tr("1 occurrence was replaced"), "blue");
    }

    FindResult result = finder->findNext();

    if (result) {
        editor->goToRange(result.range);
    }
    else {
        showMessage(tr("No more occurrences were found"), "red");
        ui->comboFind->setFocus();
        ui->comboFind->lineEdit()->selectAll();
    }
}

void FindReplaceDialog::replaceAll()
{
    qInfo(Q_FUNC_INFO);

    prepareToPerformSearch(true);

    QString replaceText = replaceString();

    if (ui->radioExtendedSearch->isChecked()) {
        convertToExtended(replaceText);
    }

    const bool inSelection = ui->checkBoxInSelection->isChecked() && ui->checkBoxInSelection->isEnabled();
    const auto originalDocLength = inSelection ? static_cast<int>(editor->length()) : 0;

    int totalReplaced = finder->replaceAll(replaceText);
    showMessage(tr("Replaced %Ln matches", "", totalReplaced), "green");

    if (inSelection) {
        const auto lengthDelta = static_cast<int>(editor->length()) - originalDocLength;
        const int newEnd = static_cast<int>(capturedSelectionRange.cpMax) + lengthDelta;
        editor->setSelection(newEnd, static_cast<int>(capturedSelectionRange.cpMin));
        capturedSelectionRange.cpMax = static_cast<Sci_PositionCR>(newEnd);
        if (newEnd == capturedSelectionRange.cpMin) {
            ui->checkBoxInSelection->setChecked(false);
            updateInSelectionCheckbox();
        }
    }
}

void FindReplaceDialog::count()
{
    qInfo(Q_FUNC_INFO);

    prepareToPerformSearch();

    int total = finder->count();

    showMessage(tr("Found %Ln matches", "", total), "green");
}

void FindReplaceDialog::setEditor(ScintillaNext *targetEditor)
{
    this->editor = targetEditor;

    finder->setEditor(targetEditor);

    capturedSelectionRange = {0, 0};
    ui->checkBoxInSelection->setChecked(false);
    ui->checkBoxInSelection->setEnabled(false);
    finder->clearSelectionRange();
}

void FindReplaceDialog::performNextSearch()
{
    FindResult result = finder->findNext();

    if (result)
        editor->goToRange(result.range);
}

void FindReplaceDialog::performPrevSearch()
{
    FindResult result = finder->findPrev();

    if (result)
        editor->goToRange(result.range);
}

void FindReplaceDialog::adjustOpacity(int value)
{
    qInfo(Q_FUNC_INFO);

    setWindowOpacity(value * .01);
}

void FindReplaceDialog::transparencyToggled(bool on)
{
    qInfo(Q_FUNC_INFO);

    if (on) {
        if (ui->radioOnLosingFocus->isChecked()) {
            adjustOpacityWhenLosingFocus(true);
            adjustOpacityAlways(false);
        }
        else {
            adjustOpacityWhenLosingFocus(false);
            adjustOpacityAlways(true);
        }
    }
    else {
        adjustOpacityWhenLosingFocus(false);
        adjustOpacityAlways(false);
        adjustOpacity(100);
    }
}

void FindReplaceDialog::adjustOpacityWhenLosingFocus(bool checked)
{
    qInfo(Q_FUNC_INFO);

    if (checked) {
        connect(this, &FindReplaceDialog::windowActivated, [this]() {
            this->adjustOpacity(100);
        });
        connect(this, &FindReplaceDialog::windowDeactivated, [this]() {
            this->adjustOpacity(ui->horizontalSlider->value());
        });
        adjustOpacity(100);
    }
    else {
        disconnect(this, &FindReplaceDialog::windowActivated, nullptr, nullptr);
        disconnect(this, &FindReplaceDialog::windowDeactivated, nullptr, nullptr);
    }
}

void FindReplaceDialog::adjustOpacityAlways(bool checked)
{
    qInfo(Q_FUNC_INFO);

    if (checked) {
        connect(ui->horizontalSlider, &QSlider::valueChanged, this, &FindReplaceDialog::adjustOpacity);
        adjustOpacity(ui->horizontalSlider->value());
    }
    else {
        disconnect(ui->horizontalSlider, &QSlider::valueChanged, this, &FindReplaceDialog::adjustOpacity);
    }
}

void FindReplaceDialog::changeTab(int index)
{
    if (index == FIND_TAB) {
        ui->labelReplaceWith->setMaximumHeight(0);
        ui->comboReplace->setMaximumHeight(0);
        // The combo box isn't actually "hidden", so adjust the focus policy so it does not get tabbed to
        ui->comboReplace->setFocusPolicy(Qt::NoFocus);

        ui->buttonFind->show();

        ui->buttonReplace->hide();
        ui->buttonReplaceAll->hide();
        ui->buttonReplaceAllInDocuments->hide();
        ui->buttonMarkAll->hide();
        ui->buttonClearAllMarks->hide();
        ui->buttonCopyMarkedText->hide();

        ui->buttonCount->show();
        ui->buttonFindAllInCurrent->show();
        ui->buttonFindAllInDocuments->show();

        ui->checkBoxBookmarkLine->hide();
        ui->checkBoxPurgeForEachSearch->hide();

        ui->checkBoxBackwardsDirection->setEnabled(!ui->radioRegexSearch->isChecked());
        ui->checkBoxWrapAround->setEnabled(true);
    }
    else if (index == REPLACE_TAB) {
        ui->labelReplaceWith->setMaximumHeight(QWIDGETSIZE_MAX);
        ui->comboReplace->setMaximumHeight(QWIDGETSIZE_MAX);
        ui->comboReplace->setFocusPolicy(Qt::StrongFocus); // Reset its focus policy

        ui->buttonFind->show();

        ui->buttonReplace->show();
        ui->buttonReplaceAll->show();
        ui->buttonReplaceAllInDocuments->show();
        ui->buttonMarkAll->hide();
        ui->buttonClearAllMarks->hide();
        ui->buttonCopyMarkedText->hide();

        ui->buttonCount->hide();
        ui->buttonFindAllInCurrent->hide();
        ui->buttonFindAllInDocuments->hide();

        ui->checkBoxBookmarkLine->hide();
        ui->checkBoxPurgeForEachSearch->hide();

        ui->checkBoxBackwardsDirection->setEnabled(!ui->radioRegexSearch->isChecked());
        ui->checkBoxWrapAround->setEnabled(true);
    }
    else if (index == MARK_TAB) {
        ui->labelReplaceWith->setMaximumHeight(0);
        ui->comboReplace->setMaximumHeight(0);
        ui->comboReplace->setFocusPolicy(Qt::NoFocus);

        ui->buttonFind->hide();

        ui->buttonReplace->hide();
        ui->buttonReplaceAll->hide();
        ui->buttonReplaceAllInDocuments->hide();
        ui->buttonCount->hide();
        ui->buttonFindAllInCurrent->hide();
        ui->buttonFindAllInDocuments->hide();

        ui->buttonMarkAll->show();
        ui->buttonClearAllMarks->show();
        ui->buttonCopyMarkedText->show();

        ui->checkBoxBookmarkLine->show();
        ui->checkBoxPurgeForEachSearch->show();

        ui->checkBoxBackwardsDirection->setEnabled(!ui->radioRegexSearch->isChecked());
        ui->checkBoxWrapAround->setEnabled(true);
    }

    ui->comboFind->setFocus();
    ui->comboFind->lineEdit()->selectAll();
}

QString FindReplaceDialog::findString() const
{
    return ui->comboFind->currentText();
}

QString FindReplaceDialog::replaceString() const
{
    return ui->comboReplace->currentText();
}

void FindReplaceDialog::setSearchResultsHandler(ISearchResultsHandler *searchResults)
{
    this->searchResultsHandler = searchResults;
}

void FindReplaceDialog::prepareToPerformSearch(bool isReplace)
{
    qInfo(Q_FUNC_INFO);

    QString findText = findString();

    updateFindList(findText);
    if (isReplace) {
        QString replaceText = replaceString();
        updateReplaceList(replaceText);
    }

    statusBar->clearMessage();

    if (ui->radioExtendedSearch->isChecked()) {
        convertToExtended(findText);
    }

    finder->options() = findOptions();

    if (ui->checkBoxInSelection->isChecked() && ui->checkBoxInSelection->isEnabled()) {
        finder->setSelectionRange(capturedSelectionRange);
    } else {
        finder->clearSelectionRange();
    }
}

void FindReplaceDialog::loadSettings()
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;

    settings.beginGroup("FindReplaceDialog");

    restoreGeometry(settings.value("geometry").toByteArray());

    // Defensively clamp the restored size in case a previously persisted geometry
    // is larger than the available screen (e.g. from an older version that let the
    // dialog grow to fit a very long recent search string). This lets users recover
    // automatically without having to edit or delete notepadnext.ini.
    if (const QScreen *screen = QGuiApplication::primaryScreen()) {
        const QSize available = screen->availableGeometry().size();
        const QSize current = size();
        const QSize clamped = current.boundedTo(available);
        if (clamped != current)
            resize(clamped);
    }

    ui->comboFind->addItems(settings.value("RecentSearchList").toStringList());
    ui->comboReplace->addItems(settings.value("RecentReplaceList").toStringList());

    ui->checkBoxBackwardsDirection->setChecked(settings.value("Backwards").toBool());
    ui->checkBoxMatchWholeWord->setChecked(settings.value("WholeWord").toBool());
    ui->checkBoxMatchCase->setChecked(settings.value("MatchCase").toBool());
    ui->checkBoxWrapAround->setChecked(settings.value("WrapAround", true).toBool());

    if (settings.contains("SearchMode")) {
        const QString searchMode = settings.value("SearchMode").toString();
        if (searchMode == "normal")
            ui->radioNormalSearch->setChecked(true);
        else if (searchMode == "extended")
            ui->radioExtendedSearch->setChecked(true);
        else
            ui->radioRegexSearch->setChecked(true);
    }
    ui->checkBoxRegexMatchesNewline->setChecked(settings.value("DotMatchesNewline").toBool());

    ui->transparency->setChecked(settings.value("TransparencyUsed").toBool());
    if (ui->transparency->isChecked()) {
        ui->horizontalSlider->setValue(settings.value("Transparency", 70).toInt());

        if (settings.value("TransparencyMode").toString() == "focus") {
            ui->radioOnLosingFocus->setChecked(true);
        }
        else {
            ui->radioAlways->setChecked(true);
        }
    }

    settings.endGroup();
}

void FindReplaceDialog::saveSettings() const
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;

    settings.beginGroup("FindReplaceDialog");
    settings.remove(""); // clear out any previous keys

    settings.setValue("geometry", saveGeometry());

    QStringList recentSearches;
    for (int i = 0; i < ui->comboFind->count(); ++i) {
        recentSearches << ui->comboFind->itemText(i);
    }
    settings.setValue("RecentSearchList", recentSearches);

    recentSearches.clear();
    for (int i = 0; i < ui->comboReplace->count(); ++i) {
        recentSearches << ui->comboReplace->itemText(i);
    }
    settings.setValue("RecentReplaceList", recentSearches);

    settings.setValue("Backwards", ui->checkBoxBackwardsDirection->isChecked());
    settings.setValue("WholeWord", ui->checkBoxMatchWholeWord->isChecked());
    settings.setValue("MatchCase", ui->checkBoxMatchCase->isChecked());
    settings.setValue("WrapAround", ui->checkBoxWrapAround->isChecked());

    if (ui->radioNormalSearch->isChecked())
        settings.setValue("SearchMode", "normal");
    else if (ui->radioExtendedSearch->isChecked())
        settings.setValue("SearchMode", "extended");
    else if (ui->radioRegexSearch->isChecked())
        settings.setValue("SearchMode", "regex");
    settings.setValue("DotMatchesNewline", ui->checkBoxRegexMatchesNewline->isChecked());

    settings.setValue("TransparencyUsed", ui->transparency->isChecked());
    if (ui->transparency->isChecked()) {
        settings.setValue("Transparency", ui->horizontalSlider->value());
        settings.setValue("TransparencyMode", ui->radioOnLosingFocus->isChecked() ? "focus" : "always");
    }

    settings.endGroup();
}

void FindReplaceDialog::savePosition()
{
    qInfo(Q_FUNC_INFO);

    lastClosedPosition = pos();
}

void FindReplaceDialog::restorePosition()
{
    qInfo(Q_FUNC_INFO);

    ApplicationSettings settings;

    if (settings.centerSearchDialog()) {
        const QPoint centerPoint = parentWidget()->geometry().center();
        move(centerPoint - rect().center());
    }
    else {
        move(lastClosedPosition);
    }
}

FindOptions FindReplaceDialog::findOptions() const
{
    using enum Scintilla::FindOption;
    FindOptions options;

    options.text = findString();

    if (ui->checkBoxMatchWholeWord->isChecked())
        options.flags |= WholeWord;

    if (ui->checkBoxMatchCase->isChecked())
        options.flags |= MatchCase;

    if (ui->radioRegexSearch->isChecked())
        options.flags |= RegExp;

    options.wrapAround = ui->checkBoxWrapAround->isChecked();

    return options;
}

int FindReplaceDialog::ensureMarkIndicator()
{
    int markIndicator = editor->allocateIndicator(QStringLiteral("find_mark_highlight"));
    editor->indicSetFore(markIndicator, 0xFFCC00);
    editor->indicSetStyle(markIndicator, INDIC_FULLBOX);
    editor->indicSetOutlineAlpha(markIndicator, 200);
    editor->indicSetAlpha(markIndicator, 100);
    editor->indicSetUnder(markIndicator, true);

    return markIndicator;
}

BookMarkDecorator *FindReplaceDialog::bookMarkDecorator() const
{
    if (auto *decorator = editor->findChild<BookMarkDecorator *>(QString(), Qt::FindDirectChildrenOnly);
        decorator && decorator->isEnabled()) {
        return decorator;
    }

    return nullptr;
}

void FindReplaceDialog::clearAllBookmarks() const
{
    BookMarkDecorator *decorator = bookMarkDecorator();
    if (decorator) {
        decorator->clearAllBookmarks();
    }
}

void FindReplaceDialog::markAll()
{
    qInfo(Q_FUNC_INFO);

    prepareToPerformSearch();
    int markIndicator = ensureMarkIndicator();

    editor->setIndicatorCurrent(markIndicator);

    const bool inSelection = finder->hasSelectionRange();

    if (ui->checkBoxPurgeForEachSearch->isChecked()) {
        if (inSelection) {
            editor->indicatorClearRange(capturedSelectionRange.cpMin,
                                        capturedSelectionRange.cpMax - capturedSelectionRange.cpMin);
        } else {
            editor->indicatorClearRange(0, editor->length());
        }
        clearAllBookmarks();
    }

    BookMarkDecorator *bmDecorator = nullptr;
    if (ui->checkBoxBookmarkLine->isChecked()) {
        bmDecorator = this->bookMarkDecorator();
    }

    int matchCount = 0;
    auto callback = [&matchCount, bmDecorator, this](int start, int end) {
        editor->indicatorFillRange(start, end - start);
        matchCount++;

        if (bmDecorator) {
            const auto line = static_cast<int>(editor->lineFromPosition(start));
            if (!bmDecorator->isBookmarkSet(line)) {
                bmDecorator->addBookmark(line);
            }
        }

        return end;
    };

    if (inSelection) {
        finder->forEachMatchInRange(callback, capturedSelectionRange);
        showMessage(tr("Mark: %Ln match in selection", "", matchCount), "green");
    } else {
        finder->forEachMatch(callback);
        showMessage(tr("Mark: %Ln match in entire file", "", matchCount), "green");
    }
}

void FindReplaceDialog::updateInSelectionCheckbox()
{
    if (!editor) {
        ui->checkBoxInSelection->setEnabled(false);
        return;
    }

    const auto selStart = static_cast<int>(editor->selectionStart());
    const auto selEnd   = static_cast<int>(editor->selectionEnd());
    const bool hasSelection    = (selStart != selEnd);
    const bool isRectangular   = editor->selectionIsRectangle();
    const bool isMultiple      = (editor->selections() > 1);

    if (hasSelection && !isRectangular && !isMultiple) {
        ui->checkBoxInSelection->setEnabled(true);
        if (!ui->checkBoxInSelection->isChecked()) {
            capturedSelectionRange = {static_cast<Sci_PositionCR>(selStart),
                                      static_cast<Sci_PositionCR>(selEnd)};
        }
    } else {
        ui->checkBoxInSelection->setChecked(false);
        ui->checkBoxInSelection->setEnabled(false);
        finder->clearSelectionRange();
    }
}

void FindReplaceDialog::clearAllMarks()
{
    qInfo(Q_FUNC_INFO);

    int markIndicator = ensureMarkIndicator();
    editor->setIndicatorCurrent(markIndicator);
    editor->indicatorClearRange(0, static_cast<int>(editor->length()));
    clearAllBookmarks();
    showMessage(tr("All marks cleared"), "green");
}

void FindReplaceDialog::copyMarkedText()
{
    qInfo(Q_FUNC_INFO);

    int markIndicator = ensureMarkIndicator();

    QStringList markedTexts;
    int pos = 0;
    const auto len = static_cast<int>(editor->length());

    while (pos < len) {
        if (editor->indicatorValueAt(markIndicator, pos)) {
            const auto end = static_cast<int>(editor->indicatorEnd(markIndicator, pos));
            markedTexts << QString::fromUtf8(editor->get_text_range(pos, end));
            pos = end;
        } else {
            pos++;
        }
    }

    if (markedTexts.isEmpty()) {
        showMessage(tr("No marks to copy"), "red");
        return;
    }

    QGuiApplication::clipboard()->setText(markedTexts.join("\n"));
    showMessage(tr("Copied %Ln marked text(s)", "", static_cast<int>(markedTexts.size())), "green");
}

void FindReplaceDialog::showMessage(const QString &message, const QString &color)
{
    statusBar->setStyleSheet(QStringLiteral("color: %1").arg(color));
    statusBar->showMessage(message);
}

bool FindReplaceDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() != QEvent::KeyPress) {
        return QDialog::eventFilter(obj, event);
    }

    if (const auto *keyEvent = static_cast<const QKeyEvent *>(event); !keyEvent->matches(QKeySequence::Paste)) {
        return QDialog::eventFilter(obj, event);
    }

    const QClipboard *clipboard = QGuiApplication::clipboard();
    if (!clipboard) {
        return QDialog::eventFilter(obj, event);
    }

    QString text = clipboard->text();
    if (!text.contains(QLatin1Char('\n')) && !text.contains(QLatin1Char('\r'))) {
        return QDialog::eventFilter(obj, event);
    }

    auto *le = qobject_cast<QLineEdit *>(obj);
    if (!le) {
        return QDialog::eventFilter(obj, event);
    }

    if (ui->radioExtendedSearch->isChecked() || ui->radioRegexSearch->isChecked()) {
        text.replace(QStringLiteral("\r\n"), QStringLiteral("\\n"));
        text.replace(QStringLiteral("\r"), QStringLiteral("\\n"));
        text.replace(QStringLiteral("\n"), QStringLiteral("\\n"));
    } else {
        text.replace(QStringLiteral("\r\n"), QStringLiteral(" "));
        text.replace(QStringLiteral("\r"), QStringLiteral(" "));
        text.replace(QStringLiteral("\n"), QStringLiteral(" "));
    }
    le->insert(text);
    return true;
}
