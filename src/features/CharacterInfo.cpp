/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "CharacterInfo.h"
#ifndef UNIT_TESTING
#include "../ScintillaNext.h"
#endif

#include <QChar>
#include <QStringConverter>
#include <QMap>

static const char *getAsciiName(uint32_t cp) {
    static const char *const asciiNames[128] = {"NULL",
                                                "START OF HEADING",
                                                "START OF TEXT",
                                                "END OF TEXT",
                                                "END OF TRANSMISSION",
                                                "ENQUIRY",
                                                "ACKNOWLEDGE",
                                                "BELL",
                                                "BACKSPACE",
                                                "CHARACTER TABULATION",
                                                "LINE FEED",
                                                "LINE TABULATION",
                                                "FORM FEED",
                                                "CARRIAGE RETURN",
                                                "SHIFT OUT",
                                                "SHIFT IN",
                                                "DATA LINK ESCAPE",
                                                "DEVICE CONTROL ONE",
                                                "DEVICE CONTROL TWO",
                                                "DEVICE CONTROL THREE",
                                                "DEVICE CONTROL FOUR",
                                                "NEGATIVE ACKNOWLEDGE",
                                                "SYNCHRONOUS IDLE",
                                                "END OF TRANSMISSION BLOCK",
                                                "CANCEL",
                                                "END OF MEDIUM",
                                                "SUBSTITUTE",
                                                "ESCAPE",
                                                "INFORMATION SEPARATOR FOUR",
                                                "INFORMATION SEPARATOR THREE",
                                                "INFORMATION SEPARATOR TWO",
                                                "INFORMATION SEPARATOR ONE",
                                                "SPACE",
                                                "EXCLAMATION MARK",
                                                "QUOTATION MARK",
                                                "NUMBER SIGN",
                                                "DOLLAR SIGN",
                                                "PERCENT SIGN",
                                                "AMPERSAND",
                                                "APOSTROPHE",
                                                "LEFT PARENTHESIS",
                                                "RIGHT PARENTHESIS",
                                                "ASTERISK",
                                                "PLUS SIGN",
                                                "COMMA",
                                                "HYPHEN-MINUS",
                                                "FULL STOP",
                                                "SOLIDUS",
                                                "DIGIT ZERO",
                                                "DIGIT ONE",
                                                "DIGIT TWO",
                                                "DIGIT THREE",
                                                "DIGIT FOUR",
                                                "DIGIT FIVE",
                                                "DIGIT SIX",
                                                "DIGIT SEVEN",
                                                "DIGIT EIGHT",
                                                "DIGIT NINE",
                                                "COLON",
                                                "SEMICOLON",
                                                "LESS-THAN SIGN",
                                                "EQUALS SIGN",
                                                "GREATER-THAN SIGN",
                                                "QUESTION MARK",
                                                "COMMERCIAL AT",
                                                "LATIN CAPITAL LETTER A",
                                                "LATIN CAPITAL LETTER B",
                                                "LATIN CAPITAL LETTER C",
                                                "LATIN CAPITAL LETTER D",
                                                "LATIN CAPITAL LETTER E",
                                                "LATIN CAPITAL LETTER F",
                                                "LATIN CAPITAL LETTER G",
                                                "LATIN CAPITAL LETTER H",
                                                "LATIN CAPITAL LETTER I",
                                                "LATIN CAPITAL LETTER J",
                                                "LATIN CAPITAL LETTER K",
                                                "LATIN CAPITAL LETTER L",
                                                "LATIN CAPITAL LETTER M",
                                                "LATIN CAPITAL LETTER N",
                                                "LATIN CAPITAL LETTER O",
                                                "LATIN CAPITAL LETTER P",
                                                "LATIN CAPITAL LETTER Q",
                                                "LATIN CAPITAL LETTER R",
                                                "LATIN CAPITAL LETTER S",
                                                "LATIN CAPITAL LETTER T",
                                                "LATIN CAPITAL LETTER U",
                                                "LATIN CAPITAL LETTER V",
                                                "LATIN CAPITAL LETTER W",
                                                "LATIN CAPITAL LETTER X",
                                                "LATIN CAPITAL LETTER Y",
                                                "LATIN CAPITAL LETTER Z",
                                                "LEFT SQUARE BRACKET",
                                                "REVERSE SOLIDUS",
                                                "RIGHT SQUARE BRACKET",
                                                "CIRCUMFLEX ACCENT",
                                                "LOW LINE",
                                                "GRAVE ACCENT",
                                                "LATIN SMALL LETTER A",
                                                "LATIN SMALL LETTER B",
                                                "LATIN SMALL LETTER C",
                                                "LATIN SMALL LETTER D",
                                                "LATIN SMALL LETTER E",
                                                "LATIN SMALL LETTER F",
                                                "LATIN SMALL LETTER G",
                                                "LATIN SMALL LETTER H",
                                                "LATIN SMALL LETTER I",
                                                "LATIN SMALL LETTER J",
                                                "LATIN SMALL LETTER K",
                                                "LATIN SMALL LETTER L",
                                                "LATIN SMALL LETTER M",
                                                "LATIN SMALL LETTER N",
                                                "LATIN SMALL LETTER O",
                                                "LATIN SMALL LETTER P",
                                                "LATIN SMALL LETTER Q",
                                                "LATIN SMALL LETTER R",
                                                "LATIN SMALL LETTER S",
                                                "LATIN SMALL LETTER T",
                                                "LATIN SMALL LETTER U",
                                                "LATIN SMALL LETTER V",
                                                "LATIN SMALL LETTER W",
                                                "LATIN SMALL LETTER X",
                                                "LATIN SMALL LETTER Y",
                                                "LATIN SMALL LETTER Z",
                                                "LEFT CURLY BRACKET",
                                                "VERTICAL LINE",
                                                "RIGHT CURLY BRACKET",
                                                "TILDE",
                                                "DELETE"};

    if (cp < 128)
        return asciiNames[cp];
    return nullptr;
}

