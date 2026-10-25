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

#include <utility>
#include <array>

#include <QApplication>
#include <QTimer>

#include "ApplicationSettings.h"

#include "EditorManager.h"
#include "ThemeManager.h"
#include "ScintillaNext.h"
#include "Scintilla.h"

// Editor decorators
#include "BraceMatch.h"
#include "HighlightedScrollBar.h"
#include "SmartHighlighter.h"
#include "SurroundSelection.h"
#include "LineNumbers.h"
#include "BetterMultiSelection.h"
#include "AutoIndentation.h"
#include "AutoCompletion.h"
#include "URLFinder.h"
#include "BookMarkDecorator.h"
#include "HTMLAutoCompleteDecorator.h"


const int MARK_HIDELINESBEGIN = 23;
const int MARK_HIDELINESEND = 22;
const int MARK_HIDELINESUNDERLINE = 21;


EditorManager::EditorManager(ApplicationSettings *settings, QObject *parent)
    : QObject(parent), settings(settings)
{
    fileWatcher = new QFileSystemWatcher(this);

    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
        // Debounce: a single external write commonly fires this signal 2+
        // times in a row. Only arm one timer per path; further signals
        // received before it fires are simply dropped.
        if (pendingFileChanges.contains(path)) {
            return;
        }

        pendingFileChanges.insert(path);

        QTimer::singleShot(250, this, [this, path]() {
            pendingFileChanges.remove(path);
            processExternalFileChange(path);
        });
    });

    connect(this, &EditorManager::editorCreated, this, [this](ScintillaNext *editor) {
        connect(editor, &ScintillaNext::closed, this, [this, editor]() {
            emit editorClosed(editor);
        });

        connect(editor, &ScintillaNext::closed, this, [this, editor]() {
            unwatchEditorFile(editor);
        });

        // Covers "Save As" and renaming a "New" buffer into an actual file,
        // in addition to a plain rename - fileInfo is already up to date
        // by the time this signal fires.
        connect(editor, &ScintillaNext::renamed, this, [this, editor]() {
            watchEditorFile(editor);
        });

        watchEditorFile(editor);
    });

    setupSettingsConnections();
}

