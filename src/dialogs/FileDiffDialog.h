#pragma once
/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef FILEDIFFDIALOG_H
#define FILEDIFFDIALOG_H

#include <QDialog>
#include <QVector>
#include "../utils/MyersDiffEngine.h"

class QComboBox;
class QLabel;
class QPushButton;
class QToolButton;
class QSplitter;
class MainWindow;
class ScintillaNext;

class FileDiffDialog : public QDialog {
    Q_OBJECT

public:
    explicit FileDiffDialog(MainWindow *parent);
    ~FileDiffDialog() override = default;

    void setInitialFiles(const QString &pathA, const QString &pathB = QString());

private slots:
    void onCompareClicked();
    void onSwapClicked();
    void onBrowseAClicked();
    void onBrowseBClicked();
    void onNextDiffClicked();
    void onPrevDiffClicked();
    void onEditorAScrolled(int value);
    void onEditorBScrolled(int value);

private:
    void setupUi();
    void setupMarkers();
    void populateCombos();
    [[nodiscard]] QString readContent(int comboIndex, const QString &customPath) const;

    MainWindow *m_mainWindow{nullptr};
    QComboBox *m_comboA{nullptr};
    QComboBox *m_comboB{nullptr};
    QPushButton *m_btnBrowseA{nullptr};
    QPushButton *m_btnBrowseB{nullptr};
    QToolButton *m_btnSwap{nullptr};
    QPushButton *m_btnCompare{nullptr};
    QLabel *m_lblSummary{nullptr};
    QPushButton *m_btnPrevDiff{nullptr};
    QPushButton *m_btnNextDiff{nullptr};

    QSplitter *m_splitter{nullptr};
    ScintillaNext *m_editorA{nullptr};
    ScintillaNext *m_editorB{nullptr};

    MyersDiffEngine::DiffResult m_currentDiff;
    QString m_customPathA;
    QString m_customPathB;
    QVector<int> m_diffLineIndices;
    int m_currentDiffIndex{-1};
    bool m_syncingScroll{false};
};

#endif  // FILEDIFFDIALOG_H
