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
#include "CustomizeToolbarDialog.h"
#include "features/ToolbarIconManager.h"
#include "ui_PreferencesDialog.h"
#include "ScintillaNext.h"

#include <array>
#include <QButtonGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QFormLayout>
#include <QComboBox>
#include <QPushButton>
#include <QAbstractItemView>
#include <QScrollArea>
#include <QLabel>
#include <QTabWidget>
#include <QToolBar>


PreferencesDialog::PreferencesDialog(ApplicationSettings *settings, QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(std::make_unique<Ui::PreferencesDialog>()),
    settings(settings)
{
    ui->setupUi(this);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setObjectName(QStringLiteral("preferencesTabWidget"));

    ui->verticalLayout->removeWidget(ui->scrollArea);
    m_tabWidget->addTab(ui->scrollArea, QIcon(QStringLiteral(":/icons/saved.png")), tr("&General"));

    setupThemesAndToolbarTab();

    if (auto *mainWin = qobject_cast<MainWindow *>(parent)) {
        auto *toolbarWidget = new CustomizeToolbarWidget(mainWin, this);
        m_tabWidget->addTab(toolbarWidget, QIcon(QStringLiteral(":/icons/find.png")), tr("&Customize Toolbar"));
    }

    ui->verticalLayout->insertWidget(0, m_tabWidget);

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

    if (const auto *app = qobject_cast<const NotepadNextApplication *>(qApp); app) {
        ui->comboBoxDefaultLanguage->addItems(app->getLanguages());
    }

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

    // Sorted alphabetically by display name while keeping the system default at the top
    const auto translations = app->getTranslationManager()->availableTranslations();
    std::vector<QPair<QString, QString>> translationList;
    translationList.reserve(translations.size());
    for (const auto &localeName : translations)
    {
        QLocale locale(localeName);
        translationList.emplace_back(TranslationManager::FormatLocaleTerritoryAndLanguage(locale), localeName);
    }
    std::ranges::sort(translationList, [](const auto &a, const auto &b) {
        return a.first.localeAwareCompare(b.first) < 0;
    });
    for (const auto &item : translationList)
    {
        ui->comboBoxTranslation->addItem(item.first, item.second);
    }

    // Select the current one
    int index = ui->comboBoxTranslation->findData(settings->translation());
    if (index != -1) {
        ui->comboBoxTranslation->setCurrentIndex(index);
    }
}

void PreferencesDialog::setupThemesAndToolbarTab()
{
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *container = new QWidget(scrollArea);
    auto *mainLayout = new QVBoxLayout(container);
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(14, 16, 14, 14);

    setupThemesGroup(container);
    setupToolbarPacksGroup(container);
    mainLayout->addStretch();

    scrollArea->setWidget(container);
    m_themesAndToolbarWidget = scrollArea;
    m_tabWidget->addTab(m_themesAndToolbarWidget, QIcon(QStringLiteral(":/icons/find.png")), tr("&Themes && Toolbar"));
}

void PreferencesDialog::setupThemesGroup(QWidget *container)
{
    auto *gbxThemes = new QGroupBox(tr("Themes & Visual Styling"), container);
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
    const QString currentId = ThemeManager::instance()->currentPaletteId();
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
        const QString themeId = comboBoxTheme->itemData(index).toString();
        if (themeId.isEmpty()) {
            return;
        }
        auto *mw = qobject_cast<MainWindow *>(parentWidget());
        ThemeManager::instance()->setPalette(themeId, mw);
    });

    auto *themeRowWidget = new QWidget(gbxThemes);
    auto *themeRowLayout = new QHBoxLayout(themeRowWidget);
    themeRowLayout->setContentsMargins(0, 0, 0, 0);
    themeRowLayout->addWidget(comboBoxTheme, 1);

    btnToggleDarkLight = new QPushButton(tr("Toggle Dark/Light"), gbxThemes);
    btnToggleDarkLight->setToolTip(tr("Quickly switch between active dark and light modes"));
    connect(btnToggleDarkLight, &QPushButton::clicked, this, [this]() {
        auto *mw = qobject_cast<MainWindow *>(parentWidget());
        ThemeManager::instance()->toggleTheme(mw);
        const QString cur = ThemeManager::instance()->currentPaletteId();
        const int found = comboBoxTheme->findData(cur);
        if (found >= 0) {
            const QSignalBlocker blocker(comboBoxTheme);
            comboBoxTheme->setCurrentIndex(found);
        }
    });
    themeRowLayout->addWidget(btnToggleDarkLight);
    themeLayout->addRow(tr("Active Theme:"), themeRowWidget);

    container->layout()->addWidget(gbxThemes);
}

