/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "ColorPreviewCallTip.h"
#include "../ScintillaNext.h"

#include <QApplication>
#include <QColorDialog>
#include <QRegularExpression>
#include <QToolTip>
#include <QtGlobal>
#include <algorithm>

static inline int QColorToScintillaColour(const QColor &c) {
    return c.red() | (c.green() << 8) | (c.blue() << 16);
}

static inline QColor parseColor(const QString &text) {
    return QColor::fromString(text);
}

ColorPreviewCallTip::ColorPreviewCallTip(ScintillaNext *editor) : EditorDecorator(editor) {
    setObjectName(QStringLiteral("ColorPreviewCallTip"));

    if (editor) {
        editor->setMouseDwellTime(350);
    }
}

ColorPreviewCallTip::~ColorPreviewCallTip() {
    dismissPreview();
}

void ColorPreviewCallTip::notify(const Scintilla::NotificationData *pscn) {
    if (!enabled || !editor) {
        return;
    }

    if (pscn->nmhdr.code == Scintilla::Notification::UpdateUI) {
        if (editor->callTipActive() && !m_currentMatch.found) {
            dismissPreview();
        }
    } else if (pscn->nmhdr.code == Scintilla::Notification::DwellStart) {
        onDwellStart(static_cast<int>(pscn->x), static_cast<int>(pscn->y));
    } else if (pscn->nmhdr.code == Scintilla::Notification::DwellEnd) {
        onDwellEnd(static_cast<int>(pscn->x), static_cast<int>(pscn->y));
    } else if (pscn->nmhdr.code == Scintilla::Notification::CallTipClick) {
        onCallTipClick(static_cast<int>(pscn->position));
    }
}

void ColorPreviewCallTip::onDwellStart(int x, int y) {
    if (!enabled || !editor) {
        return;
    }

    if (!editor->selectionEmpty() || QApplication::mouseButtons() != Qt::NoButton) {
        return;
    }

    const int pos = static_cast<int>(editor->positionFromPoint(x, y));
    if (pos >= 0 && pos <= editor->length()) {
        showPreviewAt(pos);
    }
}

void ColorPreviewCallTip::onDwellEnd(int, int) {
    // Keep CallTip visible until cursor moves away or user dismisses
}

void ColorPreviewCallTip::onCallTipClick(int) {
    openColorDialogAndReplace();
}

void ColorPreviewCallTip::dismissPreview() {
    if (editor && editor->callTipActive()) {
        editor->callTipCancel();
    }
    QToolTip::hideText();
    m_currentMatch = ColorMatch();
}