static const char *getLatin1SupplementName(uint32_t cp) {
    static const char *const latin1Names[128] = {"PADDING CHARACTER",
                                                 "HIGH OCTET PRESET",
                                                 "BREAK PERMITTED HERE",
                                                 "NO BREAK HERE",
                                                 "INDEX",
                                                 "NEXT LINE",
                                                 "START OF SELECTED AREA",
                                                 "END OF SELECTED AREA",
                                                 "CHARACTER TABULATION SET",
                                                 "CHARACTER TABULATION WITH JUSTIFICATION",
                                                 "LINE TABULATION SET",
                                                 "PARTIAL LINE FORWARD",
                                                 "PARTIAL LINE BACKWARD",
                                                 "REVERSE LINE FEED",
                                                 "SINGLE SHIFT TWO",
                                                 "SINGLE SHIFT THREE",
                                                 "DEVICE CONTROL STRING",
                                                 "PRIVATE USE ONE",
                                                 "PRIVATE USE TWO",
                                                 "SET TRANSMIT STATE",
                                                 "CANCEL CHARACTER",
                                                 "MESSAGE WAITING",
                                                 "START OF GUARDED AREA",
                                                 "END OF GUARDED AREA",
                                                 "START OF STRING",
                                                 "SINGLE GRAPHIC CHARACTER INTRODUCER",
                                                 "SINGLE CHARACTER INTRODUCER",
                                                 "CONTROL SEQUENCE INTRODUCER",
                                                 "STRING TERMINATOR",
                                                 "OPERATING SYSTEM COMMAND",
                                                 "PRIVACY MESSAGE",
                                                 "APPLICATION PROGRAM COMMAND",
                                                 "NO-BREAK SPACE",
                                                 "INVERTED EXCLAMATION MARK",
                                                 "CENT SIGN",
                                                 "POUND SIGN",
                                                 "CURRENCY SIGN",
                                                 "YEN SIGN",
                                                 "BROKEN BAR",
                                                 "SECTION SIGN",
                                                 "DIAERESIS",
                                                 "COPYRIGHT SIGN",
                                                 "FEMININE ORDINAL INDICATOR",
                                                 "LEFT-POINTING DOUBLE ANGLE QUOTATION MARK",
                                                 "NOT SIGN",
                                                 "SOFT HYPHEN",
                                                 "REGISTERED SIGN",
                                                 "MACRON",
                                                 "DEGREE SIGN",
                                                 "PLUS-MINUS SIGN",
                                                 "SUPERSCRIPT TWO",
                                                 "SUPERSCRIPT THREE",
                                                 "ACUTE ACCENT",
                                                 "MICRO SIGN",
                                                 "PILCROW SIGN",
                                                 "MIDDLE DOT",
                                                 "CEDILLA",
                                                 "SUPERSCRIPT ONE",
                                                 "MASCULINE ORDINAL INDICATOR",
                                                 "RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK",
                                                 "VULGAR FRACTION ONE QUARTER",
                                                 "VULGAR FRACTION ONE HALF",
                                                 "VULGAR FRACTION THREE QUARTERS",
                                                 "INVERTED QUESTION MARK",
                                                 "LATIN CAPITAL LETTER A WITH GRAVE",
                                                 "LATIN CAPITAL LETTER A WITH ACUTE",
                                                 "LATIN CAPITAL LETTER A WITH CIRCUMFLEX",
                                                 "LATIN CAPITAL LETTER A WITH TILDE",
                                                 "LATIN CAPITAL LETTER A WITH DIAERESIS",
                                                 "LATIN CAPITAL LETTER A WITH RING ABOVE",
                                                 "LATIN CAPITAL LETTER AE",
                                                 "LATIN CAPITAL LETTER C WITH CEDILLA",
                                                 "LATIN CAPITAL LETTER E WITH GRAVE",
                                                 "LATIN CAPITAL LETTER E WITH ACUTE",
                                                 "LATIN CAPITAL LETTER E WITH CIRCUMFLEX",
                                                 "LATIN CAPITAL LETTER E WITH DIAERESIS",
                                                 "LATIN CAPITAL LETTER I WITH GRAVE",
                                                 "LATIN CAPITAL LETTER I WITH ACUTE",
                                                 "LATIN CAPITAL LETTER I WITH CIRCUMFLEX",
                                                 "LATIN CAPITAL LETTER I WITH DIAERESIS",
                                                 "LATIN CAPITAL LETTER ETH",
                                                 "LATIN CAPITAL LETTER N WITH TILDE",
                                                 "LATIN CAPITAL LETTER O WITH GRAVE",
                                                 "LATIN CAPITAL LETTER O WITH ACUTE",
                                                 "LATIN CAPITAL LETTER O WITH CIRCUMFLEX",
                                                 "LATIN CAPITAL LETTER O WITH TILDE",
                                                 "LATIN CAPITAL LETTER O WITH DIAERESIS",
                                                 "MULTIPLICATION SIGN",
                                                 "LATIN CAPITAL LETTER O WITH STROKE",
                                                 "LATIN CAPITAL LETTER U WITH GRAVE",
                                                 "LATIN CAPITAL LETTER U WITH ACUTE",
                                                 "LATIN CAPITAL LETTER U WITH CIRCUMFLEX",
                                                 "LATIN CAPITAL LETTER U WITH DIAERESIS",
                                                 "LATIN CAPITAL LETTER Y WITH ACUTE",
                                                 "LATIN CAPITAL LETTER THORN",
                                                 "LATIN SMALL LETTER SHARP S",
                                                 "LATIN SMALL LETTER A WITH GRAVE",
                                                 "LATIN SMALL LETTER A WITH ACUTE",
                                                 "LATIN SMALL LETTER A WITH CIRCUMFLEX",
                                                 "LATIN SMALL LETTER A WITH TILDE",
                                                 "LATIN SMALL LETTER A WITH DIAERESIS",
                                                 "LATIN SMALL LETTER A WITH RING ABOVE",
                                                 "LATIN SMALL LETTER AE",
                                                 "LATIN SMALL LETTER C WITH CEDILLA",
                                                 "LATIN SMALL LETTER E WITH GRAVE",
                                                 "LATIN SMALL LETTER E WITH ACUTE",
                                                 "LATIN SMALL LETTER E WITH CIRCUMFLEX",
                                                 "LATIN SMALL LETTER E WITH DIAERESIS",
                                                 "LATIN SMALL LETTER I WITH GRAVE",
                                                 "LATIN SMALL LETTER I WITH ACUTE",
                                                 "LATIN SMALL LETTER I WITH CIRCUMFLEX",
                                                 "LATIN SMALL LETTER I WITH DIAERESIS",
                                                 "LATIN SMALL LETTER ETH",
                                                 "LATIN SMALL LETTER N WITH TILDE",
                                                 "LATIN SMALL LETTER O WITH GRAVE",
                                                 "LATIN SMALL LETTER O WITH ACUTE",
                                                 "LATIN SMALL LETTER O WITH CIRCUMFLEX",
                                                 "LATIN SMALL LETTER O WITH TILDE",
                                                 "LATIN SMALL LETTER O WITH DIAERESIS",
                                                 "DIVISION SIGN",
                                                 "LATIN SMALL LETTER O WITH STROKE",
                                                 "LATIN SMALL LETTER U WITH GRAVE",
                                                 "LATIN SMALL LETTER U WITH ACUTE",
                                                 "LATIN SMALL LETTER U WITH CIRCUMFLEX",
                                                 "LATIN SMALL LETTER U WITH DIAERESIS",
                                                 "LATIN SMALL LETTER Y WITH ACUTE",
                                                 "LATIN SMALL LETTER THORN",
                                                 "LATIN SMALL LETTER Y WITH DIAERESIS"};

    if (cp >= 0x80 && cp <= 0xFF)
        return latin1Names[cp - 0x80];
    return nullptr;
}

