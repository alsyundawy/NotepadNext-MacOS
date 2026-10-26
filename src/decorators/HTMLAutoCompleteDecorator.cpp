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

#include "HTMLAutoCompleteDecorator.h"
#include "SciLexer.h"
#include "UndoAction.h"

static const int MAX_TAG_NAME_LENGTH = 64;
static const int MAX_TAG_LENGTH = 1024;

static const QByteArrayList voidTags = { "area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "source", "track", "wbr" }; // NOLINT(bugprone-throwing-static-initialization)


HTMLAutoCompleteDecorator::HTMLAutoCompleteDecorator(ScintillaNext *editor)
    : EditorDecorator(editor)
{
    connect(editor, &ScintillaNext::lexerChanged, this, [=, this]() {
        QString language = editor->languageName;
        setEnabled(language == "HTML");
    });
}

void HTMLAutoCompleteDecorator::notify(const Scintilla::NotificationData *pscn)
{
    if (pscn->nmhdr.code == Scintilla::Notification::CharAdded) {
        if (pscn->ch == '>') {
            const int currentPos = static_cast<int>(editor->currentPos());

            // Skip if it is inside any scripting language
            if (editor->styleAt(currentPos) >= SCE_HJ_START) {
                return;
            }

            int tagEndPos = static_cast<int>(editor->positionBefore(currentPos));
            int tagStartPos = static_cast<int>(editor->positionBefore(tagEndPos));

            // Check the char right before the >
            const int prevChar = static_cast<int>(editor->charAt(tagStartPos));
            if (prevChar == '-') {
                return; // Assume -->
            }
            if (prevChar == '/') {
                return; // <tag/>
            }

            // Go backwards searching for the beginning of the tag
            while ((currentPos - tagStartPos) <= MAX_TAG_LENGTH) {
                const int c = static_cast<int>(editor->charAt(tagStartPos));

                // While backing up, save the position of the last whitespace character we encounter
                if (std::isspace(c) != 0) {
                    tagEndPos = tagStartPos;
                }

                // Found the start (yes this can be fooled but this is by far the easiest for now)
                if (c == '<') {
                    break;
                }

                // Reached the beginning of the document
                if (tagStartPos == 0) {
                    return;
                }

                tagStartPos = static_cast<int>(editor->positionBefore(tagStartPos));
            }

            // For sanity, check the length of the tag name
            if (tagEndPos - tagStartPos >= MAX_TAG_NAME_LENGTH) {
                return;
            }

            QByteArray tag = editor->get_text_range(static_cast<int>(editor->positionAfter(tagStartPos)), tagEndPos);

            if (tag.isEmpty()) {
                return; // <>
            }
            if (tag[0] == '/') {
                return; // </div>
            }
            if (tag[0] == '?') {
                return; // <?php>
            }
            if (tag[0] == '!') {
                return; // <!-- and <!doctype
            }
            if (voidTags.contains(tag)) {
                return; // Some tags are not expected to have closing tags
            }

            // All good to go now, wrap it and insert it
            tag.prepend("</");
            tag.append('>');

            const UndoAction ua(editor);
            editor->insertText(currentPos, tag.constData());
        }
    }
}