ColorPreviewCallTip::ColorMatch ColorPreviewCallTip::findColorAt(ScintillaNext *editor, int pos) {
    ColorMatch match;
    if (!editor || pos < 0 || pos > editor->length()) {
        return match;
    }

    // 1. Check active selection if non-empty
    if (editor->selectionStart() != editor->selectionEnd()) {
        const int selStart = editor->selectionStart();
        const int selEnd = editor->selectionEnd();
        if (pos >= selStart && pos <= selEnd) {
            const QString selText = QString::fromUtf8(editor->get_text_range(selStart, selEnd)).trimmed();
            QColor c;
            if (selText.startsWith(QLatin1String("0x"), Qt::CaseInsensitive)) {
                c = parseColor(QStringLiteral("#") + selText.mid(2));
            } else {
                c = parseColor(selText);
            }
            if (c.isValid()) {
                match.found = true;
                match.startPos = selStart;
                match.endPos = selEnd;
                match.text = selText;
                match.color = c;
                if (selText.startsWith(QLatin1String("0x"), Qt::CaseInsensitive)) {
                    match.format = Hex0x;
                } else if (selText.startsWith(QLatin1String("rgba"), Qt::CaseInsensitive)) {
                    match.format = Rgba;
                } else if (selText.startsWith(QLatin1String("rgb"), Qt::CaseInsensitive)) {
                    match.format = Rgb;
                } else if (selText.startsWith(QLatin1String("hsla"), Qt::CaseInsensitive)) {
                    match.format = Hsla;
                } else if (selText.startsWith(QLatin1String("hsl"), Qt::CaseInsensitive)) {
                    match.format = Hsl;
                } else if (selText.length() == 4) {
                    match.format = Hex3;
                } else if (selText.length() == 5) {
                    match.format = Hex4;
                } else if (selText.length() == 7) {
                    match.format = Hex6;
                } else {
                    match.format = Hex8;
                }
                return match;
            }
        }
    }

    // 2. Scan line containing pos
    const int line = static_cast<int>(editor->lineFromPosition(pos));
    const int lineStart = static_cast<int>(editor->positionFromLine(line));
    const int lineEnd = static_cast<int>(editor->lineEndPosition(line));
    if (lineStart >= lineEnd) {
        return match;
    }

    const QString lineText = QString::fromUtf8(editor->get_text_range(lineStart, lineEnd));
    const int col = pos - lineStart;

    // Pattern 1: Hex colors (#RGB, #RGBA, #RRGGBB, #RRGGBBAA, 0xRRGGBB)
    static const QRegularExpression hexRegex(
        QStringLiteral("(?:#|0x)([0-9a-fA-F]{8}|[0-9a-fA-F]{6}|[0-9a-fA-F]{4}|[0-9a-fA-F]{3})\\b"));
    auto hexIt = hexRegex.globalMatch(lineText);
    while (hexIt.hasNext()) {
        auto m = hexIt.next();
        if (col >= m.capturedStart() && col <= m.capturedEnd()) {
            const QString raw = m.captured(0);
            QColor c;
            if (raw.startsWith(QLatin1String("0x"), Qt::CaseInsensitive)) {
                c = parseColor(QStringLiteral("#") + raw.mid(2));
                match.format = Hex0x;
            } else {
                c = parseColor(raw);
                if (raw.length() == 4) {
                    match.format = Hex3;
                } else if (raw.length() == 5) {
                    match.format = Hex4;
                } else if (raw.length() == 7) {
                    match.format = Hex6;
                } else {
                    match.format = Hex8;
                }
            }
            if (c.isValid()) {
                match.found = true;
                match.startPos = lineStart + static_cast<int>(m.capturedStart());
                match.endPos = lineStart + static_cast<int>(m.capturedEnd());
                match.text = raw;
                match.color = c;
                return match;
            }
        }
    }

    // Pattern 2: rgb / rgba functional syntax
    static const QRegularExpression rgbRegex(
        QStringLiteral(
            "rgba?\\s*\\(\\s*\\d{1,3}%?\\s*,\\s*\\d{1,3}%?\\s*,\\s*\\d{1,3}%?(?:\\s*,\\s*[0-9.]+%?)?\\s*\\)"),
        QRegularExpression::CaseInsensitiveOption);
    auto rgbIt = rgbRegex.globalMatch(lineText);
    while (rgbIt.hasNext()) {
        auto m = rgbIt.next();
        if (col >= m.capturedStart() && col <= m.capturedEnd()) {
            const QString raw = m.captured(0);
            const QColor c = parseColor(raw);
            if (c.isValid()) {
                match.found = true;
                match.startPos = lineStart + static_cast<int>(m.capturedStart());
                match.endPos = lineStart + static_cast<int>(m.capturedEnd());
                match.text = raw;
                match.color = c;
                match.format = raw.startsWith(QLatin1String("rgba"), Qt::CaseInsensitive) ? Rgba : Rgb;
                return match;
            }
        }
    }

    // Pattern 3: hsl / hsla functional syntax
    static const QRegularExpression hslRegex(
        QStringLiteral(
            "hsla?\\s*\\(\\s*[0-9.]+(?:deg)?\\s*,\\s*[0-9.]+%\\s*,\\s*[0-9.]+%(?:\\s*,\\s*[0-9.]+%?)?\\s*\\)"),
        QRegularExpression::CaseInsensitiveOption);
    auto hslIt = hslRegex.globalMatch(lineText);
    while (hslIt.hasNext()) {
        auto m = hslIt.next();
        if (col >= m.capturedStart() && col <= m.capturedEnd()) {
            const QString raw = m.captured(0);
            const QColor c = parseColor(raw);
            if (c.isValid()) {
                match.found = true;
                match.startPos = lineStart + static_cast<int>(m.capturedStart());
                match.endPos = lineStart + static_cast<int>(m.capturedEnd());
                match.text = raw;
                match.color = c;
                match.format = raw.startsWith(QLatin1String("hsla"), Qt::CaseInsensitive) ? Hsla : Hsl;
                return match;
            }
        }
    }

    // Pattern 4: Named CSS colors
    static const QStringList namedColors = {
        QStringLiteral("red"),     QStringLiteral("green"),  QStringLiteral("blue"),    QStringLiteral("cyan"),
        QStringLiteral("magenta"), QStringLiteral("yellow"), QStringLiteral("black"),   QStringLiteral("white"),
        QStringLiteral("gray"),    QStringLiteral("orange"), QStringLiteral("purple"),  QStringLiteral("pink"),
        QStringLiteral("brown"),   QStringLiteral("gold"),   QStringLiteral("indigo"),  QStringLiteral("teal"),
        QStringLiteral("navy"),    QStringLiteral("coral"),  QStringLiteral("crimson"), QStringLiteral("violet"),
        QStringLiteral("lime"),    QStringLiteral("aqua"),   QStringLiteral("fuchsia"), QStringLiteral("silver")};

    static const QRegularExpression wordRegex(QStringLiteral("\\b[a-zA-Z]{3,10}\\b"));
    auto wordIt = wordRegex.globalMatch(lineText);
    while (wordIt.hasNext()) {
        auto m = wordIt.next();
        if (col >= m.capturedStart() && col <= m.capturedEnd()) {
            const QString word = m.captured(0).toLower();
            if (namedColors.contains(word)) {
                const QColor c = parseColor(word);
                if (c.isValid()) {
                    match.found = true;
                    match.startPos = lineStart + static_cast<int>(m.capturedStart());
                    match.endPos = lineStart + static_cast<int>(m.capturedEnd());
                    match.text = m.captured(0);
                    match.color = c;
                    match.format = Named;
                    return match;
                }
            }
        }
    }

    return match;
}