static QString getUnicodeBlock(uint32_t cp) {
    if (cp <= 0x007F)
        return QStringLiteral("Basic Latin");
    if (cp <= 0x00FF)
        return QStringLiteral("Latin-1 Supplement");
    if (cp <= 0x017F)
        return QStringLiteral("Latin Extended-A");
    if (cp <= 0x024F)
        return QStringLiteral("Latin Extended-B");
    if (cp <= 0x02AF)
        return QStringLiteral("IPA Extensions");
    if (cp <= 0x02FF)
        return QStringLiteral("Spacing Modifier Letters");
    if (cp <= 0x036F)
        return QStringLiteral("Combining Diacritical Marks");
    if (cp <= 0x03FF)
        return QStringLiteral("Greek and Coptic");
    if (cp <= 0x04FF)
        return QStringLiteral("Cyrillic");
    if (cp <= 0x052F)
        return QStringLiteral("Cyrillic Supplement");
    if (cp <= 0x058F)
        return QStringLiteral("Armenian");
    if (cp <= 0x05FF)
        return QStringLiteral("Hebrew");
    if (cp <= 0x06FF)
        return QStringLiteral("Arabic");
    if (cp <= 0x074F)
        return QStringLiteral("Syriac");
    if (cp <= 0x077F)
        return QStringLiteral("Arabic Supplement");
    if (cp <= 0x07BF)
        return QStringLiteral("Thaana");
    if (cp <= 0x07FF)
        return QStringLiteral("NKo");
    if (cp <= 0x08FF)
        return QStringLiteral("Arabic Extended-A");
    if (cp <= 0x097F)
        return QStringLiteral("Devanagari");
    if (cp <= 0x09FF)
        return QStringLiteral("Bengali");
    if (cp <= 0x0A7F)
        return QStringLiteral("Gurmukhi");
    if (cp <= 0x0AFF)
        return QStringLiteral("Gujarati");
    if (cp <= 0x0B7F)
        return QStringLiteral("Oriya");
    if (cp <= 0x0BFF)
        return QStringLiteral("Tamil");
    if (cp <= 0x0C7F)
        return QStringLiteral("Telugu");
    if (cp <= 0x0CFF)
        return QStringLiteral("Kannada");
    if (cp <= 0x0D7F)
        return QStringLiteral("Malayalam");
    if (cp <= 0x0E7F)
        return QStringLiteral("Thai");
    if (cp <= 0x0EFF)
        return QStringLiteral("Lao");
    if (cp <= 0x0FFF)
        return QStringLiteral("Tibetan");
    if (cp <= 0x109F)
        return QStringLiteral("Myanmar");
    if (cp <= 0x10FF)
        return QStringLiteral("Georgian");
    if (cp <= 0x11FF)
        return QStringLiteral("Hangul Jamo");
    if (cp <= 0x137F)
        return QStringLiteral("Ethiopic");
    if (cp <= 0x139F)
        return QStringLiteral("Ethiopic Supplement");
    if (cp <= 0x13FF)
        return QStringLiteral("Cherokee");
    if (cp <= 0x167F)
        return QStringLiteral("Unified Canadian Aboriginal Syllabics");
    if (cp <= 0x169F)
        return QStringLiteral("Ogham");
    if (cp <= 0x16FF)
        return QStringLiteral("Runic");
    if (cp <= 0x177F)
        return QStringLiteral("Tagalog / Hanunoo / Buhid / Tagbanwa");
    if (cp <= 0x17FF)
        return QStringLiteral("Khmer");
    if (cp <= 0x18AF)
        return QStringLiteral("Mongolian");
    if (cp <= 0x1AFF)
        return QStringLiteral("Sundanese / Balinese");
    if (cp <= 0x1B7F)
        return QStringLiteral("Balinese");
    if (cp <= 0x1C4F)
        return QStringLiteral("Lepcha / Ol Chiki");
    if (cp <= 0x1C7F)
        return QStringLiteral("Vedic Extensions");
    if (cp <= 0x1CFF)
        return QStringLiteral("Phonetic Extensions");
    if (cp <= 0x1D7F)
        return QStringLiteral("Phonetic Extensions Supplement");
    if (cp <= 0x1DBF)
        return QStringLiteral("Combining Diacritical Marks Supplement");
    if (cp <= 0x1DFF)
        return QStringLiteral("Latin Extended Additional");
    if (cp <= 0x1EFF)
        return QStringLiteral("Greek Extended");
    if (cp <= 0x206F)
        return QStringLiteral("General Punctuation");
    if (cp <= 0x209F)
        return QStringLiteral("Superscripts and Subscripts");
    if (cp <= 0x20CF)
        return QStringLiteral("Currency Symbols");
    if (cp <= 0x20FF)
        return QStringLiteral("Combining Diacritical Marks for Symbols");
    if (cp <= 0x214F)
        return QStringLiteral("Letterlike Symbols");
    if (cp <= 0x218F)
        return QStringLiteral("Number Forms");
    if (cp <= 0x21FF)
        return QStringLiteral("Arrows");
    if (cp <= 0x22FF)
        return QStringLiteral("Mathematical Operators");
    if (cp <= 0x23FF)
        return QStringLiteral("Miscellaneous Technical");
    if (cp <= 0x243F)
        return QStringLiteral("Control Pictures");
    if (cp <= 0x245F)
        return QStringLiteral("Optical Character Recognition");
    if (cp <= 0x24FF)
        return QStringLiteral("Enclosed Alphanumerics");
    if (cp <= 0x257F)
        return QStringLiteral("Box Drawing");
    if (cp <= 0x259F)
        return QStringLiteral("Block Elements");
    if (cp <= 0x25FF)
        return QStringLiteral("Geometric Shapes");
    if (cp <= 0x26FF)
        return QStringLiteral("Miscellaneous Symbols");
    if (cp <= 0x27BF)
        return QStringLiteral("Dingbats");
    if (cp <= 0x27EF)
        return QStringLiteral("Miscellaneous Mathematical Symbols-A");
    if (cp <= 0x27FF)
        return QStringLiteral("Supplemental Arrows-A");
    if (cp <= 0x28FF)
        return QStringLiteral("Braille Patterns");
    if (cp <= 0x2BFF)
        return QStringLiteral("Miscellaneous Symbols and Arrows");
    if (cp <= 0x2E7F)
        return QStringLiteral("Supplemental Punctuation");
    if (cp <= 0x2EFF)
        return QStringLiteral("CJK Radicals Supplement");
    if (cp <= 0x2FDF)
        return QStringLiteral("Kangxi Radicals");
    if (cp <= 0x303F)
        return QStringLiteral("CJK Symbols and Punctuation");
    if (cp <= 0x309F)
        return QStringLiteral("Hiragana");
    if (cp <= 0x30FF)
        return QStringLiteral("Katakana");
    if (cp <= 0x312F)
        return QStringLiteral("Bopomofo");
    if (cp <= 0x318F)
        return QStringLiteral("Hangul Compatibility Jamo");
    if (cp <= 0x319F)
        return QStringLiteral("Kanbun");
    if (cp <= 0x31BF)
        return QStringLiteral("Bopomofo Extended");
    if (cp <= 0x31EF)
        return QStringLiteral("CJK Strokes");
    if (cp <= 0x31FF)
        return QStringLiteral("Katakana Phonetic Extensions");
    if (cp <= 0x32FF)
        return QStringLiteral("Enclosed CJK Letters and Months");
    if (cp <= 0x33FF)
        return QStringLiteral("CJK Compatibility");
    if (cp <= 0x4DBF)
        return QStringLiteral("CJK Unified Ideographs Extension A");
    if (cp <= 0x4DFF)
        return QStringLiteral("Yijing Hexagram Symbols");
    if (cp <= 0x9FFF)
        return QStringLiteral("CJK Unified Ideographs");
    if (cp <= 0xA48F)
        return QStringLiteral("Yi Syllables");
    if (cp <= 0xA4CF)
        return QStringLiteral("Yi Radicals");
    if (cp <= 0xAC00)
        return QStringLiteral("Vai / Cherokee / Syloti Nagri");
    if (cp <= 0xD7AF)
        return QStringLiteral("Hangul Syllables");
    if (cp <= 0xD7FF)
        return QStringLiteral("Hangul Jamo Extended-B");
    if (cp <= 0xDFFF)
        return QStringLiteral("High/Low Surrogates");
    if (cp <= 0xF8FF)
        return QStringLiteral("Private Use Area");
    if (cp <= 0xFAFF)
        return QStringLiteral("CJK Compatibility Ideographs");
    if (cp <= 0xFB4F)
        return QStringLiteral("Alphabetic Presentation Forms");
    if (cp <= 0xFDFF)
        return QStringLiteral("Arabic Presentation Forms-A");
    if (cp <= 0xFE0F)
        return QStringLiteral("Variation Selectors");
    if (cp <= 0xFE1F)
        return QStringLiteral("Vertical Forms");
    if (cp <= 0xFE2F)
        return QStringLiteral("Combining Half Marks");
    if (cp <= 0xFE4F)
        return QStringLiteral("CJK Compatibility Forms");
    if (cp <= 0xFE6F)
        return QStringLiteral("Small Form Variants");
    if (cp <= 0xFEFF)
        return QStringLiteral("Arabic Presentation Forms-B / Specials");
    if (cp <= 0xFFEF)
        return QStringLiteral("Halfwidth and Fullwidth Forms");
    if (cp <= 0xFFFF)
        return QStringLiteral("Specials");
    if (cp <= 0x1013F)
        return QStringLiteral("Aegean Numbers / Linear B");
    if (cp <= 0x1037F)
        return QStringLiteral("Old Persian / Ugaritic");
    if (cp <= 0x104AF)
        return QStringLiteral("Deseret / Shavian");
    if (cp <= 0x1083F)
        return QStringLiteral("Imperial Aramaic / Palmyrene");
    if (cp <= 0x1091F)
        return QStringLiteral("Phoenician / Lydian");
    if (cp <= 0x1107F)
        return QStringLiteral("Brahmi");
    if (cp <= 0x123FF)
        return QStringLiteral("Cuneiform");
    if (cp <= 0x1342F)
        return QStringLiteral("Egyptian Hieroglyphs");
    if (cp <= 0x16A3F)
        return QStringLiteral("Tangut");
    if (cp <= 0x1D1FF)
        return QStringLiteral("Musical Symbols");
    if (cp <= 0x1D7FF)
        return QStringLiteral("Mathematical Alphanumeric Symbols");
    if (cp <= 0x1F02F)
        return QStringLiteral("Mahjong Tiles");
    if (cp <= 0x1F09F)
        return QStringLiteral("Domino Tiles");
    if (cp <= 0x1F0FF)
        return QStringLiteral("Playing Cards");
    if (cp <= 0x1F2FF)
        return QStringLiteral("Enclosed Ideographic Supplement");
    if (cp <= 0x1F5FF)
        return QStringLiteral("Miscellaneous Symbols and Pictographs");
    if (cp <= 0x1F64F)
        return QStringLiteral("Emoticons");
    if (cp <= 0x1F67F)
        return QStringLiteral("Ornamental Dingbats");
    if (cp <= 0x1F6FF)
        return QStringLiteral("Transport and Map Symbols");
    if (cp <= 0x1F77F)
        return QStringLiteral("Alchemical Symbols");
    if (cp <= 0x1F7FF)
        return QStringLiteral("Geometric Shapes Extended");
    if (cp <= 0x1F8FF)
        return QStringLiteral("Supplemental Arrows-C");
    if (cp <= 0x1F9FF)
        return QStringLiteral("Supplemental Symbols and Pictographs");
    if (cp <= 0x1FA6F)
        return QStringLiteral("Chess Symbols");
    if (cp <= 0x1FAFF)
        return QStringLiteral("Symbols and Pictographs Extended-A");
    if (cp <= 0x2A6DF)
        return QStringLiteral("CJK Unified Ideographs Extension B");
    if (cp <= 0x2B73F)
        return QStringLiteral("CJK Unified Ideographs Extension C");
    if (cp <= 0x2B81F)
        return QStringLiteral("CJK Unified Ideographs Extension D");
    if (cp <= 0x2CEAF)
        return QStringLiteral("CJK Unified Ideographs Extension E");
    if (cp <= 0x2EBEF)
        return QStringLiteral("CJK Unified Ideographs Extension F");
    if (cp <= 0x2F7FF)
        return QStringLiteral("CJK Compatibility Ideographs Supplement");
    if (cp <= 0x323AF)
        return QStringLiteral("CJK Unified Ideographs Extension G/H/I");
    if (cp >= 0xE0000 && cp <= 0xE007F)
        return QStringLiteral("Tags");
    if (cp >= 0xE0100 && cp <= 0xE01EF)
        return QStringLiteral("Variation Selectors Supplement");
    if (cp >= 0xF0000 && cp <= 0x10FFFF)
        return QStringLiteral("Supplementary Private Use Area");

    return QStringLiteral("Unassigned");
}

