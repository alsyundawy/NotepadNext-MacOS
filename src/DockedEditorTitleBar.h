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

#ifndef DOCKEDEDITORTITLEBAR_H
#define DOCKEDEDITORTITLEBAR_H

#include <DockAreaTitleBar.h>
#include <DockAreaTabBar.h>
#include <QToolButton>
#include <QMouseEvent>

class DockedEditorTitleBar : public ads::CDockAreaTitleBar
{
    Q_OBJECT

public:
    explicit DockedEditorTitleBar(ads::CDockAreaWidget* parent) : ads::CDockAreaTitleBar(parent) {
        m_plusButton = new QToolButton(this);
        m_plusButton->setObjectName(QStringLiteral("tabBarPlusButton"));
        m_plusButton->setText(QStringLiteral("+"));
        m_plusButton->setToolTip(tr("New Document (Cmd+N)"));
        m_plusButton->setAutoRaise(true);
        m_plusButton->setCursor(Qt::PointingHandCursor);
        m_plusButton->setFocusPolicy(Qt::NoFocus);
        m_plusButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_plusButton->setFixedSize(22, 22);
        m_plusButton->setStyleSheet(
            QStringLiteral("QToolButton#tabBarPlusButton {"
                           "  font-size: 15px;"
                           "  font-weight: bold;"
                           "  padding: 0px;"
                           "  margin: 1px 2px;"
                           "  border: 1px solid transparent;"
                           "  border-radius: 3px;"
                           "}"
                           "QToolButton#tabBarPlusButton:hover {"
                           "  background-color: rgba(128, 128, 128, 0.25);"
                           "  border-color: rgba(128, 128, 128, 0.4);"
                           "}"
                           "QToolButton#tabBarPlusButton:pressed {"
                           "  background-color: rgba(128, 128, 128, 0.4);"
                           "}"));
        connect(m_plusButton, &QToolButton::clicked, this, &DockedEditorTitleBar::plusButtonClicked);

        const int tabIndex = indexOf(tabBar());
        if (tabIndex >= 0) {
            insertWidget(tabIndex + 1, m_plusButton);
        } else {
            insertWidget(1, m_plusButton);
        }
    }

    [[nodiscard]] QToolButton *plusButton() const noexcept { return m_plusButton; }

signals:
    void doubleClicked();
    void plusButtonClicked();

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override {
        ads::CDockAreaTitleBar::mouseDoubleClickEvent(event);

        if (event->button() == Qt::LeftButton) {
            emit doubleClicked();
        }
    }

private:
    QToolButton *m_plusButton{nullptr};
};

#endif // DOCKEDEDITORTITLEBAR_H
