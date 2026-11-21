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

#include "DockedEditor.h"
#include "DockAreaTabBar.h"
#include "DockAreaWidget.h"
#include "DockWidgetTab.h"
#include "DockComponentsFactory.h"
#include "DockedEditorTitleBar.h"
#include "DockAreaTitleBar.h"

#include "ScintillaNext.h"
#include "features/TabColorManager.h"
#include "features/ThemeManager.h"
#include "features/ThemePaletteData.h"

#include <QUuid>
#include <algorithm>


class DockedEditorComponentsFactory : public ads::CDockComponentsFactory
{
public:
    ads::CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* DockArea) const override {
        auto *titleBar = new DockedEditorTitleBar(DockArea);

        // Disable the built in context menu for the title bar since it has options we don't want
        titleBar->setContextMenuPolicy(Qt::NoContextMenu);

        return titleBar;
    }
};


DockedEditor::DockedEditor(QWidget *parent) : QObject(parent)
{
    ads::CDockComponentsFactory::setFactory(new DockedEditorComponentsFactory());

    ads::CDockManager::setConfigFlag(ads::CDockManager::AllTabsHaveCloseButton, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewIsDynamic, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewShowsContentPixmap, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    // When tabs title/text elide disabled and lots of tabs opened, tabs menu button will not show
    // as it only shows when tab title elided. 
    // So disable dynamic tabs menu visibility.
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaDynamicTabsMenuButtonVisibility, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::EqualSplitOnInsertion, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::MiddleMouseButtonClosesTab, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasTabsMenuButton, false);

    dockManager = new ads::CDockManager(parent);
    dockManager->setStyleSheet("");

    connect(dockManager, &ads::CDockManager::focusedDockWidgetChanged, this, &DockedEditor::onFocusedDockWidgetChanged);

    connect(dockManager, &ads::CDockManager::dockAreaCreated, this, [this](const ads::CDockAreaWidget* DockArea) {
        const auto *titleBar = qobject_cast<const DockedEditorTitleBar *>(DockArea->titleBar());
        connect(titleBar, &DockedEditorTitleBar::doubleClicked, this, &DockedEditor::titleBarDoubleClicked);
        connect(titleBar, &DockedEditorTitleBar::plusButtonClicked, this, &DockedEditor::newFileRequested);

        connect(DockArea->titleBar()->tabBar(), &ads::CDockAreaTabBar::tabMoved, this, [this](int /*from*/, int /*to*/) {
            emit editorOrderChanged();
        });
    });

    connect(TabColorManager::instance(), &TabColorManager::tabColorChanged, this, [this](const QString &filePath, const QColor &color) {
        for (auto *ed : editors()) {
            if (ed) {
                const QString key = ed->isFile() ? ed->getFilePath() : ed->getName();
                if (key == filePath) {
                    setEditorTabColor(ed, color);
                }
            }
        }
    });

    connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, [this](bool /*isDark*/) {
        refreshAllEditorTabColors();
    });

    connect(ThemeManager::instance(), &ThemeManager::paletteChanged, this, [this](const QString & /*paletteId*/) {
        refreshAllEditorTabColors();
    });
}

namespace {
ScintillaNext *extractActiveEditor(const ads::CDockWidget *dockWidget)
{
    if (!dockWidget || dockWidget->isClosed()) {
        return nullptr;
    }
    return qobject_cast<ScintillaNext *>(dockWidget->widget());
}

bool isEditorOpen(const ScintillaNext *editor)
{
    if (!editor) {
        return false;
    }
    const auto *dockWidget = qobject_cast<const ads::CDockWidget *>(editor->parentWidget());
    return !dockWidget || !dockWidget->isClosed();
}

ScintillaNext *findFallbackEditor(const ads::CDockManager *mgr, ads::CDockAreaWidget *&targetArea)
{
    for (ads::CDockAreaWidget *areaWidget : mgr->openedDockAreas()) {
        if (!areaWidget) {
            continue;
        }
        if (auto *ed = extractActiveEditor(areaWidget->currentDockWidget())) {
            targetArea = areaWidget;
            return ed;
        }
        const auto widgets = areaWidget->dockWidgets();
        const auto it = std::ranges::find_if(widgets, [](const ads::CDockWidget *w) {
            return extractActiveEditor(w) != nullptr;
        });
        if (it != widgets.cend()) {
            targetArea = areaWidget;
            return extractActiveEditor(*it);
        }
    }
    return nullptr;
}
}