static const char *getHtmlNamedEntity(uint32_t cp) {
    switch (cp) {
        case 34:
            return "&quot;";
        case 38:
            return "&amp;";
        case 39:
            return "&apos;";
        case 60:
            return "&lt;";
        case 62:
            return "&gt;";
        case 160:
            return "&nbsp;";
        case 161:
            return "&iexcl;";
        case 162:
            return "&cent;";
        case 163:
            return "&pound;";
        case 164:
            return "&curren;";
        case 165:
            return "&yen;";
        case 166:
            return "&brvbar;";
        case 167:
            return "&sect;";
        case 168:
            return "&uml;";
        case 169:
            return "&copy;";
        case 170:
            return "&ordf;";
        case 171:
            return "&laquo;";
        case 172:
            return "&not;";
        case 173:
            return "&shy;";
        case 174:
            return "&reg;";
        case 175:
            return "&macr;";
        case 176:
            return "&deg;";
        case 177:
            return "&plusmn;";
        case 178:
            return "&sup2;";
        case 179:
            return "&sup3;";
        case 180:
            return "&acute;";
        case 181:
            return "&micro;";
        case 182:
            return "&para;";
        case 183:
            return "&middot;";
        case 184:
            return "&cedil;";
        case 185:
            return "&sup1;";
        case 186:
            return "&ordm;";
        case 187:
            return "&raquo;";
        case 188:
            return "&frac14;";
        case 189:
            return "&frac12;";
        case 190:
            return "&frac34;";
        case 191:
            return "&iquest;";
        case 215:
            return "&times;";
        case 247:
            return "&divide;";
        case 8211:
            return "&ndash;";
        case 8212:
            return "&mdash;";
        case 8216:
            return "&lsquo;";
        case 8217:
            return "&rsquo;";
        case 8218:
            return "&sbquo;";
        case 8220:
            return "&ldquo;";
        case 8221:
            return "&rdquo;";
        case 8222:
            return "&bdquo;";
        case 8224:
            return "&dagger;";
        case 8225:
            return "&Dagger;";
        case 8226:
            return "&bull;";
        case 8230:
            return "&hellip;";
        case 8240:
            return "&permil;";
        case 8242:
            return "&prime;";
        case 8243:
            return "&Prime;";
        case 8364:
            return "&euro;";
        case 8482:
            return "&trade;";
        case 8592:
            return "&larr;";
        case 8593:
            return "&uarr;";
        case 8594:
            return "&rarr;";
        case 8595:
            return "&darr;";
        case 8596:
            return "&harr;";
        case 8704:
            return "&forall;";
        case 8706:
            return "&part;";
        case 8707:
            return "&exist;";
        case 8709:
            return "&empty;";
        case 8711:
            return "&nabla;";
        case 8712:
            return "&isin;";
        case 8713:
            return "&notin;";
        case 8715:
            return "&ni;";
        case 8719:
            return "&prod;";
        case 8721:
            return "&sum;";
        case 8722:
            return "&minus;";
        case 8730:
            return "&radic;";
        case 8734:
            return "&infin;";
        case 8743:
            return "&and;";
        case 8744:
            return "&or;";
        case 8745:
            return "&cap;";
        case 8746:
            return "&cup;";
        case 8747:
            return "&int;";
        case 8756:
            return "&there4;";
        case 8764:
            return "&sim;";
        case 8773:
            return "&cong;";
        case 8776:
            return "&asymp;";
        case 8800:
            return "&ne;";
        case 8801:
            return "&equiv;";
        case 8804:
            return "&le;";
        case 8805:
            return "&ge;";
        case 8834:
            return "&sub;";
        case 8835:
            return "&sup;";
        case 8838:
            return "&sube;";
        case 8839:
            return "&supe;";
        case 8853:
            return "&oplus;";
        case 8855:
            return "&otimes;";
        case 8869:
            return "&perp;";
        case 8901:
            return "&sdot;";
        default:
            return nullptr;
    }
}

