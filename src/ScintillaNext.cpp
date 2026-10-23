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

#include "ScintillaNext.h"
#include "EncodingDetector.h"
#include "Finder.h"
#include "ScintillaCommenter.h"

#include "ByteArrayUtils.h"
#include "uchardet.h"
#include <cinttypes>

#include <QDir>
#include <QMouseEvent>
#include <QSaveFile>
#include <QTextCodec>
#include <QTextBoundaryFinder>
#include <QKeyEvent>


const int CHUNK_SIZE = 1024 * 1024 * 4; // Not sure what is best

inline const QByteArray BOM_UTF8    = QByteArray::fromHex("EFBBBF");
inline const QByteArray BOM_UTF16LE = QByteArray::fromHex("FFFE");
inline const QByteArray BOM_UTF16BE = QByteArray::fromHex("FEFF");

ScintillaNext::BomType detectBom(const QByteArray& data)
{
    using enum ScintillaNext::BomType;
    if (data.startsWith(BOM_UTF8))    return Utf8;
    if (data.startsWith(BOM_UTF16LE)) return Utf16LE;
    if (data.startsWith(BOM_UTF16BE)) return Utf16BE;

    return None;
}

QByteArray bomData(ScintillaNext::BomType bom)
{
    using enum ScintillaNext::BomType;
    switch (bom) {
    case Utf8:    return BOM_UTF8;
    case Utf16LE: return BOM_UTF16LE;
    case Utf16BE: return BOM_UTF16BE;
    case None:    return QByteArray();
    }
    return QByteArray();
}

int bomLength(ScintillaNext::BomType bom)
{
    using enum ScintillaNext::BomType;
    if (bom == Utf8) return BOM_UTF8.length();
    else if (bom == Utf16LE) return BOM_UTF16LE.length();
    else if (bom == Utf16BE) return BOM_UTF16BE.length();

    return 0;
}

static QFileDevice::FileError writeToDisk(const QByteArray &data, const QString &path, ScintillaNext::BomType bom, const QString &encoding)
{
    qInfo(Q_FUNC_INFO);

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("writeToDisk() failed to open file %s: %s", qPrintable(path), qPrintable(file.errorString()));
        return file.error();
    }

    const QByteArray encodedData = EncodingDetector::encodeFromUtf8(data, encoding, bom);
    if (file.write(encodedData) == -1) {
        qWarning("writeToDisk() failed writing to file %s: %s", qPrintable(path), qPrintable(file.errorString()));
        file.cancelWriting();
        return file.error();
    }

    if (!file.commit()) {
        qWarning("writeToDisk() failed to commit file %s: %s", qPrintable(path), qPrintable(file.errorString()));
        return file.error();
    }

    return QFileDevice::NoError;
}

static bool isNewlineCharacter(char c)
{
    return c == '\n' || c == '\r';
}

ScintillaNext::ScintillaNext(QString name, QWidget *parent) :
    ScintillaEdit(parent),
    name(name),
    indicatorResources(INDICATOR_MAX + 1)
{
    setCodePage(SC_CP_UTF8);

#ifdef Q_OS_MACOS
    send(SCI_SETFONTQUALITY, SC_EFF_QUALITY_LCD_OPTIMIZED);
    styleSetFont(STYLE_DEFAULT, "Menlo, Apple Color Emoji");
#endif

    // Defensive initialization of caret position and cursor geometry
    setCurrentPos(0);
    setAnchor(0);
    chooseCaretX();

    // Per the scintilla documentation, some parts of the range are not generally available
    indicatorResources.disableRange(0, 7);
    indicatorResources.disableRange(INDICATOR_IME, INDICATOR_IME_MAX);
    indicatorResources.disableRange(INDICATOR_HISTORY_REVERTED_TO_ORIGIN_INSERTION, INDICATOR_HISTORY_REVERTED_TO_MODIFIED_DELETION);
}

ScintillaNext::~ScintillaNext() = default;

