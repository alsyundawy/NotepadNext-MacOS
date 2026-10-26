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

#include "ComboBoxDelegate.h"

#include <QComboBox>
#include <QApplication>


ComboBoxDelegate::ComboBoxDelegate(const QList<ComboBoxItem> &items, QObject *parent)
    : QStyledItemDelegate(parent), comboBoxItems(items)
{
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    auto *comboBox = new QComboBox(parent);

    for (const auto &[text, data] : comboBoxItems) {
        comboBox->addItem(text, data);
    }

    return comboBox;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QVariant value = index.model()->data(index, Qt::EditRole);
    auto *comboBox = static_cast<QComboBox*>(editor);

    comboBox->setCurrentIndex(comboBox->findData(value));
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    const auto *comboBox = static_cast<const QComboBox*>(editor);
    const QVariant value = comboBox->currentData();

    model->setData(index, value, Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)

    editor->setGeometry(option.rect);
}

QSize ComboBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    const QFontMetricsF fm(qApp->font());
    qreal width = 0.0;
    qreal height = 0.0;

    for (const auto &[text, data] : comboBoxItems) {
        const QRectF rect = fm.boundingRect(text);

        height = rect.height(); // Heights should all be the same
        width = qMax(rect.width(), width);
    }

    const QStyleOptionComboBox comobBoxStyleOptions;
    const QSize maxStringSize(static_cast<int>(width), static_cast<int>(height));
    return QApplication::style()->sizeFromContents(QStyle::CT_ComboBox, &comobBoxStyleOptions, maxStringSize, nullptr);
}