static void mapCategory(QChar::Category cat, QString &code, QString &desc) {
    switch (cat) {
        case QChar::Letter_Uppercase:
            code = "Lu";
            desc = "Letter, Uppercase";
            break;
        case QChar::Letter_Lowercase:
            code = "Ll";
            desc = "Letter, Lowercase";
            break;
        case QChar::Letter_Titlecase:
            code = "Lt";
            desc = "Letter, Titlecase";
            break;
        case QChar::Letter_Modifier:
            code = "Lm";
            desc = "Letter, Modifier";
            break;
        case QChar::Letter_Other:
            code = "Lo";
            desc = "Letter, Other";
            break;
        case QChar::Mark_NonSpacing:
            code = "Mn";
            desc = "Mark, Non-Spacing";
            break;
        case QChar::Mark_SpacingCombining:
            code = "Mc";
            desc = "Mark, Spacing Combining";
            break;
        case QChar::Mark_Enclosing:
            code = "Me";
            desc = "Mark, Enclosing";
            break;
        case QChar::Number_DecimalDigit:
            code = "Nd";
            desc = "Number, Decimal Digit";
            break;
        case QChar::Number_Letter:
            code = "Nl";
            desc = "Number, Letter";
            break;
        case QChar::Number_Other:
            code = "No";
            desc = "Number, Other";
            break;
        case QChar::Punctuation_Connector:
            code = "Pc";
            desc = "Punctuation, Connector";
            break;
        case QChar::Punctuation_Dash:
            code = "Pd";
            desc = "Punctuation, Dash";
            break;
        case QChar::Punctuation_Open:
            code = "Ps";
            desc = "Punctuation, Open / Initial";
            break;
        case QChar::Punctuation_Close:
            code = "Pe";
            desc = "Punctuation, Close / Final";
            break;
        case QChar::Punctuation_InitialQuote:
            code = "Pi";
            desc = "Punctuation, Initial Quote";
            break;
        case QChar::Punctuation_FinalQuote:
            code = "Pf";
            desc = "Punctuation, Final Quote";
            break;
        case QChar::Punctuation_Other:
            code = "Po";
            desc = "Punctuation, Other";
            break;
        case QChar::Symbol_Math:
            code = "Sm";
            desc = "Symbol, Math";
            break;
        case QChar::Symbol_Currency:
            code = "Sc";
            desc = "Symbol, Currency";
            break;
        case QChar::Symbol_Modifier:
            code = "Sk";
            desc = "Symbol, Modifier";
            break;
        case QChar::Symbol_Other:
            code = "So";
            desc = "Symbol, Other";
            break;
        case QChar::Separator_Space:
            code = "Zs";
            desc = "Separator, Space";
            break;
        case QChar::Separator_Line:
            code = "Zl";
            desc = "Separator, Line";
            break;
        case QChar::Separator_Paragraph:
            code = "Zp";
            desc = "Separator, Paragraph";
            break;
        case QChar::Other_Control:
            code = "Cc";
            desc = "Other, Control";
            break;
        case QChar::Other_Format:
            code = "Cf";
            desc = "Other, Format";
            break;
        case QChar::Other_Surrogate:
            code = "Cs";
            desc = "Other, Surrogate";
            break;
        case QChar::Other_PrivateUse:
            code = "Co";
            desc = "Other, Private Use";
            break;
        case QChar::Other_NotAssigned:
        default:
            code = "Cn";
            desc = "Other, Not Assigned";
            break;
    }
}

