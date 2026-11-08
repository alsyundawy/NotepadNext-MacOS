local L = {}

L.name = "Text"

L.lexer = "null"

L.disableFoldMargin = true

L.extensions = {
    "",
    "txt",
    "text",
    "log",
    "csv",
    "tsv",
    "tab",
    "def",
    "list",
    "in",
    "out",
    "ans",
    "asc",
    "diz",
    "me",
    "now",
    "1st",
    "sub",
    "idx",
    "ics",
    "ifb",
    "vcf",
    "vcard",
    "vtt",
    "srt",
    "ass",
    "ssa",
    "lrc",
    "rst",
    "adoc",
    "asciidoc",
    "org",
    "gitattributes",
    "gitignore",
    "dockerignore",
}

L.styles = {
    ["DEFAULT"] = {
        id = 0,
        fgColor = rgb(0x000000),
        bgColor = rgb(0xFFFFFF),
        fontStyle = 0,
    },
}

return L