void DockedEditor::onFocusedDockWidgetChanged(const ads::CDockWidget * /*old*/, const ads::CDockWidget *now)
{
    if (now && !now->isClosed()) {
        if (auto *editor = qobject_cast<ScintillaNext *>(now->widget())) {
            currentEditor = editor;
            latestDockArea = now->dockAreaWidget();
            editor->grabFocus();
            emit editorActivated(editor);
            return;
        }
    }

    ads::CDockAreaWidget *fallbackArea = nullptr;
    currentEditor = findFallbackEditor(dockManager, fallbackArea);
    if (fallbackArea) {
        latestDockArea = fallbackArea;
    }
    if (currentEditor) {
        currentEditor->grabFocus();
        emit editorActivated(currentEditor.data());
    }
}


ScintillaNext *DockedEditor::getCurrentEditor() const
{
    if (isEditorOpen(currentEditor.data())) {
        return currentEditor.data();
    }

    for (const ads::CDockAreaWidget *areaWidget : dockManager->openedDockAreas()) {
        if (!areaWidget) {
            continue;
        }
        if (auto *ed = extractActiveEditor(areaWidget->currentDockWidget())) {
            return ed;
        }
        const auto widgets = areaWidget->dockWidgets();
        const auto it = std::ranges::find_if(widgets, [](const ads::CDockWidget *w) {
            return extractActiveEditor(w) != nullptr;
        });
        if (it != widgets.cend()) {
            return extractActiveEditor(*it);
        }
    }

    return nullptr;
}

int DockedEditor::count() const
{
    int total = 0;

    for (int i = 0; i < dockManager->dockAreaCount(); ++i) {
        if (const auto *area = dockManager->dockArea(i)) {
            total += area->openDockWidgetsCount();
        }
    }

    return total;
}

QVector<ScintillaNext *> DockedEditor::editors() const
{
    QVector<ScintillaNext *> editors;

    for (const ads::CDockAreaWidget *areaWidget : dockManager->openedDockAreas()) {
        if (!areaWidget) {
            continue;
        }
        for (const ads::CDockWidget *dockWidget : areaWidget->dockWidgets()) {
            if (auto *ed = extractActiveEditor(dockWidget)) {
                editors.append(ed);
            }
        }
    }

    return editors;
}

void DockedEditor::switchToEditor(const ScintillaNext *editor) const
{
    if (!editor) {
        return;
    }
    if (auto *dockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget())) {
        dockWidget->setAsCurrentTab();
        dockWidget->raise();
    }
    else {
        qWarning() << "Expected editor's parent to be CDockWidget";
    }
}

void DockedEditor::dockWidgetCloseRequested()
{
    auto *dockWidget = qobject_cast<ads::CDockWidget *>(sender());
    if (!dockWidget || !dockWidget->widget()) {
        return;
    }
    if (auto *editor = qobject_cast<ScintillaNext *>(dockWidget->widget())) {
        emit editorCloseRequested(editor);
    } else {
        dockWidget->closeDockWidget();
    }
}