static QString mapScript(QChar::Script s) {
    switch (s) {
        case QChar::Script_Common:
            return QStringLiteral("Common");
        case QChar::Script_Inherited:
            return QStringLiteral("Inherited");
        case QChar::Script_Latin:
            return QStringLiteral("Latin");
        case QChar::Script_Greek:
            return QStringLiteral("Greek");
        case QChar::Script_Cyrillic:
            return QStringLiteral("Cyrillic");
        case QChar::Script_Armenian:
            return QStringLiteral("Armenian");
        case QChar::Script_Hebrew:
            return QStringLiteral("Hebrew");
        case QChar::Script_Arabic:
            return QStringLiteral("Arabic");
        case QChar::Script_Syriac:
            return QStringLiteral("Syriac");
        case QChar::Script_Thaana:
            return QStringLiteral("Thaana");
        case QChar::Script_Devanagari:
            return QStringLiteral("Devanagari");
        case QChar::Script_Bengali:
            return QStringLiteral("Bengali");
        case QChar::Script_Gurmukhi:
            return QStringLiteral("Gurmukhi");
        case QChar::Script_Gujarati:
            return QStringLiteral("Gujarati");
        case QChar::Script_Oriya:
            return QStringLiteral("Oriya");
        case QChar::Script_Tamil:
            return QStringLiteral("Tamil");
        case QChar::Script_Telugu:
            return QStringLiteral("Telugu");
        case QChar::Script_Kannada:
            return QStringLiteral("Kannada");
        case QChar::Script_Malayalam:
            return QStringLiteral("Malayalam");
        case QChar::Script_Sinhala:
            return QStringLiteral("Sinhala");
        case QChar::Script_Thai:
            return QStringLiteral("Thai");
        case QChar::Script_Lao:
            return QStringLiteral("Lao");
        case QChar::Script_Tibetan:
            return QStringLiteral("Tibetan");
        case QChar::Script_Myanmar:
            return QStringLiteral("Myanmar");
        case QChar::Script_Georgian:
            return QStringLiteral("Georgian");
        case QChar::Script_Hangul:
            return QStringLiteral("Hangul");
        case QChar::Script_Ethiopic:
            return QStringLiteral("Ethiopic");
        case QChar::Script_Cherokee:
            return QStringLiteral("Cherokee");
        case QChar::Script_Ogham:
            return QStringLiteral("Ogham");
        case QChar::Script_Runic:
            return QStringLiteral("Runic");
        case QChar::Script_Khmer:
            return QStringLiteral("Khmer");
        case QChar::Script_Mongolian:
            return QStringLiteral("Mongolian");
        case QChar::Script_Hiragana:
            return QStringLiteral("Hiragana");
        case QChar::Script_Katakana:
            return QStringLiteral("Katakana");
        case QChar::Script_Bopomofo:
            return QStringLiteral("Bopomofo");
        case QChar::Script_Han:
            return QStringLiteral("Han (CJK Ideographs)");
        case QChar::Script_Yi:
            return QStringLiteral("Yi");
        case QChar::Script_Braille:
            return QStringLiteral("Braille");
        default:
            return QStringLiteral("Unknown");
    }
}

