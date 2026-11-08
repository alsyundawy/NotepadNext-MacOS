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
#include "ScintillaNext.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>

#include <QChar>
#include <QStringConverter>
#include <QMap>

static const char *getAsciiName(uint32_t cp) {
    static constexpr std::array<const char *, 128> asciiNames = {
"NULL",
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
                                                "DELETE"
};

    if (cp < asciiNames.size())
        return asciiNames[cp];
    return nullptr;
}

static const char *getLatin1SupplementName(uint32_t cp) {
    static constexpr std::array<const char *, 128> latin1Names = {
"PADDING CHARACTER",
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
                                                 "LATIN SMALL LETTER Y WITH DIAERESIS"
};

    if (cp >= 0x80 && cp <= 0xFF)
        return latin1Names[cp - 0x80];
    return nullptr;
}

struct UnicodeBlockEntry {
    uint32_t endCp{0};
    const char *name{nullptr};
};

static constexpr std::array<UnicodeBlockEntry, 139> unicodeBlocks = {{

    {0x007F, "Basic Latin"},
    {0x00FF, "Latin-1 Supplement"},
    {0x017F, "Latin Extended-A"},
    {0x024F, "Latin Extended-B"},
    {0x02AF, "IPA Extensions"},
    {0x02FF, "Spacing Modifier Letters"},
    {0x036F, "Combining Diacritical Marks"},
    {0x03FF, "Greek and Coptic"},
    {0x04FF, "Cyrillic"},
    {0x052F, "Cyrillic Supplement"},
    {0x058F, "Armenian"},
    {0x05FF, "Hebrew"},
    {0x06FF, "Arabic"},
    {0x074F, "Syriac"},
    {0x077F, "Arabic Supplement"},
    {0x07BF, "Thaana"},
    {0x07FF, "NKo"},
    {0x08FF, "Arabic Extended-A"},
    {0x097F, "Devanagari"},
    {0x09FF, "Bengali"},
    {0x0A7F, "Gurmukhi"},
    {0x0AFF, "Gujarati"},
    {0x0B7F, "Oriya"},
    {0x0BFF, "Tamil"},
    {0x0C7F, "Telugu"},
    {0x0CFF, "Kannada"},
    {0x0D7F, "Malayalam"},
    {0x0E7F, "Thai"},
    {0x0EFF, "Lao"},
    {0x0FFF, "Tibetan"},
    {0x109F, "Myanmar"},
    {0x10FF, "Georgian"},
    {0x11FF, "Hangul Jamo"},
    {0x137F, "Ethiopic"},
    {0x139F, "Ethiopic Supplement"},
    {0x13FF, "Cherokee"},
    {0x167F, "Unified Canadian Aboriginal Syllabics"},
    {0x169F, "Ogham"},
    {0x16FF, "Runic"},
    {0x177F, "Tagalog / Hanunoo / Buhid / Tagbanwa"},
    {0x17FF, "Khmer"},
    {0x18AF, "Mongolian"},
    {0x1AFF, "Sundanese / Balinese"},
    {0x1B7F, "Balinese"},
    {0x1C4F, "Lepcha / Ol Chiki"},
    {0x1C7F, "Vedic Extensions"},
    {0x1CFF, "Phonetic Extensions"},
    {0x1D7F, "Phonetic Extensions Supplement"},
    {0x1DBF, "Combining Diacritical Marks Supplement"},
    {0x1DFF, "Latin Extended Additional"},
    {0x1EFF, "Greek Extended"},
    {0x206F, "General Punctuation"},
    {0x209F, "Superscripts and Subscripts"},
    {0x20CF, "Currency Symbols"},
    {0x20FF, "Combining Diacritical Marks for Symbols"},
    {0x214F, "Letterlike Symbols"},
    {0x218F, "Number Forms"},
    {0x21FF, "Arrows"},
    {0x22FF, "Mathematical Operators"},
    {0x23FF, "Miscellaneous Technical"},
    {0x243F, "Control Pictures"},
    {0x245F, "Optical Character Recognition"},
    {0x24FF, "Enclosed Alphanumerics"},
    {0x257F, "Box Drawing"},
    {0x259F, "Block Elements"},
    {0x25FF, "Geometric Shapes"},
    {0x26FF, "Miscellaneous Symbols"},
    {0x27BF, "Dingbats"},
    {0x27EF, "Miscellaneous Mathematical Symbols-A"},
    {0x27FF, "Supplemental Arrows-A"},
    {0x28FF, "Braille Patterns"},
    {0x2BFF, "Miscellaneous Symbols and Arrows"},
    {0x2E7F, "Supplemental Punctuation"},
    {0x2EFF, "CJK Radicals Supplement"},
    {0x2FDF, "Kangxi Radicals"},
    {0x303F, "CJK Symbols and Punctuation"},
    {0x309F, "Hiragana"},
    {0x30FF, "Katakana"},
    {0x312F, "Bopomofo"},
    {0x318F, "Hangul Compatibility Jamo"},
    {0x319F, "Kanbun"},
    {0x31BF, "Bopomofo Extended"},
    {0x31EF, "CJK Strokes"},
    {0x31FF, "Katakana Phonetic Extensions"},
    {0x32FF, "Enclosed CJK Letters and Months"},
    {0x33FF, "CJK Compatibility"},
    {0x4DBF, "CJK Unified Ideographs Extension A"},
    {0x4DFF, "Yijing Hexagram Symbols"},
    {0x9FFF, "CJK Unified Ideographs"},
    {0xA48F, "Yi Syllables"},
    {0xA4CF, "Yi Radicals"},
    {0xAC00, "Vai / Cherokee / Syloti Nagri"},
    {0xD7AF, "Hangul Syllables"},
    {0xD7FF, "Hangul Jamo Extended-B"},
    {0xDFFF, "High/Low Surrogates"},
    {0xF8FF, "Private Use Area"},
    {0xFAFF, "CJK Compatibility Ideographs"},
    {0xFB4F, "Alphabetic Presentation Forms"},
    {0xFDFF, "Arabic Presentation Forms-A"},
    {0xFE0F, "Variation Selectors"},
    {0xFE1F, "Vertical Forms"},
    {0xFE2F, "Combining Half Marks"},
    {0xFE4F, "CJK Compatibility Forms"},
    {0xFE6F, "Small Form Variants"},
    {0xFEFF, "Arabic Presentation Forms-B / Specials"},
    {0xFFEF, "Halfwidth and Fullwidth Forms"},
    {0xFFFF, "Specials"},
    {0x1013F, "Aegean Numbers / Linear B"},
    {0x1037F, "Old Persian / Ugaritic"},
    {0x104AF, "Deseret / Shavian"},
    {0x1083F, "Imperial Aramaic / Palmyrene"},
    {0x1091F, "Phoenician / Lydian"},
    {0x1107F, "Brahmi"},
    {0x123FF, "Cuneiform"},
    {0x1342F, "Egyptian Hieroglyphs"},
    {0x16A3F, "Tangut"},
    {0x1D1FF, "Musical Symbols"},
    {0x1D7FF, "Mathematical Alphanumeric Symbols"},
    {0x1F02F, "Mahjong Tiles"},
    {0x1F09F, "Domino Tiles"},
    {0x1F0FF, "Playing Cards"},
    {0x1F2FF, "Enclosed Ideographic Supplement"},
    {0x1F5FF, "Miscellaneous Symbols and Pictographs"},
    {0x1F64F, "Emoticons"},
    {0x1F67F, "Ornamental Dingbats"},
    {0x1F6FF, "Transport and Map Symbols"},
    {0x1F77F, "Alchemical Symbols"},
    {0x1F7FF, "Geometric Shapes Extended"},
    {0x1F8FF, "Supplemental Arrows-C"},
    {0x1F9FF, "Supplemental Symbols and Pictographs"},
    {0x1FA6F, "Chess Symbols"},
    {0x1FAFF, "Symbols and Pictographs Extended-A"},
    {0x2A6DF, "CJK Unified Ideographs Extension B"},
    {0x2B73F, "CJK Unified Ideographs Extension C"},
    {0x2B81F, "CJK Unified Ideographs Extension D"},
    {0x2CEAF, "CJK Unified Ideographs Extension E"},
    {0x2EBEF, "CJK Unified Ideographs Extension F"},
    {0x2F7FF, "CJK Compatibility Ideographs Supplement"},
    {0x323AF, "CJK Unified Ideographs Extension G/H/I"},
}};