ads::CDockWidget *DockedEditor::addTabWidget(QWidget *widget, const QString &title, const QIcon &icon,
                                             const QString &toolTip)
{
    if (!widget) {
        return nullptr;
    }

    ads::CDockWidget *dockWidget = dockManager->createDockWidget(title);
    dockWidget->tabWidget()->setElideMode(Qt::ElideNone);
    dockWidget->setObjectName(QUuid::createUuid().toString());
    dockWidget->setWidget(widget);
    dockWidget->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetDeleteOnClose, true);
    dockWidget->setFeature(ads::CDockWidget::DockWidgetFeature::CustomCloseHandling, true);
    dockWidget->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetFloatable, false);

    if (!icon.isNull()) {
        dockWidget->tabWidget()->setIcon(icon);
    }

    if (!toolTip.isEmpty()) {
        dockWidget->tabWidget()->setToolTip(toolTip);
    } else {
        dockWidget->tabWidget()->setToolTip(title);
    }

    connect(dockWidget, &ads::CDockWidget::closeRequested, this, &DockedEditor::dockWidgetCloseRequested);

    if (ads::CDockAreaWidget *area = currentDockArea()) {
        dockManager->addDockWidget(ads::CenterDockWidgetArea, dockWidget, area);
    } else {
        dockManager->addDockWidget(ads::CenterDockWidgetArea, dockWidget);
    }

    dockWidget->setAsCurrentTab();
    dockWidget->raise();
    return dockWidget;
}

ads::CDockAreaWidget *DockedEditor::currentDockArea() const
{
    if (const ads::CDockWidget *focused = dockManager->focusedDockWidget()) {
        if (ads::CDockAreaWidget *area = focused->dockAreaWidget()) {
            return area;
        }
    }

    if (latestDockArea) {
        return latestDockArea.data();
    }

    if (dockManager->dockAreaCount() > 0) {
        return dockManager->dockArea(0);
    }

    return nullptr;
}

void DockedEditor::addEditor(ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    Q_ASSERT(editor != Q_NULLPTR);

    if (currentEditor == Q_NULLPTR) {
        currentEditor = editor;
    }

    // Create the dock widget for the editor
    ads::CDockWidget *dockWidget = dockManager->createDockWidget(editor->getName());

    // Disable elide, elided file names not readable when lots of files opened
    dockWidget->tabWidget()->setElideMode(Qt::ElideNone);

    // We need a unique object name. Can't use the name or file path so use a uuid
    dockWidget->setObjectName(QUuid::createUuid().toString());

    dockWidget->setWidget(editor);
    dockWidget->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetDeleteOnClose, true);
    dockWidget->setFeature(ads::CDockWidget::DockWidgetFeature::CustomCloseHandling, true);
    dockWidget->setFeature(ads::CDockWidget::DockWidgetFeature::DockWidgetFloatable, false);

    dockWidget->tabWidget()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(dockWidget->tabWidget(), &QWidget::customContextMenuRequested, this, [this, editor](const QPoint & /*pos*/) {
        emit contextMenuRequestedForEditor(editor);
    });

    dockWidget->tabWidget()->setToolTip(editor->getToolTip());

    // Set the icon
    if (editor->readOnly()) {
        dockWidget->tabWidget()->setIcon(QIcon(":/icons/readonly.png"));
    }
    else {
        dockWidget->tabWidget()->setIcon(QIcon(editor->canSaveToDisk() ? ":/icons/unsaved.png" : ":/icons/saved.png"));
        connect(editor, &ScintillaNext::savePointChanged, dockWidget, [editor, dockWidget](bool /*dirty*/) {
            const bool actuallyDirty = editor->canSaveToDisk();
            const QString iconPath = actuallyDirty ? QStringLiteral(":/icons/unsaved.png") : QStringLiteral(":/icons/saved.png");
            dockWidget->tabWidget()->setIcon(QIcon(iconPath));
        });
    }

    connect(editor, &ScintillaNext::closed, dockWidget, &ads::CDockWidget::closeDockWidget);
    connect(editor, &ScintillaNext::closed, this, [this, editor]() {
        if (currentEditor == editor) {
            currentEditor = nullptr;
        }
        emit editorClosed(editor);
    });
    connect(editor, &QObject::destroyed, this, [this](QObject *obj) {
        if (currentEditor == obj) {
            currentEditor = nullptr;
        }
        auto edList = editors();
        edList.removeAll(static_cast<ScintillaNext *>(obj));
        if (!edList.isEmpty()) {
            currentEditor = edList.first();
            emit editorActivated(currentEditor.data());
        } else {
            currentEditor = nullptr;
        }
    });
    connect(editor, &ScintillaNext::renamed, this, [this, editor]() { editorRenamed(editor); });

    connect(dockWidget, &ads::CDockWidget::closeRequested, this, &DockedEditor::dockWidgetCloseRequested);

    latestDockArea = dockManager->addDockWidget(ads::CenterDockWidgetArea, dockWidget, currentDockArea());

    const QString pathKey = editor->isFile() ? editor->getFilePath() : editor->getName();
    const QColor initialColor = TabColorManager::instance()->tabColor(pathKey);
    if (initialColor.isValid()) {
        setEditorTabColor(editor, initialColor);
    }

    emit editorAdded(editor);
}