QString ColorPreviewCallTip::formatColor(const QColor &color, ColorFormat format) {
    switch (format) {
        case Hex3: {
            const int r = qRound(color.red() / 17.0);
            const int g = qRound(color.green() / 17.0);
            const int b = qRound(color.blue() / 17.0);
            return QStringLiteral("#%1%2%3").arg(r, 1, 16).arg(g, 1, 16).arg(b, 1, 16).toUpper();
        }
        case Hex4: {
            const int r = qRound(color.red() / 17.0);
            const int g = qRound(color.green() / 17.0);
            const int b = qRound(color.blue() / 17.0);
            const int a = qRound(color.alpha() / 17.0);
            return QStringLiteral("#%1%2%3%4").arg(r, 1, 16).arg(g, 1, 16).arg(b, 1, 16).arg(a, 1, 16).toUpper();
        }
        case Hex0x:
            return QStringLiteral("0x") + color.name(QColor::HexRgb).mid(1).toUpper();
        case Hex8:
            return color.name(QColor::HexArgb).toUpper();
        case Rgb:
            return QStringLiteral("rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
        case Rgba:
            return QStringLiteral("rgba(%1, %2, %3, %4)")
                .arg(color.red())
                .arg(color.green())
                .arg(color.blue())
                .arg(color.alphaF(), 0, 'g', 2);
        case Hsl:
            return QStringLiteral("hsl(%1, %2%, %3%)")
                .arg(qMax(0, color.hslHue()))
                .arg(int(color.hslSaturationF() * 100))
                .arg(int(color.lightnessF() * 100));
        case Hsla:
            return QStringLiteral("hsla(%1, %2%, %3%, %4)")
                .arg(qMax(0, color.hslHue()))
                .arg(int(color.hslSaturationF() * 100))
                .arg(int(color.lightnessF() * 100))
                .arg(color.alphaF(), 0, 'g', 2);
        case Named:
        case Hex6:
        default:
            return color.name(QColor::HexRgb).toUpper();
    }
}

bool ColorPreviewCallTip::showPreviewAt(int pos) {
    if (!editor || !editor->selectionEmpty() || QApplication::mouseButtons() != Qt::NoButton) {
        return false;
    }

    ColorMatch match = findColorAt(editor, pos);
    if (!match.found || !match.color.isValid()) {
        return false;
    }
    m_currentMatch = match;

    const int sciCol = QColorToScintillaColour(match.color);
    editor->callTipSetBack(sciCol);
    editor->callTipSetFore(match.color.lightness() < 128 ? 0xFFFFFF : 0x000000);

    const QString tipText = QStringLiteral("■ %1 | RGB(%2, %3, %4) | Click CallTip to edit...")
                                .arg(match.text)
                                .arg(match.color.red())
                                .arg(match.color.green())
                                .arg(match.color.blue());

    editor->callTipShow(match.startPos, tipText.toUtf8().constData());

    const QPoint pt(static_cast<int>(editor->pointXFromPosition(match.startPos)),
                    static_cast<int>(editor->pointYFromPosition(match.startPos)));
    QPoint globalPt = editor->mapToGlobal(pt);
    globalPt.setY(globalPt.y() + 22);

    const QString swatchHtml = QStringLiteral(
                                   "<div style='background-color:%1; width:80px; height:24px; border:1px solid #444; "
                                   "border-radius:3px;'></div>"
                                   "<b>%1</b> (%2)<br>"
                                   "RGB: %3, %4, %5<br>"
                                   "HSL: %6&deg;, %7%, %8%<br>"
                                   "<i>Click CallTip or press Alt+C to edit</i>")
                                   .arg(match.color.name(QColor::HexRgb).toUpper())
                                   .arg(match.text)
                                   .arg(match.color.red())
                                   .arg(match.color.green())
                                   .arg(match.color.blue())
                                   .arg(qMax(0, match.color.hslHue()))
                                   .arg(int(match.color.hslSaturationF() * 100))
                                   .arg(int(match.color.lightnessF() * 100));

    QToolTip::showText(globalPt, swatchHtml, editor);
    return true;
}

bool ColorPreviewCallTip::openColorDialogAndReplace() {
    if (!m_currentMatch.found || !editor || editor->readOnly()) {
        return false;
    }

    const QColor initial = m_currentMatch.color;
    const QColor chosen = QColorDialog::getColor(initial, editor, tr("Pick New Color"), QColorDialog::ShowAlphaChannel);
    if (chosen.isValid()) {
        const QString replacement = formatColor(chosen, m_currentMatch.format);
        editor->beginUndoAction();
        editor->setTargetRange(m_currentMatch.startPos, m_currentMatch.endPos);
        const QByteArray utf8 = replacement.toUtf8();
        editor->replaceTarget(utf8.length(), utf8.constData());
        editor->setSelection(m_currentMatch.startPos + utf8.length(), m_currentMatch.startPos + utf8.length());
        editor->endUndoAction();

        dismissPreview();
        return true;
    }
    return false;
}

bool ColorPreviewCallTip::triggerColorPreviewOrPick() {
    if (!editor) {
        return false;
    }

    int pos = static_cast<int>(editor->currentPos());
    if (editor->selectionStart() != editor->selectionEnd()) {
        pos = editor->selectionStart();
    }

    if (showPreviewAt(pos)) {
        return openColorDialogAndReplace();
    }
    return false;
}
