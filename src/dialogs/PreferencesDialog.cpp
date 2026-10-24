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

#include "PreferencesDialog.h"
#include "NotepadNextApplication.h"
#include "TranslationManager.h"
#include "ThemeManager.h"
#include "ThemePaletteData.h"
#include "MainWindow.h"
#include "ui_PreferencesDialog.h"
#include "ScintillaNext.h"

#include <QButtonGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QAbstractItemView>


PreferencesDialog::PreferencesDialog(ApplicationSettings *settings, QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(std::make_unique<Ui::PreferencesDialog>()),
    settings(settings)
{
    ui->setupUi(this);

    setupThemeSettings();
    connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &PreferencesDialog::updateThemeColors);
    connect(ThemeManager::instance(), &ThemeManager::paletteChanged, this, &PreferencesDialog::updateThemeColors);

    QIcon icon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
    QPixmap pixmap = icon.pixmap(QSize(16, 16));
    ui->labelAppRestartIcon->setPixmap(pixmap);
    ui->labelAppRestartIcon->hide();
    ui->labelAppRestart->hide();

    MapSettingToCheckBox(ui->checkBoxMenuBar, &ApplicationSettings::showMenuBar, &ApplicationSettings::setShowMenuBar, &ApplicationSettings::showMenuBarChanged);
    MapSettingToCheckBox(ui->checkBoxToolBar, &ApplicationSettings::showToolBar, &ApplicationSettings::setShowToolBar, &ApplicationSettings::showToolBarChanged);
    MapSettingToCheckBox(ui->checkBoxStatusBar, &ApplicationSettings::showStatusBar, &ApplicationSettings::setShowStatusBar, &ApplicationSettings::showStatusBarChanged);
    MapSettingToCheckBox(ui->checkBoxRecenterSearchDialog, &ApplicationSettings::centerSearchDialog, &ApplicationSettings::setCenterSearchDialog, &ApplicationSettings::centerSearchDialogChanged);

    MapSettingToGroupBox(ui->gbxRestorePreviousSession, &ApplicationSettings::restorePreviousSession, &ApplicationSettings::setRestorePreviousSession, &ApplicationSettings::restorePreviousSessionChanged);
    connect(ui->gbxRestorePreviousSession, &QGroupBox::toggled, this, [this](bool checked) {
        if (!checked) {
            ui->checkBoxUnsavedFiles->setChecked(false);
            ui->checkBoxRestoreTempFiles->setChecked(false);
        }
        else {
            QMessageBox::warning(this, tr("Warning"), tr("This feature is experimental and it should not be considered safe for critically important work. It may lead to possible data loss. Use at your own risk."));
        }
    });

    MapSettingToCheckBox(ui->checkBoxUnsavedFiles, &ApplicationSettings::restoreUnsavedFiles, &ApplicationSettings::setRestoreUnsavedFiles, &ApplicationSettings::restoreUnsavedFilesChanged);
    MapSettingToCheckBox(ui->checkBoxRestoreTempFiles, &ApplicationSettings::restoreTempFiles, &ApplicationSettings::setRestoreTempFiles, &ApplicationSettings::restoreTempFilesChanged);

    MapSettingToCheckBox(ui->checkBoxCombineSearchResults, &ApplicationSettings::combineSearchResults, &ApplicationSettings::setCombineSearchResults, &ApplicationSettings::combineSearchResultsChanged);

    populateTranslationComboBox();
    connect(ui->comboBoxTranslation, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, settings](int idx) {
        settings->setTranslation(ui->comboBoxTranslation->itemData(idx).toString());
        showApplicationRestartRequired();
    });

    MapSettingToCheckBox(ui->checkBoxExitOnLastTabClosed, &ApplicationSettings::exitOnLastTabClosed, &ApplicationSettings::setExitOnLastTabClosed, &ApplicationSettings::exitOnLastTabClosedChanged);

    ui->fcbDefaultFont->setCurrentFont(QFont(settings->fontName()));
    connect(ui->fcbDefaultFont, &QFontComboBox::currentFontChanged, this, [settings](const QFont &f) {
        settings->setFontName(f.family());
    });
    connect(settings, &ApplicationSettings::fontNameChanged, this, [this](const QString &fontName){
        ui->fcbDefaultFont->setCurrentFont(QFont(fontName));
    });

    ui->spbDefaultFontSize->setValue(settings->fontSize());
    connect(ui->spbDefaultFontSize, QOverload<int>::of(&QSpinBox::valueChanged), settings, &ApplicationSettings::setFontSize);
    connect(settings, &ApplicationSettings::fontSizeChanged, ui->spbDefaultFontSize, &QSpinBox::setValue);

    ui->comboBoxLineEndings->addItem(tr("System Default"), QString(""));
    ui->comboBoxLineEndings->addItem(tr("Windows (CR LF)"), ScintillaNext::eolModeToString(SC_EOL_CRLF));
    ui->comboBoxLineEndings->addItem(tr("Linux (LF)"), ScintillaNext::eolModeToString(SC_EOL_LF));
    ui->comboBoxLineEndings->addItem(tr("Macintosh (CR)"), ScintillaNext::eolModeToString(SC_EOL_CR));

    // Select the current one
    int index = ui->comboBoxLineEndings->findData(settings->defaultEOLMode());
    ui->comboBoxLineEndings->setCurrentIndex(index == -1 ? 0 : index);

    connect(ui->comboBoxLineEndings, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, settings](int eolIndex) {
        settings->setDefaultEOLMode(ui->comboBoxLineEndings->itemData(eolIndex).toString());
    });
    connect(settings, &ApplicationSettings::defaultEOLModeChanged, this, [this](const QString &defaultEOLMode) {
        int foundIdx = ui->comboBoxLineEndings->findData(defaultEOLMode);
        ui->comboBoxLineEndings->setCurrentIndex(foundIdx == -1 ? 0 : foundIdx);
    });

    NotepadNextApplication *app = qobject_cast<NotepadNextApplication *>(qApp);
    ui->comboBoxDefaultLanguage->addItems(app->getLanguages());

    // Select the current one
    ui->comboBoxDefaultLanguage->setCurrentText(settings->defaultLanguage());

    connect(ui->comboBoxDefaultLanguage, &QComboBox::currentTextChanged, settings, &ApplicationSettings::setDefaultLanguage);
    connect(settings, &ApplicationSettings::defaultLanguageChanged, ui->comboBoxDefaultLanguage, &QComboBox::setCurrentText);

    MapSettingToCheckBox(ui->checkBoxHighlightURLs, &ApplicationSettings::urlHighlighting, &ApplicationSettings::setURLHighlighting, &ApplicationSettings::urlHighlightingChanged);
    MapSettingToCheckBox(ui->checkBoxShowLineNumbers, &ApplicationSettings::showLineNumbers, &ApplicationSettings::setShowLineNumbers, &ApplicationSettings::showLineNumbersChanged);
    MapSettingToCheckBox(ui->checkBoxAutoCompletion, &ApplicationSettings::autoCompletion, &ApplicationSettings::setAutoCompletion, &ApplicationSettings::autoCompletionChanged);

    auto *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->radioFollowCurrentDirectory, static_cast<int>(ApplicationSettings::DefaultDirectoryBehaviorEnum::FollowCurrentDocument));
    buttonGroup->addButton(ui->radioLastUsedDirectory, static_cast<int>(ApplicationSettings::DefaultDirectoryBehaviorEnum::RememberLastUsed));
    buttonGroup->addButton(ui->radioHardCoded, static_cast<int>(ApplicationSettings::DefaultDirectoryBehaviorEnum::HardCoded));

    connect(buttonGroup, &QButtonGroup::idClicked, this, [settings](int id) {
        auto e = static_cast<ApplicationSettings::DefaultDirectoryBehaviorEnum>(id);
        settings->setDefaultDirectoryBehavior(e);
    });

    connect(ui->radioHardCoded, &QRadioButton::toggled, this, [this](bool checked){
        ui->btnSelectHardCodedPath->setEnabled(checked);
        ui->txtHardCodedPath->setEnabled(checked);
    });

    connect(ui->btnSelectHardCodedPath, &QToolButton::clicked, this, [this, settings]() {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Default Directory"));
        if (dir.isEmpty()) {
            return; // user cancelled
        }

        settings->setDefaultDirectory(QDir::fromNativeSeparators(dir));
        ui->txtHardCodedPath->setText(QDir::toNativeSeparators(dir));
    });

    connect(ui->txtHardCodedPath, &QLineEdit::editingFinished, this, [this, settings]() {
        QString dir = ui->txtHardCodedPath->text();
        settings->setDefaultDirectory(QDir::fromNativeSeparators(dir));
        ui->txtHardCodedPath->setText(QDir::toNativeSeparators(dir));
    });

    if (auto b = buttonGroup->button(static_cast<int>(settings->defaultDirectoryBehavior()))) {
        b->setChecked(true);
    }

    if (settings->defaultDirectoryBehavior() == ApplicationSettings::DefaultDirectoryBehaviorEnum::HardCoded) {
        ui->txtHardCodedPath->setText(QDir::toNativeSeparators(settings->defaultDirectory()));
    }
    else {
        ui->txtHardCodedPath->setText(QString());
    }
}