static QString mapDirection(QChar::Direction d) {
    switch (d) {
        case QChar::DirL:
            return QStringLiteral("Left to Right (L)");
        case QChar::DirR:
            return QStringLiteral("Right to Left (R)");
        case QChar::DirEN:
            return QStringLiteral("European Number (EN)");
        case QChar::DirES:
            return QStringLiteral("European Number Separator (ES)");
        case QChar::DirET:
            return QStringLiteral("European Number Terminator (ET)");
        case QChar::DirAN:
            return QStringLiteral("Arabic Number (AN)");
        case QChar::DirCS:
            return QStringLiteral("Common Number Separator (CS)");
        case QChar::DirB:
            return QStringLiteral("Paragraph Separator (B)");
        case QChar::DirS:
            return QStringLiteral("Segment Separator (S)");
        case QChar::DirWS:
            return QStringLiteral("Whitespace (WS)");
        case QChar::DirON:
            return QStringLiteral("Other Neutral (ON)");
        case QChar::DirLRE:
            return QStringLiteral("Left-to-Right Embedding (LRE)");
        case QChar::DirLRO:
            return QStringLiteral("Left-to-Right Override (LRO)");
        case QChar::DirAL:
            return QStringLiteral("Right-to-Left Arabic (AL)");
        case QChar::DirRLE:
            return QStringLiteral("Right-to-Left Embedding (RLE)");
        case QChar::DirRLO:
            return QStringLiteral("Right-to-Left Override (RLO)");
        case QChar::DirPDF:
            return QStringLiteral("Pop Directional Format (PDF)");
        case QChar::DirNSM:
            return QStringLiteral("Non-Spacing Mark (NSM)");
        case QChar::DirBN:
            return QStringLiteral("Boundary Neutral (BN)");
        default:
            return QStringLiteral("Left to Right");
    }
}

QString CharacterInfo::formatCodePoint(uint32_t cp) {
    if (cp <= 0xFFFF) {
        return QStringLiteral("U+%1").arg(cp, 4, 16, QChar('0')).toUpper();
    }
    return QStringLiteral("U+%1").arg(cp, 5, 16, QChar('0')).toUpper();
}

