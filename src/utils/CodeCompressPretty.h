#pragma once
/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Adapted from Notepad4 (zufuliu/notepad4) Compress and Pretty Code features.
 * Source: zufuliu/notepad4 (src/Edit.cpp, src/Styles.cpp)
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef CODECOMPRESSPRETTY_H
#define CODECOMPRESSPRETTY_H

#include <cstdint>
#include <QString>

#ifndef UNIT_TESTING
class ScintillaNext;
#endif

class CodeCompressPretty {
public:
    enum class Language : std::uint8_t { Auto, Json, Json5, Css, JavaScript, Xml, Sql };

    struct Result {
        bool success{false};
        bool wholeDocument{false};
        Language detectedLanguage{Language::Auto};
        QString original;
        QString formatted;
        QString errorMessage;
    };

#ifndef UNIT_TESTING
    // Formats (Prettifies) selection or whole document in editor
    static Result prettify(ScintillaNext *editor, Language lang = Language::Auto);

    // Minifies (Compresses) selection or whole document in editor
    static Result compress(ScintillaNext *editor, Language lang = Language::Auto);
#endif

    // Direct string formatters
    static Result prettifyString(const QString &text, Language lang = Language::Auto);
    static Result compressString(const QString &text, Language lang = Language::Auto);

    // Language-specific helpers
    static Result prettifyJson(const QString &text, bool isJson5 = false);
    static Result compressJson(const QString &text, bool isJson5 = false);

    static Result prettifyCss(const QString &text);
    static Result compressCss(const QString &text);

    static Result prettifyJs(const QString &text);
    static Result compressJs(const QString &text);

    static Result prettifyXml(const QString &text);
    static Result compressXml(const QString &text);

    static Result prettifySql(const QString &text);

    // Detect language from text sample or editor context
    static Language detectLanguage(const QString &sample = QString());
#ifndef UNIT_TESTING
    static Language detectLanguage(const ScintillaNext *editor, const QString &sample = QString());
#endif
    static QString languageName(Language lang);
};

#endif  // CODECOMPRESSPRETTY_H