PreferencesDialog::~PreferencesDialog() = default;

void PreferencesDialog::showApplicationRestartRequired() const
{
    ui->labelAppRestartIcon->show();
    ui->labelAppRestart->show();
}

template<typename Func1, typename Func2, typename Func3>
void PreferencesDialog::MapSettingToCheckBox(QCheckBox *checkBox, Func1 getter, Func2 setter, Func3 notifier) const
{
    // Get the value and set the checkbox state
    checkBox->setChecked(std::bind_front(getter, settings)());

    // Set up two way connection
    connect(settings, notifier, checkBox, &QCheckBox::setChecked);
    connect(checkBox, &QCheckBox::toggled, settings, setter);
}

template<typename Func1, typename Func2, typename Func3>
void PreferencesDialog::MapSettingToGroupBox(QGroupBox *groupBox, Func1 getter, Func2 setter, Func3 notifier) const
{
    // Get the value and set the checkbox state
    groupBox->setChecked(std::bind_front(getter, settings)());

    // Set up two way connection
    connect(settings, notifier, groupBox, &QGroupBox::setChecked);
    connect(groupBox, &QGroupBox::toggled, settings, setter);
}

void PreferencesDialog::populateTranslationComboBox()
{
    const auto *app = qobject_cast<NotepadNextApplication *>(qApp);

    // Add the system default at the top
    ui->comboBoxTranslation->addItem(tr("<System Default>"), QStringLiteral(""));

    // TODO: sort this list and keep the system default at the top
    for (const auto &localeName : app->getTranslationManager()->availableTranslations())
    {
        QLocale locale(localeName);
        const QString localeDisplay = TranslationManager::FormatLocaleTerritoryAndLanguage(locale);
        ui->comboBoxTranslation->addItem(localeDisplay, localeName);
    }

    // Select the current one
    int index = ui->comboBoxTranslation->findData(settings->translation());
    if (index != -1) {
        ui->comboBoxTranslation->setCurrentIndex(index);
    }
}