ScintillaNext *ScintillaNext::fromFile(const QString &filePath, bool tryToCreate)
{
    QFile file(filePath);
    auto *editor = new ScintillaNext(file.fileName());

    if(tryToCreate && !file.exists()) {
        qInfo("Trying to create %s", qUtf8Printable(filePath));
        QDir d;
        if (!d.mkpath(QFileInfo(file).path())) {
            qWarning("Failed to create directory structure for %s", qUtf8Printable(filePath));
        }

        QFile f(filePath);
        if (f.open(QIODevice::WriteOnly)) {
            f.close();
        }
    }

    bool readSuccessful = editor->readFromDisk(file);

    if (!readSuccessful) {
        delete editor;
        return Q_NULLPTR;
    }

    editor->setFileInfo(filePath);

    return editor;
}

QString ScintillaNext::eolModeToString(int eolMode)
{
    if (eolMode == SC_EOL_CRLF)
        return QStringLiteral("crlf");
    else if (eolMode == SC_EOL_CR)
        return QStringLiteral("cr");
    else if (eolMode == SC_EOL_LF)
        return QStringLiteral("lf");
    else
        return QString(); // unknown
}

int ScintillaNext::stringToEolMode(const QString &eolMode)
{
    if (eolMode == QStringLiteral("crlf"))
        return SC_EOL_CRLF;
    else if (eolMode == QStringLiteral("cr"))
        return SC_EOL_CR;
    else if (eolMode == QStringLiteral("lf"))
        return SC_EOL_LF;
    else
        return -1;
}

int ScintillaNext::allocateIndicator(const QString &indicatorName)
{
    return indicatorResources.requestResource(indicatorName);
}

void ScintillaNext::goToRange(const Sci_CharacterRange &range)
{
    qInfo(Q_FUNC_INFO);

    if (isRangeValid(range)) {
        // Lines can be folded so make sure they are visible
        ensureVisible(lineFromPosition(range.cpMin));
        ensureVisible(lineFromPosition(range.cpMax));

        setSelection(range.cpMax, range.cpMin);
        scrollRange(range.cpMax, range.cpMin);
    }
}

QByteArray ScintillaNext::eolString() const
{
    const int eol = eOLMode();

    if (eol == SC_EOL_LF) return QByteArrayLiteral("\n");
    else if (eol == SC_EOL_CRLF) return QByteArrayLiteral("\r\n");
    else return QByteArrayLiteral("\r");
}

bool ScintillaNext::lineIsEmpty(int line)
{
    return (lineEndPosition(line) - positionFromLine(line)) == 0;
}

void ScintillaNext::deleteLine(int line)
{
    deleteRange(positionFromLine(line), lineLength(line));
}

void ScintillaNext::cutAllowLine()
{
    if (selectionEmpty()) {
        copyAllowLine();
        lineDelete();
    }
    else {
        cut();
    }
}

Sci_CharacterRange ScintillaNext::getContextText()
{
    if (!selectionEmpty()) {
        if (selections() == 1) {
            int start = selectionStart();
            int end = selectionEnd();

            if (end - start < 1024 &&
                lineFromPosition(start) == lineFromPosition(end)) {
                return { start, end };
            }
        }

        return { INVALID_POSITION, INVALID_POSITION };
    }

    return wordAtPosition(currentPos());
}

Sci_CharacterRange ScintillaNext::wordAtPosition(int pos)
{
    int start = wordStartPosition(pos, true);
    int end = wordEndPosition(pos, true);

    if (end > start) {
        return { start, end };
    }

    return { INVALID_POSITION, INVALID_POSITION };
}

void ScintillaNext::modifyFoldLevels(int level, int action)
{
    const int totalLines = lineCount();

    int line = 0;
    while (line < totalLines) {
        int foldFlags = foldLevel(line); // Even though its called fold level it contains several other flags
        bool isHeader = foldFlags & SC_FOLDLEVELHEADERFLAG;
        int actualLevel = (foldFlags & SC_FOLDLEVELNUMBERMASK) - SC_FOLDLEVELBASE;

        if (isHeader && actualLevel == level) {
            foldLine(line, action);
            line = lastChild(line, -1) + 1;
        }
        else {
            ++line;
        }
    }
}