CharacterInfo::Details CharacterInfo::getDetailsForCodePoint(uint32_t cp) {
    Details d;
    d.codePoint = cp;
    d.isValid = true;

    char32_t ucs[1] = {cp};
    d.glyph = QString::fromUcs4(ucs, 1);

    // Visual display for control characters
    if (cp == 0x00)
        d.displayGlyph = QStringLiteral("␀ (NUL)");
    else if (cp == 0x07)
        d.displayGlyph = QStringLiteral("␇ (BEL)");
    else if (cp == 0x08)
        d.displayGlyph = QStringLiteral("␈ (BS)");
    else if (cp == 0x09)
        d.displayGlyph = QStringLiteral("␉ (TAB)");
    else if (cp == 0x0A)
        d.displayGlyph = QStringLiteral("␊ (LF)");
    else if (cp == 0x0B)
        d.displayGlyph = QStringLiteral("␋ (VT)");
    else if (cp == 0x0C)
        d.displayGlyph = QStringLiteral("␌ (FF)");
    else if (cp == 0x0D)
        d.displayGlyph = QStringLiteral("␍ (CR)");
    else if (cp == 0x1B)
        d.displayGlyph = QStringLiteral("␛ (ESC)");
    else if (cp == 0x20)
        d.displayGlyph = QStringLiteral("␣ (Space)");
    else if (cp == 0x7F)
        d.displayGlyph = QStringLiteral("␡ (DEL)");
    else if (cp < 0x20 || (cp >= 0x80 && cp <= 0x9F))
        d.displayGlyph = QStringLiteral("␀ (Control)");
    else
        d.displayGlyph = d.glyph;

    // Unicode Block
    d.block = getUnicodeBlock(cp);

    // Unicode Name
    const char *asciiName = getAsciiName(cp);
    const char *latinName = getLatin1SupplementName(cp);

    if (asciiName) {
        d.name = QString::fromLatin1(asciiName);
    } else if (latinName) {
        d.name = QString::fromLatin1(latinName);
    } else if (cp >= 0x4E00 && cp <= 0x9FFF) {
        d.name = QStringLiteral("CJK UNIFIED IDEOGRAPH-%1").arg(cp, 4, 16, QChar('0')).toUpper();
    } else if (cp >= 0xAC00 && cp <= 0xD7AF) {
        d.name = QStringLiteral("HANGUL SYLLABLE (U+%1)").arg(cp, 4, 16, QChar('0')).toUpper();
    } else {
        d.name = QStringLiteral("%1 %2").arg(d.block, formatCodePoint(cp));
    }

    // Category
    mapCategory(QChar::category(cp), d.categoryCode, d.categoryDescription);

    // Script & Direction
    d.script = mapScript(QChar::script(cp));
    d.direction = mapDirection(QChar::direction(cp));

    // UTF-8 Representations
    d.utf8Bytes = d.glyph.toUtf8();
    QStringList u8HexList;
    QStringList u8DecList;
    for (int i = 0; i < d.utf8Bytes.size(); ++i) {
        unsigned char b = static_cast<unsigned char>(d.utf8Bytes.at(i));
        u8HexList << QStringLiteral("0x%1").arg(b, 2, 16, QChar('0')).toUpper();
        u8DecList << QString::number(b);
    }
    d.utf8Hex = u8HexList.join(QLatin1String(" "));
    d.utf8Dec = u8DecList.join(QLatin1String(", "));

    // UTF-16 Representations
    if (cp <= 0xFFFF) {
        d.utf16Hex = QStringLiteral("0x%1").arg(cp, 4, 16, QChar('0')).toUpper();
        d.utf16Dec = QString::number(cp);
    } else {
        const auto high = static_cast<uint16_t>(0xD800 + ((cp - 0x10000) >> 10));
        const auto low = static_cast<uint16_t>(0xDC00 + ((cp - 0x10000) & 0x3FF));
        d.utf16Hex = QStringLiteral("0x%1 0x%2 (Surrogate Pair)")
                         .arg(high, 4, 16, QChar('0'))
                         .arg(low, 4, 16, QChar('0'))
                         .toUpper();
        d.utf16Dec = QStringLiteral("%1, %2").arg(high).arg(low);
    }

    // UTF-32 Representation
    d.utf32Hex = QStringLiteral("0x%1").arg(cp, 8, 16, QChar('0')).toUpper();

    // Web Entities
    const char *namedEnt = getHtmlNamedEntity(cp);
    if (namedEnt)
        d.htmlNamedEntity = QString::fromLatin1(namedEnt);
    d.htmlDecEntity = QStringLiteral("&#%1;").arg(cp);
    d.htmlHexEntity = QStringLiteral("&#x%1;").arg(cp, 0, 16).toUpper();

    // Escape Sequences
    if (cp == '\n')
        d.cEscape = QStringLiteral("\\n");
    else if (cp == '\r')
        d.cEscape = QStringLiteral("\\r");
    else if (cp == '\t')
        d.cEscape = QStringLiteral("\\t");
    else if (cp == '\0')
        d.cEscape = QStringLiteral("\\0");
    else if (cp <= 0xFFFF)
        d.cEscape = QStringLiteral("\\u%1").arg(cp, 4, 16, QChar('0')).toUpper();
    else
        d.cEscape = QStringLiteral("\\U%1").arg(cp, 8, 16, QChar('0')).toUpper();

    if (cp <= 0xFFFF) {
        d.jsonEscape = QStringLiteral("\\u%1").arg(cp, 4, 16, QChar('0')).toUpper();
    } else {
        const auto high = static_cast<uint16_t>(0xD800 + ((cp - 0x10000) >> 10));
        const auto low = static_cast<uint16_t>(0xDC00 + ((cp - 0x10000) & 0x3FF));
        d.jsonEscape = QStringLiteral("\\u%1\\u%2").arg(high, 4, 16, QChar('0')).arg(low, 4, 16, QChar('0')).toUpper();
    }

    d.cssEscape = QStringLiteral("\\%1 ").arg(cp, 0, 16).toLower();
    d.urlPercent = QString::fromUtf8(d.utf8Bytes.toPercentEncoding());

    return d;
}

CharacterInfo::Details CharacterInfo::getDetailsAt(ScintillaNext *editor, int pos) {
#ifdef UNIT_TESTING
    Q_UNUSED(editor);
    Q_UNUSED(pos);
    return Details();
#else
    if (!editor)
        return Details();

    if (pos < 0) {
        if (editor->selectionStart() != editor->selectionEnd()) {
            pos = static_cast<int>(editor->selectionStart());
        } else {
            pos = static_cast<int>(editor->currentPos());
        }
    }

    if (pos >= editor->length()) {
        pos = static_cast<int>(editor->length() - 1);
    }
    if (pos < 0)
        return Details();

    // Rewind past UTF-8 continuation bytes if cursor is midway through a multi-byte sequence
    while (pos > 0 && (static_cast<unsigned char>(editor->charAt(pos)) & 0xC0) == 0x80) {
        pos--;
    }

    const unsigned char lead = static_cast<unsigned char>(editor->charAt(pos));
    int seqLen = 1;
    if ((lead & 0x80) == 0)
        seqLen = 1;
    else if ((lead & 0xE0) == 0xC0)
        seqLen = 2;
    else if ((lead & 0xF0) == 0xE0)
        seqLen = 3;
    else if ((lead & 0xF8) == 0xF0)
        seqLen = 4;

    if (pos + seqLen > editor->length()) {
        seqLen = static_cast<int>(editor->length() - pos);
    }

    const QByteArray rawBytes = editor->get_text_range(pos, pos + seqLen);
    const QString str = QString::fromUtf8(rawBytes);
    if (str.isEmpty())
        return Details();

    const auto ucs4 = str.toUcs4();
    if (ucs4.isEmpty())
        return Details();

    return getDetailsForCodePoint(static_cast<uint32_t>(ucs4.first()));
#endif
}

QString CharacterInfo::formatSummary(const Details &d) {
    if (!d.isValid)
        return QStringLiteral("No character selected");

    return QStringLiteral("%1 %2\nName: %3\nBlock: %4\nCategory: %5 (%6)\nUTF-8: %7\nUTF-16: %8")
        .arg(formatCodePoint(d.codePoint), d.displayGlyph, d.name, d.block, d.categoryCode, d.categoryDescription,
             d.utf8Hex, d.utf16Hex);
}