void PreferencesDialog::setupThemeSettings()
{
    auto *gbxThemes = new QGroupBox(tr("Themes & Visual Styling"), ui->scrollAreaWidgetContents);
    auto *themeLayout = new QFormLayout(gbxThemes);
    themeLayout->setSpacing(10);
    themeLayout->setContentsMargins(14, 16, 14, 14);

    // 1. Theme Palette Selector
    comboBoxTheme = new QComboBox(gbxThemes);
    comboBoxTheme->setMaxVisibleItems(12);
    if (comboBoxTheme->view()) {
        comboBoxTheme->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    int selectedIdx = 0;
    QString currentId = ThemeManager::instance()->currentPaletteId();
    QString lastFamily;
    for (const auto &pal : kAllThemePalettes) {
        QString family = QString::fromUtf8(pal.family);
        if (family != lastFamily) {
            if (!lastFamily.isEmpty()) {
                comboBoxTheme->insertSeparator(comboBoxTheme->count());
            }
            lastFamily = family;
        }
        QString label = QStringLiteral("[%1] %2 (%3)")
                            .arg(family, QString::fromUtf8(pal.displayName), pal.isDark ? tr("Dark") : tr("Light"));
        comboBoxTheme->addItem(label, QString::fromLatin1(pal.id));
        if (pal.id == currentId) {
            selectedIdx = comboBoxTheme->count() - 1;
        }
    }
    comboBoxTheme->setCurrentIndex(selectedIdx);
    connect(comboBoxTheme, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index < 0) {
            return;
        }
        QString themeId = comboBoxTheme->itemData(index).toString();
        if (themeId.isEmpty()) {
            return;
        }
        auto *mw = qobject_cast<MainWindow *>(parentWidget());
        ThemeManager::instance()->setPalette(themeId, mw);
    });

    auto *themeRowLayout = new QHBoxLayout();
    themeRowLayout->addWidget(comboBoxTheme, 1);

    btnToggleDarkLight = new QPushButton(tr("Toggle Dark/Light"), gbxThemes);
    btnToggleDarkLight->setToolTip(tr("Quickly switch between active dark and light modes"));
    connect(btnToggleDarkLight, &QPushButton::clicked, this, [this]() {
        auto *mw = qobject_cast<MainWindow *>(parentWidget());
        ThemeManager::instance()->toggleTheme(mw);
        QString currentId = ThemeManager::instance()->currentPaletteId();
        int found = comboBoxTheme->findData(currentId);
        if (found >= 0) {
            QSignalBlocker blocker(comboBoxTheme);
            comboBoxTheme->setCurrentIndex(found);
        }
    });
    themeRowLayout->addWidget(btnToggleDarkLight);
    themeLayout->addRow(tr("Active Theme:"), themeRowLayout);

    // Insert at the top of scrollAreaWidgetContents layout
    ui->verticalLayout_4->insertWidget(0, gbxThemes);
}

void PreferencesDialog::updateThemeColors()
{
    if (comboBoxTheme) {
        QString currentId = ThemeManager::instance()->currentPaletteId();
        int found = comboBoxTheme->findData(currentId);
        if (found >= 0 && comboBoxTheme->currentIndex() != found) {
            QSignalBlocker blocker(comboBoxTheme);
            comboBoxTheme->setCurrentIndex(found);
        }
    }
}