void EditorManager::setupViewSettingsConnections()
{
    connect(settings, &ApplicationSettings::showWrapSymbolChanged, this, [this](bool b) {
        for (const auto &editor : getEditors()) {
            editor->setWrapVisualFlags(b ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
        }
    });

    connect(settings, &ApplicationSettings::showWhitespaceChanged, this, [this](bool b) {
        for (const auto &editor : getEditors()) {
            editor->setViewWS(b ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
        }
    });

    connect(settings, &ApplicationSettings::showEndOfLineChanged, this, [this](bool b) {
        for (const auto &editor : getEditors()) {
            editor->setViewEOL(b);
        }
    });

    connect(settings, &ApplicationSettings::showIndentGuideChanged, this, [this](bool b) {
        for (const auto &editor : getEditors()) {
            editor->setIndentationGuides(b ? SC_IV_LOOKBOTH : SC_IV_NONE);
        }
    });
}

void EditorManager::setupWordWrapSettingsConnections()
{
    connect(settings, &ApplicationSettings::wordWrapChanged, this, [this](bool b) {
        if (b) {
            for (const auto &editor : getEditors()) {
                editor->setWrapMode(SC_WRAP_WORD);
            }
        }
        else {
            for (const auto &editor : getEditors()) {
                const auto topLine = static_cast<int>(editor->docLineFromVisible(editor->firstVisibleLine()));
                editor->setWrapMode(SC_WRAP_NONE);
                editor->setFirstVisibleLine(topLine);
            }
        }
    });
}

void EditorManager::setupFontSettingsConnections()
{
    connect(settings, &ApplicationSettings::fontNameChanged, this, [this](const QString &fontName){
        for (const auto &editor : getEditors()) {
            for (int i = 0; i <= STYLE_MAX; ++i) {
                editor->styleSetFont(i, fontName.toUtf8().data());
            }
        }
    });

    connect(settings, &ApplicationSettings::fontSizeChanged, this, [this](int fontSize){
        for (const auto &editor : getEditors()) {
            for (int i = 0; i <= STYLE_MAX; ++i) {
                editor->styleSetSize(i, fontSize);
            }
        }
    });
}

void EditorManager::setupDecoratorSettingsConnections()
{
    connect(settings, &ApplicationSettings::urlHighlightingChanged, this, [this](bool b){
        for (const auto &editor : getEditors()) {
            auto *decorator = editor->findChild<URLFinder *>(QString(), Qt::FindDirectChildrenOnly);
            if (decorator) {
                decorator->setEnabled(b);
            }
        }
    });

    connect(settings, &ApplicationSettings::showLineNumbersChanged, this, [this](bool b){
        for (const auto &editor : getEditors()) {
            auto *decorator = editor->findChild<LineNumbers *>(QString(), Qt::FindDirectChildrenOnly);
            if (decorator) {
                decorator->setEnabled(b);
            }
        }
    });

    connect(settings, &ApplicationSettings::autoCompletionChanged, this, [this](bool b){
        for (const auto &editor : getEditors()) {
            auto *decorator = editor->findChild<AutoCompletion *>(QString(), Qt::FindDirectChildrenOnly);
            if (decorator) {
                decorator->setEnabled(b);
                if (!b && editor->autoCActive()) {
                    editor->autoCCancel();
                }
            }
        }
    });
}

void EditorManager::setupSettingsConnections()
{
    setupViewSettingsConnections();
    setupWordWrapSettingsConnections();
    setupFontSettingsConnections();
    setupDecoratorSettingsConnections();
}

ScintillaNext *EditorManager::createEditor(const QString &name)
{
    auto *editor = new ScintillaNext(name); // NOSONAR

    manageEditor(editor);

    return editor;
}

ScintillaNext *EditorManager::createEditorFromFile(const QString &filePath, bool tryToCreate)
{
    ScintillaNext *editor = ScintillaNext::fromFile(filePath, tryToCreate);

    if (editor) {
        manageEditor(editor);
    }

    return editor;
}

ScintillaNext *EditorManager::getEditorByFilePath(const QString &filePath)
{
    QFileInfo newInfo(filePath);
    newInfo.makeAbsolute();

    purgeOldEditorPointers();

    for (ScintillaNext *editor : std::as_const(editors)) {
        if (editor->isFile() && editor->getFileInfo() == newInfo) {
            return editor;
        }
    }

    return Q_NULLPTR;
}

void EditorManager::manageEditor(ScintillaNext *editor)
{
    editors.append(QPointer<ScintillaNext>(editor));

    setupEditor(editor);

    emit editorCreated(editor);
}

#ifdef Q_OS_MACOS
// Scintilla's macOS key bindings are gated on PLAT_GTK_MACOSX (see KeyMap.cxx), which is
// never defined for the Qt platform layer, so they never take effect. Scintilla also routes
// word-wise movement through SCI_META, a modifier that the Qt layer does not report at all.
// The bindings are assigned here rather than by patching the bundled Scintilla so that
// updating Scintilla cannot silently revert them.
//
// On macOS Qt reports Command as Qt::ControlModifier (i.e. SCMOD_CTRL) and Option as
// Qt::AltModifier (SCMOD_ALT).
static void setupMacOsKeys(ScintillaNext *editor)
{
    constexpr auto Cmd = static_cast<unsigned int>(SCMOD_CTRL);
    constexpr auto Opt = static_cast<unsigned int>(SCMOD_ALT);
    constexpr auto Shift = static_cast<unsigned int>(SCMOD_SHIFT);

    struct KeyBinding {
        int key;
        unsigned int modifiers;
        int message;
    };

    constexpr std::array bindings = {
        // Command + arrow keys move to the start/end of the line and of the document
        KeyBinding{SCK_LEFT,  Cmd,               SCI_VCHOME},
        KeyBinding{SCK_LEFT,  Cmd | Shift,       SCI_VCHOMEEXTEND},
        KeyBinding{SCK_RIGHT, Cmd,               SCI_LINEEND},
        KeyBinding{SCK_RIGHT, Cmd | Shift,       SCI_LINEENDEXTEND},
        KeyBinding{SCK_UP,    Cmd,               SCI_DOCUMENTSTART},
        KeyBinding{SCK_DOWN,  Cmd,               SCI_DOCUMENTEND},

        // Command + Shift + Up/Down are deliberately left alone. They are already used by
        // the Move Selected Lines Up/Down actions, and those actions receive the key press
        // before the editor does.

        // Option + arrow keys move by word
        KeyBinding{SCK_LEFT,  Opt,               SCI_WORDLEFT},
        KeyBinding{SCK_LEFT,  Opt | Shift,       SCI_WORDLEFTEXTEND},
        KeyBinding{SCK_RIGHT, Opt,               SCI_WORDRIGHT},
        KeyBinding{SCK_RIGHT, Opt | Shift,       SCI_WORDRIGHTEXTEND},

        // Option + Shift + Left/Right now select by word, so add Command + Option + Shift +
        // arrow keys for rectangular selection. Option + Shift + Up/Down are left with
        // Scintilla's default rectangular behaviour since nothing else uses them.
        KeyBinding{SCK_LEFT,  Cmd | Opt | Shift, SCI_CHARLEFTRECTEXTEND},
        KeyBinding{SCK_RIGHT, Cmd | Opt | Shift, SCI_CHARRIGHTRECTEXTEND},
        KeyBinding{SCK_UP,    Cmd | Opt | Shift, SCI_LINEUPRECTEXTEND},
        KeyBinding{SCK_DOWN,  Cmd | Opt | Shift, SCI_LINEDOWNRECTEXTEND},

        // Deleting by word/to the start of the line, and redo
        KeyBinding{SCK_BACK,  Opt,               SCI_DELWORDLEFT},
        KeyBinding{SCK_BACK,  Cmd,               SCI_DELLINELEFT},
        KeyBinding{'Z',       Cmd | Shift,       SCI_REDO},
    };

    for (const KeyBinding &binding : bindings) {
        editor->assignCmdKey(binding.key + static_cast<int>(binding.modifiers << 16U), binding.message);
    }
}
#endif

void EditorManager::setupEditor(ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    editor->clearCmdKey(SCK_INSERT);

#ifdef Q_OS_MACOS
    setupMacOsKeys(editor);
#endif

    editor->setFoldMarkers(QStringLiteral("box"));
    for (int i = SC_MARKNUM_FOLDEREND; i <= SC_MARKNUM_FOLDEROPEN; ++i) {
        editor->markerSetFore(i, 0xF3F3F3);
        editor->markerSetBack(i, 0x808080);
        editor->markerSetBackSelected(i, 0x0000FF);
    }

    editor->setScaleTechnique(SCALE_TECHNIQUE_PIXEL_ALIGNED);

    editor->setIdleStyling(SC_IDLESTYLING_TOVISIBLE);
    editor->setEndAtLastLine(false);

    editor->setMultipleSelection(true);
    editor->setAdditionalSelectionTyping(true);
    editor->setMultiPaste(SC_MULTIPASTE_EACH);
    editor->setVirtualSpaceOptions(SCVS_RECTANGULARSELECTION);

    editor->setMarginLeft(2);

    editor->setMarginWidthN(0, 30);
    editor->setMarginMaskN(1, static_cast<int>((1U << static_cast<unsigned int>(MARK_HIDELINESBEGIN)) | (1U << static_cast<unsigned int>(MARK_HIDELINESEND)) | (1U << static_cast<unsigned int>(MARK_HIDELINESUNDERLINE))));
    editor->setMarginMaskN(2, SC_MASK_FOLDERS);
    editor->setMarginWidthN(2, 14);

    editor->markerDefine(MARK_HIDELINESUNDERLINE, SC_MARK_UNDERLINE);
    editor->markerSetBack(MARK_HIDELINESUNDERLINE, 0x77CC77);

    editor->markerDefine(MARK_HIDELINESBEGIN, SC_MARK_ARROW);
    editor->markerDefine(MARK_HIDELINESEND, SC_MARK_ARROWDOWN);

    editor->setMarginSensitiveN(2, true);

    editor->setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED);
    editor->setScrollWidthTracking(true);
    editor->setScrollWidth(1);

    editor->setTabDrawMode(SCTD_STRIKEOUT);
    editor->setTabWidth(4);
    editor->setBackSpaceUnIndents(true);

    editor->setCaretLineVisible(true);
    editor->setCaretLineVisibleAlways(true);
    editor->setCaretWidth(2);

    editor->setEdgeColour(0x80FFFF);

    // https://www.scintilla.org/ScintillaDoc.html#ElementColours
    // SC_ELEMENT_SELECTION_TEXT
    // SC_ELEMENT_SELECTION_BACK
    // SC_ELEMENT_SELECTION_ADDITIONAL_TEXT
    // SC_ELEMENT_SELECTION_ADDITIONAL_BACK
    // SC_ELEMENT_SELECTION_SECONDARY_TEXT
    // SC_ELEMENT_SELECTION_SECONDARY_BACK
    // SC_ELEMENT_SELECTION_INACTIVE_TEXT
    editor->setElementColour(SC_ELEMENT_SELECTION_INACTIVE_BACK, 0xFFE0E0E0);
    // SC_ELEMENT_CARET
    // SC_ELEMENT_CARET_ADDITIONAL
    editor->setElementColour(SC_ELEMENT_CARET_LINE_BACK, 0xFFFFE8E8);
    editor->setElementColour(SC_ELEMENT_WHITE_SPACE, 0xFFD0D0D0);
    // SC_ELEMENT_WHITE_SPACE_BACK
    // SC_ELEMENT_HOT_SPOT_ACTIVE
    // SC_ELEMENT_HOT_SPOT_ACTIVE_BACK
    editor->setElementColour(SC_ELEMENT_FOLD_LINE, 0xFFA0A0A0);
    // SC_ELEMENT_HIDDEN_LINE

    editor->setWhitespaceSize(2);

    editor->setFoldMarginColour(true, 0xFFFFFF);
    editor->setFoldMarginHiColour(true, 0xE9E9E9);

    editor->setAutomaticFold(static_cast<int>(static_cast<unsigned int>(SC_AUTOMATICFOLD_SHOW) | static_cast<unsigned int>(SC_AUTOMATICFOLD_CLICK) | static_cast<unsigned int>(SC_AUTOMATICFOLD_CHANGE)));
    editor->markerEnableHighlight(true);

    editor->setCharsDefault();
    editor->setWordChars(editor->wordChars() + settings->additionalWordChars().toLatin1());

    editor->styleSetFore(STYLE_DEFAULT, 0x000000);
    editor->styleSetBack(STYLE_DEFAULT, 0xFFFFFF);
    editor->styleSetSize(STYLE_DEFAULT, settings->fontSize());
    editor->styleSetFont(STYLE_DEFAULT, settings->fontName().toUtf8().data());
    editor->styleClearAll();

    editor->styleSetFore(STYLE_LINENUMBER, 0x808080);
    editor->styleSetBack(STYLE_LINENUMBER, 0xE4E4E4);
    editor->styleSetBold(STYLE_LINENUMBER, false);

    editor->styleSetFore(STYLE_BRACELIGHT, 0x0000FF);
    editor->styleSetBack(STYLE_BRACELIGHT, 0xFFFFFF);

    editor->styleSetFore(STYLE_BRACEBAD, 0x000080);
    editor->styleSetBack(STYLE_BRACEBAD, 0xFFFFFF);

    editor->styleSetFore(STYLE_INDENTGUIDE, 0xC0C0C0);
    editor->styleSetBack(STYLE_INDENTGUIDE, 0xFFFFFF);

    ThemeManager::instance()->applyThemeToEditor(editor);

    // STYLE_CONTROLCHAR
    // STYLE_CALLTIP
    // STYLE_FOLDDISPLAYTEXT

    editor->setViewWS(settings->showWhitespace() ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    editor->setViewEOL(settings->showEndOfLine());
    editor->setWrapVisualFlags(settings->showWrapSymbol() ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
    editor->setIndentationGuides(settings->showIndentGuide() ? SC_IV_LOOKBOTH : SC_IV_NONE);
    editor->setWrapMode(settings->wordWrap() ? SC_WRAP_WORD : SC_WRAP_NONE);

    if (const int detectedEOLMode = detectEOLMode(editor); detectedEOLMode == -1) {
        if (!settings->defaultEOLMode().isEmpty()) {
            const int eol = ScintillaNext::stringToEolMode(settings->defaultEOLMode().toLower());

            if (eol != -1) {
                editor->setEOLMode(eol);
            } else {
                qWarning("Unexpected DefaultEOLMode: %s", qUtf8Printable(settings->defaultEOLMode()));
            }
        }
        // else it will just stay whatever EOL mode it was when it was created
    }
    else {
        editor->setEOLMode(detectedEOLMode);
    }

    // Decorators
    auto *s = new SmartHighlighter(editor);
    s->setEnabled(true);

    auto *h = new HighlightedScrollBarDecorator(editor);
    h->setEnabled(true);

    auto *b = new BraceMatch(editor);
    b->setEnabled(true);

    auto *l = new LineNumbers(editor);
    l->setEnabled(settings->showLineNumbers());

    auto *ss = new SurroundSelection(editor);
    ss->setEnabled(true);

    auto *bms = new BetterMultiSelection(editor);
    bms->setEnabled(true);

    auto *ai = new AutoIndentation(editor);
    ai->setEnabled(true);

    auto *ac = new AutoCompletion(editor);
    ac->setEnabled(settings->autoCompletion());

    auto *uf = new URLFinder(editor);
    uf->setEnabled(settings->urlHighlighting());

    auto *bm = new BookMarkDecorator(editor);
    bm->setEnabled(true);

    new HTMLAutoCompleteDecorator(editor);
}

void EditorManager::purgeOldEditorPointers()
{
    QMutableListIterator it(editors);

    while (it.hasNext()) {
        QPointer<ScintillaNext> pointer = it.next();
        if (pointer.isNull()) {
            it.remove();
        }
    }
}

QList<QPointer<ScintillaNext> > EditorManager::getEditors()
{
    purgeOldEditorPointers();
    return editors;
}

void EditorManager::processExternalFileChange(const QString &path)
{
    for (ScintillaNext *editor : std::as_const(editors)) {
        if (editor && editor->isFile() && editor->getFilePath() == path) {
            // Some editors/tools save by replacing the file (rename-over-write),
            // which makes the OS-level watch silently drop the path. Re-arm it
            // whenever we can, so subsequent external edits keep being detected.
            if (QFileInfo::exists(path) && !fileWatcher->files().contains(path)) {
                fileWatcher->addPath(path);
            }

            emit editorFileChangedOnDisk(editor);
        }
    }
}

void EditorManager::watchEditorFile(ScintillaNext *editor)
{
    const QString oldPath = watchedPaths.value(editor);
    const QString newPath = editor->isFile() ? editor->getFilePath() : QString();

    if (oldPath == newPath) {
        return;
    }

    if (!oldPath.isEmpty()) {
        fileWatcher->removePath(oldPath);
        watchedPaths.remove(editor);
    }

    if (!newPath.isEmpty()) {
        fileWatcher->addPath(newPath);
        watchedPaths.insert(editor, newPath);
    }
}

void EditorManager::unwatchEditorFile(ScintillaNext *editor)
{
    const QString path = watchedPaths.take(editor);

    if (!path.isEmpty()) {
        fileWatcher->removePath(path);
    }
}

int EditorManager::detectEOLMode(const ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    const int MIN_LINE_THRESHOLD = 3;
    const int MAX_BYTES_TO_CHECK = 10*1024;

    int index = 0;
    int lf = 0;
    int cr = 0;
    int crlf = 0;
    int chPrev = ' ';
    auto chNext = static_cast<int>(editor->charAt(index));

    for (int i = 0; i < qMin(MAX_BYTES_TO_CHECK, static_cast<int>(editor->length())); ++i) {
        int ch = chNext;
        chNext = static_cast<int>(editor->charAt(i + 1));

        if (ch == '\r') {
            if (chNext == '\n') {
                crlf++;
            } else {
                cr++;
            }
        }
        else if (ch == '\n' && chPrev != '\r') {
            lf++;
        }

        chPrev = ch;

        // If any meet some minimum threshold then just declare victory
        if (crlf == MIN_LINE_THRESHOLD) {
            return SC_EOL_CRLF;
        } else if (cr == MIN_LINE_THRESHOLD) {
            return SC_EOL_CR;
        } else if (lf == MIN_LINE_THRESHOLD) {
            return SC_EOL_LF;
        }
    }

    // There are either no lines or only a few, so make a best effort determination

    if (crlf > cr && crlf > lf) {
        return SC_EOL_CRLF;
    }
    else if (cr > lf) {
        return SC_EOL_CR;
    }
    else if (lf > cr) {
        return SC_EOL_LF;
    }
    else {
        return -1;
    }
}