void ScintillaNext::foldAllLevels(int level)
{
    modifyFoldLevels(level, SC_FOLDACTION_CONTRACT);
}

void ScintillaNext::unFoldAllLevels(int level)
{
    modifyFoldLevels(level, SC_FOLDACTION_EXPAND);
}

void ScintillaNext::deleteLeadingEmptyLines()
{
    while (lineCount() > 1 && lineIsEmpty(0)) {
        deleteLine(0);
    }
}

void ScintillaNext::deleteTrailingEmptyLines()
{
    const int docLength = length();
    int position = docLength;

    while (position > 0 && isNewlineCharacter(charAt(position - 1))) {
        position--;
    }

    deleteRange(position, docLength - position);
}

bool ScintillaNext::isSavedToDisk() const
{
    return !canSaveToDisk();
}

bool ScintillaNext::canSaveToDisk() const
{
    // The buffer can be saved if:
    // - It is marked as a temporary since as soon as it gets saved it is no longer a temporary buffer
    // - A modified file
    // - A missing file since as soon as it is saved it is no longer missing.
    return temporary ||
           (bufferType == ScintillaNext::New && modify()) ||
           (bufferType == ScintillaNext::File && modify()) ||
            (bufferType == ScintillaNext::FileMissing);
}

void ScintillaNext::setName(const QString &newName)
{
    this->name = newName;

    emit renamed();
}

bool ScintillaNext::isFile() const
{
    return bufferType == ScintillaNext::File || bufferType == ScintillaNext::FileMissing;
}

QFileInfo ScintillaNext::getFileInfo() const
{
    Q_ASSERT(isFile());

    return fileInfo;
}

QString ScintillaNext::getPath() const
{
    Q_ASSERT(isFile());

    return QDir::toNativeSeparators(fileInfo.canonicalPath());
}

QString ScintillaNext::getFilePath() const
{
    Q_ASSERT(isFile());

    return QDir::toNativeSeparators(fileInfo.canonicalFilePath());
}

void ScintillaNext::setFoldMarkers(const QString &type)
{
    QMap<QString, QList<int>> map{
        {"simple", {SC_MARK_MINUS, SC_MARK_PLUS, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY}},
        {"arrow",  {SC_MARK_ARROWDOWN, SC_MARK_ARROW, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY}},
        {"circle", {SC_MARK_CIRCLEMINUS, SC_MARK_CIRCLEPLUS, SC_MARK_VLINE, SC_MARK_LCORNERCURVE, SC_MARK_CIRCLEPLUSCONNECTED, SC_MARK_CIRCLEMINUSCONNECTED, SC_MARK_TCORNERCURVE }},
        {"box",    {SC_MARK_BOXMINUS, SC_MARK_BOXPLUS, SC_MARK_VLINE, SC_MARK_LCORNER, SC_MARK_BOXPLUSCONNECTED, SC_MARK_BOXMINUSCONNECTED, SC_MARK_TCORNER }},
    };

    if (!map.contains(type))
        return;

    const auto types = map[type];
    markerDefine(SC_MARKNUM_FOLDEROPEN, types[0]);
    markerDefine(SC_MARKNUM_FOLDER, types[1]);
    markerDefine(SC_MARKNUM_FOLDERSUB, types[2]);
    markerDefine(SC_MARKNUM_FOLDERTAIL, types[3]);
    markerDefine(SC_MARKNUM_FOLDEREND, types[4]);
    markerDefine(SC_MARKNUM_FOLDEROPENMID, types[5]);
    markerDefine(SC_MARKNUM_FOLDERMIDTAIL, types[6]);
}

void ScintillaNext::close()
{
    emit closed();

    deleteLater();
}

