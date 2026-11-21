#pragma once
/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Adapted from Scintilla Change History and Notepad++ / Notepad4
 * change tracking margin concept.
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef CHANGEHISTORYDECORATOR_H
#define CHANGEHISTORYDECORATOR_H

#include "EditorDecorator.h"

class ChangeHistoryDecorator : public EditorDecorator {
    Q_OBJECT

public:
    static constexpr int MARGIN_CHANGE_HISTORY = 3;
    static constexpr int DEFAULT_MARGIN_WIDTH = 5;

    explicit ChangeHistoryDecorator(ScintillaNext *editor);
    ~ChangeHistoryDecorator() override;

    void notify(const Scintilla::NotificationData *pscn) override;

    int nextChangeAfter(int line);
    int previousChangeBefore(int line);

public slots:
    void setEnabled(bool b) override;

private:
    void setupMarkers();
};

#endif  // CHANGEHISTORYDECORATOR_H