void PreferencesDialog::setupToolbarPacksGroup(QWidget *container)
{
    auto *gbxToolbarPacks = new QGroupBox(tr("Toolbar Icon Packs & Sizing"), container);
    auto *toolbarLayout = new QFormLayout(gbxToolbarPacks);
    toolbarLayout->setSpacing(10);
    toolbarLayout->setContentsMargins(14, 16, 14, 14);

    // Active Icon Pack Selector
    comboBoxIconPack = new QComboBox(gbxToolbarPacks);
    comboBoxIconPack->setMaxVisibleItems(14);
    if (comboBoxIconPack->view()) {
        comboBoxIconPack->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    populateIconPacksComboBox();
    toolbarLayout->addRow(tr("Active Icon Pack:"), comboBoxIconPack);

    // Pack Description
    lblIconPackDescription = new QLabel(gbxToolbarPacks);
    lblIconPackDescription->setWordWrap(true);
    lblIconPackDescription->setTextFormat(Qt::PlainText);
    lblIconPackDescription->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    lblIconPackDescription->setMinimumHeight(44);
    lblIconPackDescription->setStyleSheet(QStringLiteral("color: #64748b; font-size: 12px; line-height: 1.4; padding: 4px 8px; background: rgba(0, 0, 0, 0.04); border-radius: 4px;"));
    if (const auto *pack = ToolbarIconManager::instance()->findPackById(ToolbarIconManager::instance()->currentPackId())) {
        lblIconPackDescription->setText(pack->description);
    }
    toolbarLayout->addRow(tr("Description:"), lblIconPackDescription);

    connect(comboBoxIconPack, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (idx < 0) {
            return;
        }
        const QString packId = comboBoxIconPack->itemData(idx).toString();
        if (packId.isEmpty()) {
            return;
        }
        const auto *mw = qobject_cast<const MainWindow *>(parentWidget());
        QToolBar *tb = mw ? mw->findChild<QToolBar *>() : nullptr;
        ToolbarIconManager::instance()->setPack(packId, tb);
        if (const auto *pack = ToolbarIconManager::instance()->findPackById(packId); pack && lblIconPackDescription) {
            const QString desc = pack->description.trimmed().isEmpty()
                                     ? tr("No description provided for this icon pack.")
                                     : pack->description;
            lblIconPackDescription->setText(desc);
        }
    });

    // Icon Size
    comboBoxIconSize = new QComboBox(gbxToolbarPacks);
    struct SizeEntry {
        QString label;
        QSize size;
    };
    const std::array<SizeEntry, 6> sizes = {{
        {tr("16 x 16 (Compact)"), QSize(16, 16)},
        {tr("20 x 20 (Medium)"), QSize(20, 20)},
        {tr("24 x 24 (Default / Recommended)"), QSize(24, 24)},
        {tr("32 x 32 (Large / High-DPI)"), QSize(32, 32)},
        {tr("48 x 48 (Extra Large)"), QSize(48, 48)},
        {tr("64 x 64 (Ultra High-DPI)"), QSize(64, 64)},
    }};
    const QSize currentSize = ToolbarIconManager::instance()->iconSize();
    int sizeIdx = 2;
    for (size_t i = 0; i < sizes.size(); ++i) {
        comboBoxIconSize->addItem(sizes[i].label, QVariant(sizes[i].size));
        if (sizes[i].size == currentSize) {
            sizeIdx = static_cast<int>(i);
        }
    }
    comboBoxIconSize->setCurrentIndex(sizeIdx);
    connect(comboBoxIconSize, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (idx < 0) {
            return;
        }
        const QSize size = comboBoxIconSize->itemData(idx).toSize();
        const auto *mw = qobject_cast<const MainWindow *>(parentWidget());
        QToolBar *tb = mw ? mw->findChild<QToolBar *>() : nullptr;
        ToolbarIconManager::instance()->setIconSize(size, tb);
    });
    toolbarLayout->addRow(tr("Toolbar Icon Size:"), comboBoxIconSize);

    container->layout()->addWidget(gbxToolbarPacks);
}

void PreferencesDialog::populateIconPacksComboBox()
{
    if (!comboBoxIconPack) {
        return;
    }
    const QSignalBlocker blocker(comboBoxIconPack);
    comboBoxIconPack->clear();
    const auto &packs = ToolbarIconManager::instance()->availablePacks();
    const QString currentPack = ToolbarIconManager::instance()->currentPackId();
    int currentIdx = 0;
    for (int i = 0; i < packs.size(); ++i) {
        const auto &pack = packs[i];
        QString label = pack.displayName;
        if (!pack.isBuiltIn) {
            label += tr(" [Custom]");
        }
        comboBoxIconPack->addItem(label, pack.id);
        if (pack.id == currentPack) {
            currentIdx = i;
        }
    }
    comboBoxIconPack->setCurrentIndex(currentIdx);
    if (const auto *pack = ToolbarIconManager::instance()->findPackById(currentPack); pack && lblIconPackDescription) {
        lblIconPackDescription->setText(pack->description);
    }
}

void PreferencesDialog::updateThemeColors()
{
    if (comboBoxTheme) {
        const QString currentId = ThemeManager::instance()->currentPaletteId();
        const int found = comboBoxTheme->findData(currentId);
        if (found >= 0 && comboBoxTheme->currentIndex() != found) {
            const QSignalBlocker blocker(comboBoxTheme);
            comboBoxTheme->setCurrentIndex(found);
        }
    }
}