QFileDevice::FileError ScintillaNext::save()
{
    qInfo(Q_FUNC_INFO);

    Q_ASSERT(isFile());

    emit aboutToSave();

    const QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char *>(characterPointer()), textLength());
    const QString path = fileInfo.filePath();
    QFileDevice::FileError writeSuccessful = writeToDisk(data, path, bomType, fileEncoding);

    if (writeSuccessful == QFileDevice::NoError) {
        updateTimestamp();
        setSavePoint();

        // If this was a temporary file, make sure it is not any more
        setTemporary(false);

        emit saved();
    }

    return writeSuccessful;
}

void ScintillaNext::reload()
{
    Q_ASSERT(isFile());

    // Ensure the file still exists.
    if (!QFile::exists(fileInfo.canonicalFilePath())) {
        return;
    }

    const int line = firstVisibleLine();
    const int caret = selectionNCaret(mainSelection());
    const int anchor = selectionNAnchor(mainSelection());

    // Remove all the text
    {
        const QSignalBlocker blocker(this);
        setUndoCollection(false);
        emptyUndoBuffer();
        setText("");
        setUndoCollection(true);
    }

    // NOTE: if the read fails then the buffer will be completely empty...which probably
    // isn't a good thing, but this should be a rare occurrence.
    QFile f(fileInfo.canonicalFilePath());
    bool readSuccessful = readFromDisk(f);

    if (!readSuccessful) {
        return;
    }

    updateTimestamp();
    setSavePoint();

    // If this was a temporary file, make sure it is not any more
    if (isTemporary())
        setTemporary(false);

    scrollVertical(line, 0);
    setSelection(caret, anchor);

    emit reloaded();
}

void ScintillaNext::omitModifications()
{
    // If file modifications will be omitted just update file timestamp
    // so pop-up will be displayed only once per file modifications.
    updateTimestamp();
    setTemporary(true);

    return;
}

QFileDevice::FileError ScintillaNext::saveAs(const QString &newFilePath)
{
    bool isRenamed = bufferType == ScintillaNext::New || fileInfo.canonicalFilePath() != newFilePath;

    emit aboutToSave();

    const QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char *>(characterPointer()), textLength());
    QFileDevice::FileError saveSuccessful = writeToDisk(data, newFilePath, bomType, fileEncoding);

    if (saveSuccessful == QFileDevice::NoError) {
        setFileInfo(newFilePath);
        setSavePoint();

        // If this was a temporary file, make sure it is not any more
        setTemporary(false);

        emit saved();

        if (isRenamed) {
            emit renamed();
        }
    }

    return saveSuccessful;
}

QFileDevice::FileError ScintillaNext::saveCopyAs(const QString &filePath)
{
    const QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char *>(characterPointer()), textLength());
    return writeToDisk(data, filePath, bomType, fileEncoding);
}

bool ScintillaNext::rename(const QString &newFilePath)
{
    emit aboutToSave();

    // Write out the buffer to the new path
    if (saveCopyAs(newFilePath) == QFileDevice::NoError) {
        // Remove the old file
        const QString oldPath = fileInfo.canonicalFilePath();
        QFile::remove(oldPath);

        // Everything worked fine, so update the buffer's info
        setFileInfo(newFilePath);
        setSavePoint();

        // If this was a temporary file, make sure it is not any more
        setTemporary(false);

        emit saved();

        emit renamed();

        return true;
    }

    return false;
}

