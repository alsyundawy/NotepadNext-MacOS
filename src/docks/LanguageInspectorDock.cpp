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

#include <QtGlobal>

#ifndef QT_VERSION_CHECK
#define QT_VERSION_CHECK(major, minor, patch) (((major)<<16)|((minor)<<8)|(patch))
#endif

#include <algorithm>
#include <QFontDatabase>

#include "LanguageInspectorDock.h"
#include "ui_LanguageInspectorDock.h"

#include "MainWindow.h"
#include "LanguageStylesModel.h"

#include "SpinBoxDelegate.h"
#include "ColorPickerDelegate.h"
#include "ComboBoxDelegate.h"

#include "ScintillaNext.h"

static QSpinBox *FontSizeSpinBoxFactory()
{
    auto *editor = new QSpinBox(); // NOSONAR(cpp:S5025)
    editor->setFrame(false);
    editor->setRange(2, 24);
    editor->setAlignment(Qt::AlignHCenter);
    return editor;
}

static QSpinBox *FontWeightSpinBoxFactory()
{
    auto *editor = new QSpinBox(); // NOSONAR(cpp:S5025)
    editor->setFrame(false);
    editor->setRange(100, 2400);
    editor->setSingleStep(10);
    editor->setAlignment(Qt::AlignHCenter);
    return editor;
}

static QString property_type_to_string(int type) {
    switch (type) {
    case SC_TYPE_BOOLEAN:
        return "Bool";
    case SC_TYPE_INTEGER:
        return "Int";
    case SC_TYPE_STRING:
        return "String";
    default:
        return "unknown";
    }
}

LanguageInspectorDock::LanguageInspectorDock(MainWindow *parent) :
    QDockWidget(parent),
    ui(std::make_unique<Ui::LanguageInspectorDock>())
{
    ui->setupUi(this);

    auto *fontSizeDelegate = new SpinBoxDelegate(FontSizeSpinBoxFactory, this);
    ui->tblStyles->setItemDelegateForColumn(5, fontSizeDelegate);

    auto *fontWeightDelegate = new SpinBoxDelegate(FontWeightSpinBoxFactory, this);
    ui->tblStyles->setItemDelegateForColumn(6, fontWeightDelegate);

    auto *foreColorDelegate = new ColorPickerDelegate(this);
    ui->tblStyles->setItemDelegateForColumn(11, foreColorDelegate);

    auto *backColorDelegate = new ColorPickerDelegate(this);
    ui->tblStyles->setItemDelegateForColumn(12, backColorDelegate);

    QList<ComboBoxItem> caseItems{
        {"SC_CASE_MIXED", SC_CASE_MIXED},
        {"SC_CASE_UPPER", SC_CASE_UPPER},
        {"SC_CASE_LOWER", SC_CASE_LOWER},
        {"SC_CASE_CAMEL", SC_CASE_CAMEL}
    };
    auto *caseComoboDelegate = new ComboBoxDelegate(caseItems, this);
    ui->tblStyles->setItemDelegateForColumn(15, caseComoboDelegate);

    QList<ComboBoxItem> fontNames;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const auto families = QFontDatabase::families();
#else
    QFontDatabase fontDatabase;
    const auto families = fontDatabase.families();
#endif
    fontNames.reserve(families.size());
    std::ranges::transform(families, std::back_inserter(fontNames),
        [](const QString &font) {
            return QPair<QString, QString>(font, font);
        });
    auto *fontComboDelegate = new ComboBoxDelegate(fontNames, this);
    ui->tblStyles->setItemDelegateForColumn(4, fontComboDelegate);

    connect(this, &QDockWidget::visibilityChanged, this, [this, parent](bool visible) {
        if (visible) {
            connectToEditor(parent->currentEditor());
            connect(parent, &MainWindow::editorActivated, this, &LanguageInspectorDock::connectToEditor);
        }
        else {
            disconnectFromEditor();
            disconnect(parent, &MainWindow::editorActivated, this, &LanguageInspectorDock::connectToEditor);
        }
    });
}

LanguageInspectorDock::~LanguageInspectorDock() = default;

void LanguageInspectorDock::connectToEditor(ScintillaNext *editor)
{
    disconnectFromEditor();

    if (!editor) {
        return;
    }

    editorConnection = connect(editor, &ScintillaNext::updateUi, this, &LanguageInspectorDock::updatePositionInfo);
    documentConnection = connect(editor, &ScintillaNext::lexerChanged, this, [this, editor]() { updateLexerInfo(editor); });

    updateLexerInfo(editor);
}

void LanguageInspectorDock::disconnectFromEditor()
{
    if (editorConnection) {
        disconnect(editorConnection);
    }

    if (documentConnection) {
        disconnect(documentConnection);
    }
}

void LanguageInspectorDock::updatePositionInfo(Scintilla::Update updated)
{
    if (FlagSet(updated, Scintilla::Update::Text) || FlagSet(updated, Scintilla::Update::Selection)) {
        const auto *editor = qobject_cast<const ScintillaNext *>(sender());
        if (editor) {
            ui->lblInfo->setText(tr("Position %1 Style %2").arg(editor->currentPos()).arg(editor->styleAt(editor->currentPos())));
        }
    }
}