static QString getUnicodeBlock(uint32_t cp) {
    if (cp <= 0x323AF) {
        const auto it = std::ranges::lower_bound(unicodeBlocks, cp, {}, &UnicodeBlockEntry::endCp);
        if (it != unicodeBlocks.end()) {
            return QString::fromUtf8(it->name);
        }
    }
    if (cp >= 0xE0000 && cp <= 0xE007F) {
        return QStringLiteral("Tags");
    }
    if (cp >= 0xE0100 && cp <= 0xE01EF) {
        return QStringLiteral("Variation Selectors Supplement");
    }
    if (cp >= 0xF0000 && cp <= 0x10FFFF) {
        return QStringLiteral("Supplementary Private Use Area");
    }
    return QStringLiteral("Unassigned");
}

struct HtmlEntityEntry {
    uint32_t cp{0};
    const char *entity{nullptr};
};

static constexpr std::array<HtmlEntityEntry, 95> htmlEntities = {{

    {34, "&quot;"},
    {38, "&amp;"},
    {39, "&apos;"},
    {60, "&lt;"},
    {62, "&gt;"},
    {160, "&nbsp;"},
    {161, "&iexcl;"},
    {162, "&cent;"},
    {163, "&pound;"},
    {164, "&curren;"},
    {165, "&yen;"},
    {166, "&brvbar;"},
    {167, "&sect;"},
    {168, "&uml;"},
    {169, "&copy;"},
    {170, "&ordf;"},
    {171, "&laquo;"},
    {172, "&not;"},
    {173, "&shy;"},
    {174, "&reg;"},
    {175, "&macr;"},
    {176, "&deg;"},
    {177, "&plusmn;"},
    {178, "&sup2;"},
    {179, "&sup3;"},
    {180, "&acute;"},
    {181, "&micro;"},
    {182, "&para;"},
    {183, "&middot;"},
    {184, "&cedil;"},
    {185, "&sup1;"},
    {186, "&ordm;"},
    {187, "&raquo;"},
    {188, "&frac14;"},
    {189, "&frac12;"},
    {190, "&frac34;"},
    {191, "&iquest;"},
    {215, "&times;"},
    {247, "&divide;"},
    {8211, "&ndash;"},
    {8212, "&mdash;"},
    {8216, "&lsquo;"},
    {8217, "&rsquo;"},
    {8218, "&sbquo;"},
    {8220, "&ldquo;"},
    {8221, "&rdquo;"},
    {8222, "&bdquo;"},
    {8224, "&dagger;"},
    {8225, "&Dagger;"},
    {8226, "&bull;"},
    {8230, "&hellip;"},
    {8240, "&permil;"},
    {8242, "&prime;"},
    {8243, "&Prime;"},
    {8364, "&euro;"},
    {8482, "&trade;"},
    {8592, "&larr;"},
    {8593, "&uarr;"},
    {8594, "&rarr;"},
    {8595, "&darr;"},
    {8596, "&harr;"},
    {8704, "&forall;"},
    {8706, "&part;"},
    {8707, "&exist;"},
    {8709, "&empty;"},
    {8711, "&nabla;"},
    {8712, "&isin;"},
    {8713, "&notin;"},
    {8715, "&ni;"},
    {8719, "&prod;"},
    {8721, "&sum;"},
    {8722, "&minus;"},
    {8730, "&radic;"},
    {8734, "&infin;"},
    {8743, "&and;"},
    {8744, "&or;"},
    {8745, "&cap;"},
    {8746, "&cup;"},
    {8747, "&int;"},
    {8756, "&there4;"},
    {8764, "&sim;"},
    {8773, "&cong;"},
    {8776, "&asymp;"},
    {8800, "&ne;"},
    {8801, "&equiv;"},
    {8804, "&le;"},
    {8805, "&ge;"},
    {8834, "&sub;"},
    {8835, "&sup;"},
    {8838, "&sube;"},
    {8839, "&supe;"},
    {8853, "&oplus;"},
    {8855, "&otimes;"},
    {8869, "&perp;"},
    {8901, "&sdot;"},
}};

