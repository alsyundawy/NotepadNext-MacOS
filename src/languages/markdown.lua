local L = {}

L.lexer = "markdown"

L.disableFoldMargin = true

L.extensions = {
    "md",
    "markdown",
}

L.styles = {
    ["DEFAULT"] = {
        id = 0,
    },
    ["SCE_MARKDOWN_STRONG1"] = {
        id = 2,
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_STRONG2"] = {
        id = 3,
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_EM1"] = {
        id = 4,
        fontStyle = 2,
    },
    ["SCE_MARKDOWN_EM2"] = {
        id = 5,
        fontStyle = 2,
    },
    ["SCE_MARKDOWN_HEADER1"] = {
        id = 6,
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER2"] = {
        id = 7,
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER3"] = {
        id = 8,
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER4"] = {
        id = 9,
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER5"] = {
        id = 10,
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_HEADER6"] = {
        id = 11,
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_PRECHAR"] = {
        id = 12,
    },
    ["SCE_MARKDOWN_ULIST_ITEM"] = {
        id = 13,
    },
    ["SCE_MARKDOWN_OLIST_ITEM"] = {
        id = 14,
    },
    ["SCE_MARKDOWN_OLIST_ITEM"] = {
        id = 15,
    },
    ["SCE_MARKDOWN_STRIKEOUT"] = {
        id = 16,
    },
    ["SCE_MARKDOWN_HRULE"] = {
        id = 17,
        fontStyle = 1,
    },
    ["SCE_MARKDOWN_LINK"] = {
        id = 18,
        fontStyle = 4,
    },
    ["SCE_MARKDOWN_CODE"] = {
        id = 19,
    },
    ["SCE_MARKDOWN_CODE2"] = {
        id = 20,
    },
    ["SCE_MARKDOWN_CODEBK"] = {
        id = 20,
    },
}

return L