void LanguageInspectorDock::updateLexerInfo(ScintillaNext *editor)
{
    updateLanguageName(editor);
    updatePropertyInfo(editor);
    updateKeywordInfo(editor);
    updateStyleInfo(editor);
}

void LanguageInspectorDock::updateLanguageName(const ScintillaNext *editor)
{
    qInfo(Q_FUNC_INFO);

    ui->editLanguage->setText(editor->languageName);
    ui->editLexer->setText(editor->lexerLanguage());
}

void LanguageInspectorDock::updatePropertyInfo(ScintillaNext *editor)
{
    const QSignalBlocker blocker(ui->tblProperties);

    ui->tblProperties->clearContents();

    if (const QString propertyNames = editor->propertyNames(); propertyNames.isEmpty()) {
        ui->tblProperties->setRowCount(0);
    }
    else {
        const QStringList propertyNamesList = propertyNames.split('\n');

        ui->tblProperties->setRowCount(static_cast<int>(propertyNamesList.count()));

        int row = 0;
        for (const auto &property : propertyNamesList) {
            auto *prop = new QTableWidgetItem(property); // NOSONAR(cpp:S5025)
            auto *type = new QTableWidgetItem(property_type_to_string(static_cast<int>(editor->propertyType(property.toLatin1().constData())))); // NOSONAR(cpp:S5025)
            auto *desc = new QTableWidgetItem(QString(editor->describeProperty(property.toLatin1().constData()))); // NOSONAR(cpp:S5025)
            auto *val = new QTableWidgetItem(QString(editor->property(property.toLatin1().constData()))); // NOSONAR(cpp:S5025)

            desc->setToolTip(desc->text());

            prop->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            type->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            desc->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            val->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

            val->setTextAlignment(Qt::AlignCenter);

            ui->tblProperties->setItem(row, 0, prop);
            ui->tblProperties->setItem(row, 1, type);
            ui->tblProperties->setItem(row, 2, desc);
            ui->tblProperties->setItem(row, 3, val);
            ++row;
        }
    }

    ui->tblProperties->resizeColumnToContents(0);
    ui->tblProperties->resizeColumnToContents(1);
    ui->tblProperties->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tblProperties->resizeColumnToContents(3);

    ui->tblProperties->disconnect();
    connect(ui->tblProperties, &QTableWidget::itemChanged, this, [this, editor](const QTableWidgetItem *item) {
        const QString property = ui->tblProperties->item(item->row(), 0)->text();

        editor->setProperty(property.toLatin1().constData(), item->text().toLatin1().constData());

        editor->colourise(0, -1);
    });
}

void LanguageInspectorDock::updateKeywordInfo(ScintillaNext *editor)
{
    ui->tblKeywords->clearContents();

    if (const auto keyWordSetsDescription = QString(editor->describeKeyWordSets()); keyWordSetsDescription.isEmpty()) {
        ui->tblKeywords->setRowCount(0);
    }
    else {
        const QStringList keyWordsSets = keyWordSetsDescription.split('\n');

        ui->tblKeywords->setRowCount(static_cast<int>(keyWordsSets.count()));

        int row = 0;
        for (const auto &kwSet : keyWordsSets) {
            auto *id = new QTableWidgetItem(QString::number(row)); // NOSONAR(cpp:S5025)
            id->setTextAlignment(Qt::AlignCenter);

            ui->tblKeywords->setItem(row, 0, id);
            ui->tblKeywords->setItem(row, 1, new QTableWidgetItem(kwSet)); // NOSONAR(cpp:S5025)
            ++row;
        }
    }

    ui->tblKeywords->resizeColumnToContents(0);
}

void LanguageInspectorDock::updateStyleInfo(ScintillaNext *editor)
{
    if (auto model = ui->tblStyles->model()) {
        model->deleteLater();
    }
    ui->tblStyles->setModel(new LanguageStylesModel(editor));

    ui->tblStyles->resizeColumnToContents(0);
    ui->tblStyles->resizeColumnToContents(1);
    ui->tblStyles->resizeColumnToContents(5);
    ui->tblStyles->resizeColumnToContents(6);
    ui->tblStyles->resizeColumnToContents(7);
    ui->tblStyles->resizeColumnToContents(8);
    ui->tblStyles->resizeColumnToContents(9);
    ui->tblStyles->resizeColumnToContents(10);
    ui->tblStyles->resizeColumnToContents(11);
    ui->tblStyles->resizeColumnToContents(12);
    ui->tblStyles->resizeColumnToContents(13);
    ui->tblStyles->resizeColumnToContents(14);
    ui->tblStyles->resizeColumnToContents(15);
    ui->tblStyles->resizeColumnToContents(16);
    ui->tblStyles->resizeColumnToContents(17);
    ui->tblStyles->resizeColumnToContents(18);
}