static const char *getHtmlNamedEntity(uint32_t cp) {
    const auto it = std::ranges::lower_bound(htmlEntities, cp, {}, &HtmlEntityEntry::cp);
    if (it != htmlEntities.end() && it->cp == cp) {
        return it->entity;
    }
    return nullptr;
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

struct ScriptEntry {
    QChar::Script script{QChar::Script_Common};
    const char *name{nullptr};
};

static constexpr std::array<ScriptEntry, 38> scriptEntries = {{

    {QChar::Script_Common, "Common"},
    {QChar::Script_Inherited, "Inherited"},
    {QChar::Script_Latin, "Latin"},
    {QChar::Script_Greek, "Greek"},
    {QChar::Script_Cyrillic, "Cyrillic"},
    {QChar::Script_Armenian, "Armenian"},
    {QChar::Script_Hebrew, "Hebrew"},
    {QChar::Script_Arabic, "Arabic"},
    {QChar::Script_Syriac, "Syriac"},
    {QChar::Script_Thaana, "Thaana"},
    {QChar::Script_Devanagari, "Devanagari"},
    {QChar::Script_Bengali, "Bengali"},
    {QChar::Script_Gurmukhi, "Gurmukhi"},
    {QChar::Script_Gujarati, "Gujarati"},
    {QChar::Script_Oriya, "Oriya"},
    {QChar::Script_Tamil, "Tamil"},
    {QChar::Script_Telugu, "Telugu"},
    {QChar::Script_Kannada, "Kannada"},
    {QChar::Script_Malayalam, "Malayalam"},
    {QChar::Script_Sinhala, "Sinhala"},
    {QChar::Script_Thai, "Thai"},
    {QChar::Script_Lao, "Lao"},
    {QChar::Script_Tibetan, "Tibetan"},
    {QChar::Script_Myanmar, "Myanmar"},
    {QChar::Script_Georgian, "Georgian"},
    {QChar::Script_Hangul, "Hangul"},
    {QChar::Script_Ethiopic, "Ethiopic"},
    {QChar::Script_Cherokee, "Cherokee"},
    {QChar::Script_Ogham, "Ogham"},
    {QChar::Script_Runic, "Runic"},
    {QChar::Script_Khmer, "Khmer"},
    {QChar::Script_Mongolian, "Mongolian"},
    {QChar::Script_Hiragana, "Hiragana"},
    {QChar::Script_Katakana, "Katakana"},
    {QChar::Script_Bopomofo, "Bopomofo"},
    {QChar::Script_Han, "Han (CJK Ideographs)"},
    {QChar::Script_Yi, "Yi"},
    {QChar::Script_Braille, "Braille"},
}};

static QString mapScript(QChar::Script s) {
    const auto it = std::ranges::find_if(scriptEntries, [s](const auto &e) { return e.script == s; });
    if (it != scriptEntries.end()) {
        return QString::fromUtf8(it->name);
    }
    return QStringLiteral("Unknown");
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

static QString getDisplayGlyph(uint32_t cp, const QString &defaultGlyph) {
    switch (cp) {
        case 0x00: return QStringLiteral("␀ (NUL)");
        case 0x07: return QStringLiteral("␇ (BEL)");
        case 0x08: return QStringLiteral("␈ (BS)");
        case 0x09: return QStringLiteral("␉ (TAB)");
        case 0x0A: return QStringLiteral("␊ (LF)");
        case 0x0B: return QStringLiteral("␋ (VT)");
        case 0x0C: return QStringLiteral("␌ (FF)");
        case 0x0D: return QStringLiteral("␍ (CR)");
        case 0x1B: return QStringLiteral("␛ (ESC)");
        case 0x20: return QStringLiteral("␣ (Space)");
        case 0x7F: return QStringLiteral("␡ (DEL)");
        default:
            if (cp < 0x20 || (cp >= 0x80 && cp <= 0x9F)) {
                return QStringLiteral("␀ (Control)");
            }
            return defaultGlyph;
    }
}

static void populateEscapes(uint32_t cp, CharacterInfo::Details &d) {
    switch (cp) {
        case '\n': d.cEscape = QStringLiteral("\\n"); break;
        case '\r': d.cEscape = QStringLiteral("\\r"); break;
        case '\t': d.cEscape = QStringLiteral("\\t"); break;
        case '\0': d.cEscape = QStringLiteral("\\0"); break;
        default:
            if (cp <= 0xFFFF) {
                d.cEscape = QStringLiteral("\\u%1").arg(cp, 4, 16, QChar('0')).toUpper();
            } else {
                d.cEscape = QStringLiteral("\\U%1").arg(cp, 8, 16, QChar('0')).toUpper();
            }
            break;
    }

    if (cp <= 0xFFFF) {
        d.jsonEscape = QStringLiteral("\\u%1").arg(cp, 4, 16, QChar('0')).toUpper();
    } else {
        const auto high = static_cast<uint16_t>(0xD800 + ((cp - 0x10000) >> 10));
        const auto low = static_cast<uint16_t>(0xDC00 + ((cp - 0x10000) & 0x3FF));
        d.jsonEscape = QStringLiteral("\\u%1\\u%2").arg(high, 4, 16, QChar('0')).arg(low, 4, 16, QChar('0')).toUpper();
    }
}

QString CharacterInfo::formatCodePoint(uint32_t cp) {
    const int fieldWidth = (cp <= 0xFFFF) ? 4 : 5;
    return QStringLiteral("U+%1").arg(cp, fieldWidth, 16, QChar('0')).toUpper();
}

CharacterInfo::Details CharacterInfo::getDetailsForCodePoint(uint32_t cp) {
    Details d;
    d.codePoint = cp;
    d.isValid = true;

    const std::array<char32_t, 1> ucs = {cp};
    d.glyph = QString::fromUcs4(ucs.data(), 1);
    d.displayGlyph = getDisplayGlyph(cp, d.glyph);

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
    for (const auto byte : d.utf8Bytes) {
        const auto b = static_cast<unsigned char>(byte);
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
    if (const char *namedEnt = getHtmlNamedEntity(cp); namedEnt != nullptr) {
        d.htmlNamedEntity = QString::fromLatin1(namedEnt);
    }
    d.htmlDecEntity = QStringLiteral("&#%1;").arg(cp);
    d.htmlHexEntity = QStringLiteral("&#x%1;").arg(cp, 0, 16).toUpper();

    // Escape Sequences
    populateEscapes(cp, d);

    d.cssEscape = QStringLiteral("\\%1 ").arg(cp, 0, 16).toLower();
    d.urlPercent = QString::fromUtf8(d.utf8Bytes.toPercentEncoding());

    return d;
}

CharacterInfo::Details CharacterInfo::getDetailsAt(const ScintillaNext *editor, int pos) {
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

    const auto lead = static_cast<unsigned char>(editor->charAt(pos));
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

    const char32_t cp = str.toUcs4().value(0, 0);
    if (cp == 0)
        return Details();

    return getDetailsForCodePoint(cp);
#endif
}

QString CharacterInfo::formatSummary(const Details &d) {
    if (!d.isValid)
        return QStringLiteral("Invalid character");

    return QStringLiteral("%1 (%2) - %3 | Category: %4 | UTF-8: %5")
        .arg(d.displayGlyph, formatCodePoint(d.codePoint), d.name, d.categoryCode, d.utf8Hex);
}
