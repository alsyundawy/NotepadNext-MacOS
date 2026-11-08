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

#include "CaseConverter.h"
#include "ScintillaNext.h"
#include "UndoAction.h"

#include <QCoreApplication>
#include <QStringList>
#include <algorithm>
#include <vector>

QString CaseConverter::toUpper(const QString &text) {
    return text.toUpper();
}

QString CaseConverter::toLower(const QString &text) {
    return text.toLower();
}

QString CaseConverter::toInvertCase(const QString &text) {
    QString result;
    result.reserve(text.size());
    for (const QChar &ch : text) {
        if (ch.isUpper()) {
            result.append(ch.toLower());
        } else if (ch.isLower()) {
            result.append(ch.toUpper());
        } else {
            result.append(ch);
        }
    }
    return result;
}

QString CaseConverter::toTitleCase(const QString &text) {
    QString result;
    result.reserve(text.size());
    bool inWord = false;

    for (int i = 0; i < text.size(); ++i) {
        const QChar ch = text.at(i);
        if (ch.isLetterOrNumber()) {
            if (!inWord) {
                result.append(ch.toUpper());
                inWord = true;
            } else {
                result.append(ch.toLower());
            }
        } else {
            if (ch == '\'' && inWord && (i + 1 < text.size()) && text.at(i + 1).isLetter()) {
                result.append(ch);
            } else {
                result.append(ch);
                inWord = false;
            }
        }
    }
    return result;
}

QString CaseConverter::toSentenceCase(const QString &text) {
    QString result;
    result.reserve(text.size());
    bool startOfSentence = true;

    for (const QChar &ch : text) {
        if (ch.isLetter()) {
            if (startOfSentence) {
                result.append(ch.toUpper());
                startOfSentence = false;
            } else {
                result.append(ch.toLower());
            }
        } else {
            result.append(ch);
            if (ch == '.' || ch == '!' || ch == '?' || ch == '\r' || ch == '\n') {
                startOfSentence = true;
            }
        }
    }
    return result;
}

static QStringList splitWords(const QString &text) {
    QStringList words;
    QString current;
    for (int i = 0; i < text.size(); ++i) {
        const QChar ch = text.at(i);
        if (ch.isLetterOrNumber()) {
            if (!current.isEmpty() && ch.isUpper() && i > 0 && text.at(i - 1).isLower()) {
                words.append(current);
                current.clear();
            }
            current.append(ch);
        } else {
            if (!current.isEmpty()) {
                words.append(current);
                current.clear();
            }
        }
    }
    if (!current.isEmpty()) {
        words.append(current);
    }
    return words;
}

QString CaseConverter::toSnakeCase(const QString &text) {
    const QStringList words = splitWords(text);
    if (words.isEmpty()) {
        return text;
    }
    QStringList lowerWords;
    lowerWords.reserve(words.size());
    std::ranges::transform(words, std::back_inserter(lowerWords),
                           [](const QString &w) { return w.toLower(); });
    return lowerWords.join(QLatin1Char('_'));
}

QString CaseConverter::toCamelCase(const QString &text) {
    const QStringList words = splitWords(text);
    if (words.isEmpty()) {
        return text;
    }
    QString result = words.first().toLower();
    for (int i = 1; i < words.size(); ++i) {
        const QString w = words.at(i).toLower();
        if (!w.isEmpty()) {
            result.append(w.at(0).toUpper());
            result.append(w.sliced(1));
        }
    }
    return result;
}

QString CaseConverter::toPascalCase(const QString &text) {
    const QStringList words = splitWords(text);
    if (words.isEmpty()) {
        return text;
    }
    QString result;
    for (const auto &w : words) {
        const QString low = w.toLower();
        if (!low.isEmpty()) {
            result.append(low.at(0).toUpper());
            result.append(low.sliced(1));
        }
    }
    return result;
}

QString CaseConverter::toKebabCase(const QString &text) {
    const QStringList words = splitWords(text);
    if (words.isEmpty()) {
        return text;
    }
    QStringList lowerWords;
    lowerWords.reserve(words.size());
    std::ranges::transform(words, std::back_inserter(lowerWords),
                           [](const QString &w) { return w.toLower(); });
    return lowerWords.join(QLatin1Char('-'));
}

QString CaseConverter::toConstantCase(const QString &text) {
    const QStringList words = splitWords(text);
    if (words.isEmpty()) {
        return text;
    }
    QStringList upperWords;
    upperWords.reserve(words.size());
    std::ranges::transform(words, std::back_inserter(upperWords),
                           [](const QString &w) { return w.toUpper(); });
    return upperWords.join(QLatin1Char('_'));
}

QString CaseConverter::convertText(const QString &text, CaseType caseType) {
    using enum CaseConverter::CaseType;
    switch (caseType) {
        case UpperCase:
            return toUpper(text);
        case LowerCase:
            return toLower(text);
        case InvertCase:
            return toInvertCase(text);
        case TitleCase:
            return toTitleCase(text);
        case SentenceCase:
            return toSentenceCase(text);
        case SnakeCase:
            return toSnakeCase(text);
        case CamelCase:
            return toCamelCase(text);
        case PascalCase:
            return toPascalCase(text);
        case KebabCase:
            return toKebabCase(text);
        case ConstantCase:
            return toConstantCase(text);
    }
    return text;
}