void DockedEditor::setEditorTabColor(ScintillaNext *editor, const QColor &color)
{
    if (!editor) {
        return;
    }
    editor->setQtProperty("tabColor", color);
    auto *dockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget());
    if (!dockWidget || !dockWidget->tabWidget()) {
        return;
    }

    if (!color.isValid()) {
        dockWidget->tabWidget()->setStyleSheet(QString());
        return;
    }

    const bool isDark = ThemeManager::instance()->isDarkActive();
    const ThemePalette *pal = ThemeManager::instance()->currentPalette();
    const QString cardBg =
        pal ? QString::fromLatin1(pal->card) : (isDark ? QStringLiteral("#282c34") : QStringLiteral("#ffffff"));
    const QString borderCol =
        pal ? QString::fromLatin1(pal->border) : (isDark ? QStringLiteral("#3e4451") : QStringLiteral("#d0d7de"));
    const QString textPri =
        pal ? QString::fromLatin1(pal->txtPri) : (isDark ? QStringLiteral("#e6edf3") : QStringLiteral("#24292f"));
    const QString textSec =
        pal ? QString::fromLatin1(pal->txtSec) : (isDark ? QStringLiteral("#8b949e") : QStringLiteral("#57606a"));
    const QString colorHex = color.name(QColor::HexRgb);
    const int r = color.red();
    const int g = color.green();
    const int b = color.blue();

    const QString style =
        QStringLiteral(
            "ads--CDockWidgetTab {"
            "  background-color: rgba(%1, %2, %3, 0.16);"
            "  margin-top: 2px;"
            "  margin-right: 2px;"
            "  border: 1px solid rgba(%1, %2, %3, 0.35);"
            "  border-top: 3px solid %4;"
            "  border-bottom: 1px solid %5;"
            "  padding: 3px 10px 2px 10px;"
            "  border-top-left-radius: 5px;"
            "  border-top-right-radius: 5px;"
            "}"
            "ads--CDockWidgetTab ads--CElidingLabel,"
            "ads--CDockWidgetTab QLabel {"
            "  color: %6;"
            "  font-weight: 500;"
            "}"
            "ads--CDockWidgetTab:hover[activeTab=\"false\"] {"
            "  background-color: rgba(%1, %2, %3, 0.26);"
            "  border: 1px solid rgba(%1, %2, %3, 0.55);"
            "  border-top: 3px solid %4;"
            "}"
            "ads--CDockWidgetTab:hover[activeTab=\"false\"] ads--CElidingLabel,"
            "ads--CDockWidgetTab:hover[activeTab=\"false\"] QLabel {"
            "  color: %7;"
            "}"
            "ads--CDockWidgetTab[activeTab=\"true\"] {"
            "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(%1, %2, %3, 0.30), stop:1 %8);"
            "  border-left: 1px solid rgba(%1, %2, %3, 0.45);"
            "  border-right: 1px solid rgba(%1, %2, %3, 0.45);"
            "  border-top: 4px solid %4;"
            "  border-bottom: 1px solid %8;"
            "  padding-bottom: 4px;"
            "  margin-top: 0px;"
            "}"
            "ads--CDockWidgetTab[activeTab=\"true\"] ads--CElidingLabel,"
            "ads--CDockWidgetTab[activeTab=\"true\"] QLabel {"
            "  color: %7;"
            "  font-weight: 600;"
            "}"
            "ads--CDockWidgetTab[activeTab=\"true\"][focused=\"true\"],"
            "ads--CDockWidgetTab[focused=\"true\"] {"
            "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(%1, %2, %3, 0.40), stop:1 %8);"
            "  border-left: 1px solid rgba(%1, %2, %3, 0.60);"
            "  border-right: 1px solid rgba(%1, %2, %3, 0.60);"
            "  border-top: 4px solid %4;"
            "  border-bottom: 1px solid %8;"
            "  padding-bottom: 4px;"
            "  margin-top: 0px;"
            "}"
            "ads--CDockWidgetTab[activeTab=\"true\"][focused=\"true\"] ads--CElidingLabel,"
            "ads--CDockWidgetTab[activeTab=\"true\"][focused=\"true\"] QLabel,"
            "ads--CDockWidgetTab[focused=\"true\"] ads--CElidingLabel,"
            "ads--CDockWidgetTab[focused=\"true\"] QLabel {"
            "  color: %7;"
            "  font-weight: 600;"
            "}"
            "#tabCloseButton {"
            "  background: transparent;"
            "  margin-bottom: 1px;"
            "  margin-left: 4px;"
            "  border: none;"
            "  padding: 0px;"
            "  border-radius: 3px;"
            "}"
            "#tabCloseButton:hover {"
            "  background-color: rgba(235, 87, 87, 0.35);"
            "  color: #eb5757;"
            "}"
            "#tabCloseButton:pressed {"
            "  background-color: #eb5757;"
            "  color: #ffffff;"
            "}")
            .arg(QString::number(r), QString::number(g), QString::number(b), colorHex, borderCol)
            .arg(textSec, textPri, cardBg);

    dockWidget->tabWidget()->setStyleSheet(style);
}

