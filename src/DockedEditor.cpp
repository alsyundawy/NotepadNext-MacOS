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

#include "DockedEditor.h"
#include "DockAreaTabBar.h"
#include "DockAreaWidget.h"
#include "DockWidgetTab.h"
#include "DockComponentsFactory.h"
#include "DockedEditorTitleBar.h"
#include "DockAreaTitleBar.h"

#include "ScintillaNext.h"

#include <QUuid>


class DockedEditorComponentsFactory : public ads::CDockComponentsFactory
{
public:
    ads::CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* DockArea) const {
        DockedEditorTitleBar *titleBar = new DockedEditorTitleBar(DockArea);

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

    connect(dockManager, &ads::CDockManager::focusedDockWidgetChanged, this, [=, this](ads::CDockWidget* old, ads::CDockWidget* now) {
        Q_UNUSED(old)

        if (now) {
            ScintillaNext *editor = qobject_cast<ScintillaNext *>(now->widget());
            if (editor) {
                currentEditor = editor;
                latestDockArea = now->dockAreaWidget();
                editor->grabFocus();
                emit editorActivated(editor);
                return;
            }
        }

        // When closing a tab or dock widget, 'now' is nullptr. Safely resolve the next active editor.
        ScintillaNext *fallbackEditor = nullptr;
        for (const ads::CDockAreaWidget* areaWidget : dockManager->openedDockAreas()) {
            if (!areaWidget) continue;
            if (ads::CDockWidget* currentDock = areaWidget->currentDockWidget()) {
                fallbackEditor = qobject_cast<ScintillaNext *>(currentDock->widget());
                if (fallbackEditor) {
                    latestDockArea = const_cast<ads::CDockAreaWidget*>(areaWidget);
                    break;
                }
            }
            for (const ads::CDockWidget* dockWidget : areaWidget->dockWidgets()) {
                if (dockWidget) {
                    fallbackEditor = qobject_cast<ScintillaNext *>(dockWidget->widget());
                    if (fallbackEditor) {
                        latestDockArea = const_cast<ads::CDockAreaWidget*>(areaWidget);
                        break;
                    }
                }
            }
            if (fallbackEditor) break;
        }

        currentEditor = fallbackEditor;
        if (currentEditor) {
            currentEditor->grabFocus();
            emit editorActivated(currentEditor.data());
        }
    });

    connect(dockManager, &ads::CDockManager::dockAreaCreated, this, [=, this](ads::CDockAreaWidget* DockArea) {
        DockedEditorTitleBar *titleBar = qobject_cast<DockedEditorTitleBar *>(DockArea->titleBar());
        connect(titleBar, &DockedEditorTitleBar::doubleClicked, this, &DockedEditor::titleBarDoubleClicked);

        connect(DockArea->titleBar()->tabBar(), &ads::CDockAreaTabBar::tabMoved, this, [=, this](int from, int to) {
            Q_UNUSED(from);
            Q_UNUSED(to);

            emit editorOrderChanged();
        });

        // In theory the order changes when a new dock area is created (e.g. editor is dragged and dropped),
        // but the dockAreaCreated() signal is triggered before it is actually added to the CDockManager,
        // so interrogating the dock manager during the signal doesn't help.
        //emit editorOrderChanged();
    });
}


ScintillaNext *DockedEditor::getCurrentEditor() const
{
    if (currentEditor) {
        return currentEditor.data();
    }

    for (const ads::CDockAreaWidget* areaWidget : dockManager->openedDockAreas()) {
        if (!areaWidget) continue;
        if (ads::CDockWidget* currentDock = areaWidget->currentDockWidget()) {
            if (ScintillaNext *ed = qobject_cast<ScintillaNext *>(currentDock->widget())) {
                return ed;
            }
        }
        for (const ads::CDockWidget* dockWidget : areaWidget->dockWidgets()) {
            if (dockWidget) {
                if (ScintillaNext *ed = qobject_cast<ScintillaNext *>(dockWidget->widget())) {
                    return ed;
                }
            }
        }
    }

    return nullptr;
}

int DockedEditor::count() const
{
    int total = 0;

    for (int i = 0; i < dockManager->dockAreaCount(); ++i)
        total += dockManager->dockArea(i)->dockWidgetsCount();

    return total;
}

