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

#include <QDesktopServices>
#include <QTimer>
#include <QUrl>

#include "URLFinder.h"


URLFinder::URLFinder(ScintillaNext *editor) :
    EditorDecorator(editor),
    timer(new QTimer(this))
{
    // Setup the indicator
    indicator = editor->allocateIndicator("url_finder");

    editor->indicSetStyle(indicator, INDIC_PLAIN);
    editor->indicSetFore(indicator, 0xFF0000);

    editor->indicSetHoverStyle(indicator, INDIC_DOTS);
    editor->indicSetHoverFore(indicator, 0xFF0000);

    // Resizing the window could reveal more text
    connect(editor, &ScintillaNext::resized, timer, qOverload<>(&QTimer::start));

    timer->setInterval(200);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &URLFinder::findURLs);

    connect(this, &EditorDecorator::stateChanged, this, [this, editor](bool b) {
        if (b) {
            connect(editor, &ScintillaNext::resized, timer, qOverload<>(&QTimer::start));
            connect(editor, &ScintillaNext::reloaded, timer, qOverload<>(&QTimer::start));
            findURLs();
        }
        else {
            disconnect(editor, &ScintillaNext::resized, timer, qOverload<>(&QTimer::start));
            clearURLs();
        }
    });
}

static void trimUrlDelimiterPunctuation(const ScintillaNext *ed, int startUrl, int &endUrl)
{
    if (startUrl <= 0) {
        return;
    }
    const auto prevChar = static_cast<int>(ed->charAt(startUrl - 1));
    const auto nextChar = static_cast<int>(ed->charAt(endUrl - 1));

    if ((prevChar == '(' && nextChar == ')') ||
        (prevChar == '[' && nextChar == ']') ||
        (prevChar == '<' && nextChar == '>') ||
        (prevChar == '"' && nextChar == '"')) {
        endUrl--;
    }
}

void URLFinder::findURLs()
{
    clearURLs();

    int currentLine = static_cast<int>(editor->docLineFromVisible(editor->firstVisibleLine()));
    int linesLeftToProcess = static_cast<int>(editor->linesOnScreen());
    const int flags = SCFIND_REGEXP;

    while(linesLeftToProcess >= 0 && currentLine < editor->lineCount()) {
        // Should only happen if the line is hidden
        if (!editor->lineVisible(currentLine)) {
            currentLine++;
            continue;
        }

        const int startPos = static_cast<int>(editor->positionFromLine(currentLine));
        const int endPos = static_cast<int>(editor->lineEndPosition(currentLine));
        const auto reg = QByteArrayLiteral(R"(\bhttps?://[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b(?:[-a-zA-Z0-9()@:%_\+.~#?&\/=]*))");

        Sci_TextToFind ttf {{static_cast<Sci_PositionCR>(startPos), static_cast<Sci_PositionCR>(endPos)}, reg.constData(), {-1, -1}};
        while (editor->send(SCI_FINDTEXT, flags, (sptr_t)&ttf) != -1) {
            const int startUrl = static_cast<int>(ttf.chrgText.cpMin);
            int endUrl = static_cast<int>(ttf.chrgText.cpMax);

            trimUrlDelimiterPunctuation(editor, startUrl, endUrl);

            editor->indicatorFillRange(startUrl, endUrl - startUrl);
            ttf.chrg.cpMin = endUrl;
        }

        // If a line is wrapped, skip however many lines it takes up on the screen
        linesLeftToProcess -= static_cast<int>(editor->wrapCount(currentLine));

        // If the current line is a fold header and the fold is not expanded, skip
        if (((editor->foldLevel(currentLine) & SC_FOLDLEVELHEADERFLAG) == SC_FOLDLEVELHEADERFLAG) && (!editor->foldExpanded(currentLine))) {
            currentLine = static_cast<int>(editor->lastChild(currentLine, -1)) + 1;
        }
        else {
            currentLine++;
        }
    }
}

void URLFinder::clearURLs()
{
    editor->setIndicatorCurrent(indicator);
    editor->indicatorClearRange(0, editor->length());
}

void URLFinder::notify(const Scintilla::NotificationData *pscn)
{
    // Note: folding/unfolding changes are captured via Scintilla update notifications below.

    // Reprocess any time the content was updated or the editor was vertically scrolled
    if (pscn->nmhdr.code == Scintilla::Notification::UpdateUI) {
        if (FlagSet(pscn->updated, Scintilla::Update::VScroll)) {
            timer->start();
        }
    }
    else if (pscn->nmhdr.code == Scintilla::Notification::Modified) {
        if (FlagSet(pscn->modificationType, Scintilla::ModificationFlags::InsertText) || FlagSet(pscn->modificationType, Scintilla::ModificationFlags::DeleteText)) {
            timer->start();
        }
    }
    else if (pscn->nmhdr.code == Scintilla::Notification::Zoom) {
        timer->start();
    }
    else if (pscn->nmhdr.code == Scintilla::Notification::IndicatorClick && FlagSet(pscn->modifiers, Scintilla::KeyMod::Ctrl)) {
        const int indicators = static_cast<int>(editor->indicatorAllOnFor(pscn->position));

        if (indicators & (1 << indicator)) {

            const int indicatorStart = static_cast<int>(editor->indicatorStart(indicator, pscn->position));
            const int indicatorEnd = static_cast<int>(editor->indicatorEnd(indicator, pscn->position));

            QUrl url(editor->get_text_range(indicatorStart, indicatorEnd));

            if (url.isValid() && (url.scheme() == QStringLiteral("http") || url.scheme() == QStringLiteral("https"))) {
                qInfo("URL hotspot click: \"%s\"", editor->get_text_range(indicatorStart, indicatorEnd).constData());
                QDesktopServices::openUrl(url);
            }
            else {
                qWarning("Invalid url \"%s\"", qUtf8Printable(url.errorString()));
            }
        }
    }
}

bool URLFinder::isURL(int position) const
{
    const int indicators = static_cast<int>(editor->indicatorAllOnFor(position));
    return indicators & (1 << indicator);
}

void URLFinder::copyURLToClipboard(int position) const
{
    const int indicatorStart = static_cast<int>(editor->indicatorStart(indicator, position));
    const int indicatorEnd = static_cast<int>(editor->indicatorEnd(indicator, position));

    QUrl url(editor->get_text_range(indicatorStart, indicatorEnd));

    if (url.isValid()) {
        qInfo("Copy URL to clipboard: \"%s\"", editor->get_text_range(indicatorStart, indicatorEnd).constData());
        editor->copyRange(indicatorStart, indicatorEnd);
    }
    else {
        qWarning("Invalid url \"%s\"", qUtf8Printable(url.errorString()));
    }
}