QColor DockedEditor::editorTabColor(const ScintillaNext *editor) const
{
    if (!editor) {
        return QColor();
    }
    return editor->qtProperty("tabColor").value<QColor>();
}

void DockedEditor::refreshAllEditorTabColors()
{
    for (auto *ed : editors()) {
        if (ed) {
            const QColor c = editorTabColor(ed);
            if (c.isValid()) {
                setEditorTabColor(ed, c);
            }
        }
    }
}

void DockedEditor::editorRenamed(const ScintillaNext *editor) const
{
    Q_ASSERT(editor != Q_NULLPTR);

    if (auto *dockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget())) {
        dockWidget->setWindowTitle(editor->getName());
        dockWidget->tabWidget()->setToolTip(editor->getToolTip());
    }

    const QColor color = editorTabColor(editor);
    if (color.isValid() && editor->isFile()) {
        TabColorManager::instance()->setTabColor(editor->getFilePath(), color);
    }
}

void DockedEditor::splitToRight(const ScintillaNext *editor)
{
    if (!editor) {
        return;
    }

    auto *newDockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget());
    ads::CDockAreaWidget *currentArea = currentDockArea();
    if (newDockWidget && currentArea) {
        latestDockArea = dockManager->addDockWidget(ads::RightDockWidgetArea, newDockWidget, currentArea);
    }
}

void DockedEditor::splitToBottom(const ScintillaNext *editor)
{
    if (!editor) {
        return;
    }

    auto *newDockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget());
    ads::CDockAreaWidget *currentArea = currentDockArea();
    if (newDockWidget && currentArea) {
        latestDockArea = dockManager->addDockWidget(ads::BottomDockWidgetArea, newDockWidget, currentArea);
    }
}