ScintillaNext::FileStateChange ScintillaNext::checkFileForStateChange()
{
    if (bufferType == BufferType::New) {
        return FileStateChange::NoChange;
    }
    else if (bufferType == BufferType::File) {
        // refresh else exists() fails to notice missing file
        fileInfo.refresh();

        if (!fileInfo.exists()) {
            bufferType = BufferType::FileMissing;

            emit savePointChanged(false);

            return FileStateChange::Deleted;
        }

        // See if the timestamp changed
        if (modifiedTime != fileTimestamp()) {
            return FileStateChange::Modified;
        }
        else {
            return FileStateChange::NoChange;
        }
    }
    else if (bufferType == BufferType::FileMissing) {
        // See if it reappeared
        fileInfo.refresh();

        if (fileInfo.exists()) {
            bufferType = BufferType::File;

            return FileStateChange::Restored;
        }
        else {
            return FileStateChange::NoChange;
        }
    }

    qInfo("type() = %d", bufferType);
    Q_ASSERT(false);

    return FileStateChange::NoChange;
}

bool ScintillaNext::moveToTrash()
{
    if (QFile::exists(fileInfo.canonicalFilePath())) {
        QFile f(fileInfo.canonicalFilePath());

        return f.moveToTrash();
    }

    return false;
}

void ScintillaNext::toggleCommentSelection()
{
    ScintillaCommenter sc(this);
    sc.toggleSelection();
}

void ScintillaNext::commentLineSelection()
{
    ScintillaCommenter sc(this);
    sc.commentSelection();
}

void ScintillaNext::uncommentLineSelection()
{
    ScintillaCommenter sc(this);
    sc.uncommentSelection();
}

void ScintillaNext::removeDuplicateLines()
{
    QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char *>(characterPointer()), textLength());
    const QByteArray delim = eolString();

    auto lines = ByteArrayUtils::split(data, delim);
    int originalLineCount = lines.length();
    ByteArrayUtils::removeDuplicates(lines);

    if (originalLineCount == lines.length()){
        return; // No lines were removed
    }

    QByteArray result = ByteArrayUtils::join(lines, delim);

    const UndoAction ua(this);
    setTargetRange(0, textLength());
    replaceTarget(result.length(), result.constData());
}

void ScintillaNext::removeConsecutiveDuplicateLines()
{
    QByteArray data = QByteArray::fromRawData(reinterpret_cast<const char *>(characterPointer()), textLength());
    const QByteArray delim = eolString();

    auto lines = ByteArrayUtils::split(data, delim);
    int originalLineCount = lines.length();
    ByteArrayUtils::removeConsecutiveDuplicates(lines);
    QByteArray result = ByteArrayUtils::join(lines, delim);

    if (originalLineCount == lines.length()){
        return; // No lines were removed
    }

    const UndoAction ua(this);
    setTargetRange(0, textLength());
    replaceTarget(result.length(), result.constData());
}

void ScintillaNext::convertTabsToSpaces()
{
    const int tabSize = std::max(1, static_cast<int>(tabWidth()));
    const QString spaceIndent(tabSize, ' ');

    bool hasSel = (selectionStart() != selectionEnd());
    Sci_Position startPos = hasSel ? selectionStart() : 0;
    Sci_Position endPos = hasSel ? selectionEnd() : length();

    if (startPos > endPos) {
        std::swap(startPos, endPos);
    }

    QByteArray docBytes = get_text_range(startPos, endPos);
    QString text = QString::fromUtf8(docBytes);
    if (!text.contains('\t')) {
        return;
    }

    text.replace('\t', spaceIndent);
    QByteArray newBytes = text.toUtf8();

    const UndoAction ua(this);
    setTargetRange(startPos, endPos);
    replaceTarget(newBytes.length(), newBytes.constData());
}