struct SelectionRange {
    int index = 0;
    sptr_t start = 0;
    sptr_t end = 0;
    bool caretAtEnd = false;
};

struct UpdatedSel {
    int index = 0;
    sptr_t newStart = 0;
    sptr_t newEnd = 0;
    bool caretAtEnd = false;
};

static void restoreSelections(ScintillaNext *editor, const std::vector<UpdatedSel> &updated, bool wasWordExpanded) {
    if (updated.empty()) {
        return;
    }

    if (wasWordExpanded || updated.size() == 1) {
        if (const auto &s = updated[0]; s.caretAtEnd) {
            editor->setSelection(s.newEnd, s.newStart);
        } else {
            editor->setSelection(s.newStart, s.newEnd);
        }
        return;
    }

    for (const auto &s : updated) {
        if (s.caretAtEnd) {
            editor->setSelectionNCaret(s.index, s.newEnd);
            editor->setSelectionNAnchor(s.index, s.newStart);
        } else {
            editor->setSelectionNCaret(s.index, s.newStart);
            editor->setSelectionNAnchor(s.index, s.newEnd);
        }
    }
}

CaseConverter::ConvertResult CaseConverter::convertEditor(ScintillaNext *editor, CaseType caseType) {
    ConvertResult res;
    if (!editor) {
        res.errorMessage = QCoreApplication::translate("CaseConverter", "No active editor.");
        return res;
    }

    if (editor->readOnly()) {
        res.errorMessage = QCoreApplication::translate("CaseConverter", "Document is read-only.");
        return res;
    }

    std::vector<SelectionRange> ranges;
    const sptr_t numSelections = editor->selections();

    for (sptr_t i = 0; i < numSelections; ++i) {
        sptr_t start = editor->selectionNStart(i);
        sptr_t end = editor->selectionNEnd(i);
        sptr_t caret = editor->selectionNCaret(i);
        sptr_t anchor = editor->selectionNAnchor(i);
        if (end > start) {
            ranges.emplace_back(static_cast<int>(i), start, end, caret >= anchor);
        }
    }

    bool wasWordExpanded = false;
    if (ranges.empty()) {
        sptr_t pos = editor->currentPos();
        sptr_t wStart = editor->wordStartPosition(pos, true);
        sptr_t wEnd = editor->wordEndPosition(pos, true);
        if (wEnd > wStart) {
            ranges.emplace_back(0, wStart, wEnd, true);
            wasWordExpanded = true;
        }
    }

    if (ranges.empty()) {
        res.errorMessage = QCoreApplication::translate("CaseConverter", "No text selected or under cursor.");
        return res;
    }

    std::ranges::sort(ranges, [](const SelectionRange &a, const SelectionRange &b) { return a.start > b.start; });

    std::vector<UpdatedSel> updated;
    updated.reserve(ranges.size());

    const UndoAction ua(editor);

    for (const auto &r : ranges) {
        const QByteArray originalBytes = editor->get_text_range(static_cast<int>(r.start), static_cast<int>(r.end));
        const QString originalStr = QString::fromUtf8(originalBytes);

        if (const QString convertedStr = convertText(originalStr, caseType); convertedStr != originalStr) {
            const QByteArray newBytes = convertedStr.toUtf8();
            editor->setTargetRange(r.start, r.end);
            editor->replaceTarget(newBytes.length(), newBytes.constData());

            const sptr_t newEnd = r.start + static_cast<sptr_t>(newBytes.length());
            updated.emplace_back(r.index, r.start, newEnd, r.caretAtEnd);
            res.charactersChanged += static_cast<int>(originalStr.length());
        } else {
            updated.emplace_back(r.index, r.start, r.end, r.caretAtEnd);
        }
        res.selectionsConverted++;
    }

    restoreSelections(editor, updated, wasWordExpanded);

    res.success = true;
    return res;
}

CaseConverter::ConvertResult CaseConverter::upperCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::UpperCase);
}

CaseConverter::ConvertResult CaseConverter::lowerCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::LowerCase);
}

CaseConverter::ConvertResult CaseConverter::invertCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::InvertCase);
}

CaseConverter::ConvertResult CaseConverter::titleCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::TitleCase);
}

CaseConverter::ConvertResult CaseConverter::sentenceCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::SentenceCase);
}

CaseConverter::ConvertResult CaseConverter::snakeCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::SnakeCase);
}

CaseConverter::ConvertResult CaseConverter::camelCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::CamelCase);
}

CaseConverter::ConvertResult CaseConverter::pascalCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::PascalCase);
}

CaseConverter::ConvertResult CaseConverter::kebabCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::KebabCase);
}

CaseConverter::ConvertResult CaseConverter::constantCase(ScintillaNext *editor) {
    return convertEditor(editor, CaseType::ConstantCase);
}