QVector<ScintillaNext *> DockedEditor::editors() const
{
    QVector<ScintillaNext *> editors;

    // For each area, for each widget, append it to our list
    for (const ads::CDockAreaWidget* areaWidget : dockManager->openedDockAreas()) {
        for (const ads::CDockWidget* dockWidget : areaWidget->dockWidgets()) {
            editors.append(qobject_cast<ScintillaNext *>(dockWidget->widget()));
        }
    }

    return editors;
}

void DockedEditor::switchToEditor(const ScintillaNext *editor)
{
    ads::CDockWidget *dockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget());

    if (dockWidget == Q_NULLPTR) {
        qWarning() << "Expected editor's parent to be CDockWidget";
    }
    else {
        dockWidget->raise();
    }
}

void DockedEditor::dockWidgetCloseRequested()
{
    ads::CDockWidget *dockWidget = qobject_cast<ads::CDockWidget *>(sender());
    if (!dockWidget) {
        return;
    }
    ScintillaNext *editor = qobject_cast<ScintillaNext *>(dockWidget->widget());
    if (editor) {
        emit editorCloseRequested(editor);
    }
}

ads::CDockAreaWidget *DockedEditor::currentDockArea() const
{
    if (ads::CDockWidget *focused = dockManager->focusedDockWidget()) {
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
    connect(dockWidget->tabWidget(), &QWidget::customContextMenuRequested, this, [=, this](const QPoint &pos) {
        Q_UNUSED(pos)

        emit contextMenuRequestedForEditor(editor);
    });

    dockWidget->tabWidget()->setToolTip(editor->getToolTip());

    // Set the icon
    if (editor->readOnly()) {
        dockWidget->tabWidget()->setIcon(QIcon(":/icons/readonly.png"));
    }
    else {
        dockWidget->tabWidget()->setIcon(QIcon(editor->canSaveToDisk() ? ":/icons/unsaved.png" : ":/icons/saved.png"));
        connect(editor, &ScintillaNext::savePointChanged, dockWidget, [=, this](bool dirty) {
            Q_UNUSED(dirty)
            const bool actuallyDirty = editor->canSaveToDisk();
            const QString iconPath = actuallyDirty ? ":/icons/unsaved.png" : ":/icons/saved.png";
            dockWidget->tabWidget()->setIcon(QIcon(iconPath));
        });
    }

    connect(editor, &ScintillaNext::closed, dockWidget, &ads::CDockWidget::closeDockWidget);
    connect(editor, &ScintillaNext::closed, this, [=, this]() {
        if (currentEditor == editor) {
            currentEditor = nullptr;
        }
        emit editorClosed(editor);
    });
    connect(editor, &ScintillaNext::renamed, this, [=, this]() { editorRenamed(editor); });

    connect(dockWidget, &ads::CDockWidget::closeRequested, this, &DockedEditor::dockWidgetCloseRequested);

    latestDockArea = dockManager->addDockWidget(ads::CenterDockWidgetArea, dockWidget, currentDockArea());

    emit editorAdded(editor);
}

void DockedEditor::editorRenamed(ScintillaNext *editor)
{
    Q_ASSERT(editor != Q_NULLPTR);

    ads::CDockWidget *dockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget());

    dockWidget->setWindowTitle(editor->getName());

    dockWidget->tabWidget()->setToolTip(editor->getToolTip());
}

void DockedEditor::splitToRight(ScintillaNext *editor)
{
    if (!editor) return;

    ads::CDockWidget *newDockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget());
    if (newDockWidget) {
        ads::CDockAreaWidget *currentArea = currentDockArea();
        if (currentArea) {
            latestDockArea = dockManager->addDockWidget(ads::RightDockWidgetArea, newDockWidget, currentArea);
        }
    }
}

void DockedEditor::splitToBottom(ScintillaNext *editor)
{
    if (!editor) return;

    ads::CDockWidget *newDockWidget = qobject_cast<ads::CDockWidget *>(editor->parentWidget());
    if (newDockWidget) {
        ads::CDockAreaWidget *currentArea = currentDockArea();
        if (currentArea) {
            latestDockArea = dockManager->addDockWidget(ads::BottomDockWidgetArea, newDockWidget, currentArea);
        }
    }
}