void ScintillaNext::convertSpacesToTabs()
{
    const int tabSize = std::max(1, static_cast<int>(tabWidth()));

    bool hasSel = (selectionStart() != selectionEnd());
    Sci_Position startPos = hasSel ? selectionStart() : 0;
    Sci_Position endPos = hasSel ? selectionEnd() : length();

    if (startPos > endPos) {
        std::swap(startPos, endPos);
    }

    QByteArray docBytes = get_text_range(startPos, endPos);
    QString text = QString::fromUtf8(docBytes);

    QStringList lines = text.split('\n');
    bool modified = false;
    for (QString &line : lines) {
        int leadingSpaces = 0;
        while (leadingSpaces < line.length() && line.at(leadingSpaces) == ' ') {
            leadingSpaces++;
        }
        if (leadingSpaces >= tabSize) {
            int numTabs = leadingSpaces / tabSize;
            int remSpaces = leadingSpaces % tabSize;
            line = QString('\t').repeated(numTabs) + QString(' ').repeated(remSpaces) + line.mid(leadingSpaces);
            modified = true;
        }
    }

    if (!modified) {
        return;
    }

    QString newText = lines.join('\n');
    QByteArray newBytes = newText.toUtf8();

    const UndoAction ua(this);
    setTargetRange(startPos, endPos);
    replaceTarget(newBytes.length(), newBytes.constData());
}

void ScintillaNext::keyPressEvent(QKeyEvent *keyEv)
{
    if (!keyEv) return;

    const int key = keyEv->key();
    const Qt::KeyboardModifiers mods = keyEv->modifiers();

    if ((key == Qt::Key_Backspace || key == Qt::Key_Delete) && (mods == Qt::NoModifier || mods == Qt::KeypadModifier)) {
        if (!selectionEmpty()) {
            ScintillaEdit::keyPressEvent(keyEv);
            return;
        }

        const Sci_Position pos = currentPos();
        const int line = lineFromPosition(pos);
        const Sci_Position lineStart = positionFromLine(line);
        const Sci_Position lineEnd = lineEndPosition(line);

        if (key == Qt::Key_Backspace && pos > lineStart) {
            const auto lenInLine = static_cast<int>(pos - lineStart);
            QByteArray bytes(lenInLine, '\0');
            Sci_TextRange tr{{static_cast<Sci_PositionCR>(lineStart), static_cast<Sci_PositionCR>(pos)}, bytes.data()};
            send(SCI_GETTEXTRANGE, 0, reinterpret_cast<sptr_t>(&tr));

            QString lineStr = QString::fromUtf8(bytes.constData(), lenInLine);
            QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, lineStr);
            finder.setPosition(lineStr.length());
            const int prevUtf16 = finder.toPreviousBoundary();
            if (prevUtf16 >= 0) {
                const int clusterByteLen = lineStr.mid(prevUtf16).toUtf8().length();
                if (clusterByteLen > 1) {
                    deleteRange(pos - clusterByteLen, clusterByteLen);
                    keyEv->accept();
                    return;
                }
            }
        } else if (key == Qt::Key_Delete && pos < lineEnd) {
            const auto lenToLineEnd = static_cast<int>(lineEnd - pos);
            QByteArray bytes(lenToLineEnd, '\0');
            Sci_TextRange tr{{static_cast<Sci_PositionCR>(pos), static_cast<Sci_PositionCR>(lineEnd)}, bytes.data()};
            send(SCI_GETTEXTRANGE, 0, reinterpret_cast<sptr_t>(&tr));

            QString lineStr = QString::fromUtf8(bytes.constData(), lenToLineEnd);
            QTextBoundaryFinder finder(QTextBoundaryFinder::Grapheme, lineStr);
            finder.setPosition(0);
            const int nextUtf16 = finder.toNextBoundary();
            if (nextUtf16 > 0) {
                const int clusterByteLen = lineStr.left(nextUtf16).toUtf8().length();
                if (clusterByteLen > 1) {
                    deleteRange(pos, clusterByteLen);
                    keyEv->accept();
                    return;
                }
            }
        }
    }

    ScintillaEdit::keyPressEvent(keyEv);
}

void ScintillaNext::dragEnterEvent(QDragEnterEvent *dragEv)
{
    // Ignore all drag and drop events with urls and let the main application handle it
    if (dragEv->mimeData()->hasUrls()) {
        return;
    }

    ScintillaEdit::dragEnterEvent(dragEv);
}

void ScintillaNext::dropEvent(QDropEvent *dropEv)
{
    // Ignore all drag and drop events with urls and let the main application handle it
    if (dropEv->mimeData()->hasUrls()) {
        return;
    }

    ScintillaEdit::dropEvent(dropEv);
}

bool ScintillaNext::readFromDisk(QFile &file, const QString &forcedEncoding)
{
    if (!file.exists()) {
        qWarning("Cannot read \"%s\": doesn't exist", qUtf8Printable(file.fileName()));
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("QFile::open() failed when opening \"%s\" - error code %d: %s", qUtf8Printable(file.fileName()), file.error(), qUtf8Printable(file.errorString()));
        return false;
    }

    allocate(file.size());
    setCodePage(SC_CP_UTF8);

    // Turn off undo collection and block signals during loading
    setUndoCollection(false);
    blockSignals(true);

    // Multi-stage encoding & BOM detection on initial sample
    const QByteArray initialSample = file.peek(65536);
    EncodingDetector::DetectionResult det = EncodingDetector::detect(initialSample);
    bomType = det.bomType;
    if (!forcedEncoding.isEmpty()) {
        fileEncoding = forcedEncoding;
    } else {
        fileEncoding = det.encoding;
    }

    QByteArray chunk;
    qint64 bytesRead = 0;
    bool first_read = true;

    do {
        chunk.resize(CHUNK_SIZE);
        bytesRead = file.read(chunk.data(), CHUNK_SIZE);
        chunk.resize(bytesRead);

        if (bytesRead <= 0) break;

        int offset = 0;
        if (first_read) {
            first_read = false;
            offset = det.bomLength;
        }

        const QByteArray rawChunk = (offset > 0) ? chunk.mid(offset) : chunk;
        const QByteArray utf8 = EncodingDetector::decodeToUtf8(rawChunk, fileEncoding, BomType::None);

        appendText(utf8.size(), utf8.constData());
    } while (!file.atEnd() && status() == SC_STATUS_OK);

    file.close();

    // Restore it back
    this->blockSignals(false);
    setUndoCollection(true);

    if (status() != SC_STATUS_OK) {
        qWarning("something bad happened in document->add_data() %ld", status());
        return false;
    }

    if (bytesRead == -1) {
        qWarning("Something bad happened when reading disk %d %s", file.error(), qUtf8Printable(file.errorString()));
        return false;
    }

    if (!QFileInfo(file).isWritable()) {
        qInfo("Setting file as read-only");
        setReadOnly(true);
    }

    return true;
}

QDateTime ScintillaNext::fileTimestamp()
{
    Q_ASSERT(bufferType != ScintillaNext::New);

    fileInfo.refresh();
    qInfo("%s last modified %s", qUtf8Printable(fileInfo.fileName()), qUtf8Printable(fileInfo.lastModified().toString()));
    return fileInfo.lastModified();
}

void ScintillaNext::updateTimestamp()
{
    modifiedTime = fileTimestamp();
}

void ScintillaNext::setFileInfo(const QString &filePath)
{
    fileInfo.setFile(filePath);
    fileInfo.makeAbsolute();

    Q_ASSERT(fileInfo.exists());

    name = fileInfo.fileName();
    bufferType = ScintillaNext::File;

    updateTimestamp();
}

void ScintillaNext::detachFileInfo(const QString &newName)
{
    setName(newName);

    bufferType = ScintillaNext::New;
}

void ScintillaNext::setTemporary(bool temp)
{
    temporary = temp;

    // Fake this signal
    emit savePointChanged(temporary);
}

void ScintillaNext::convertToEncoding(const QString &newEncoding, BomType newBom)
{
    fileEncoding = newEncoding;
    bomType = newBom;
    emit savePointChanged(false);
}

bool ScintillaNext::reloadWithEncoding(const QString &newEncoding)
{
    if (bufferType != BufferType::File) {
        fileEncoding = newEncoding;
        return true;
    }

    QFile file(fileInfo.filePath());
    if (!file.exists()) return false;

    clearAll();
    return readFromDisk(file, newEncoding);
}
