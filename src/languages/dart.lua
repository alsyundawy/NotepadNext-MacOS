local L = {}

L.lexer = "dart"

L.singleLineComment = "// "

L.extensions = {
	"dart",
}

L.keywords = {
	[0] = "abstract as assert async await base break case catch class const continue covariant default deferred do dynamic else enum export extends extension external factory false final finally for function get hide if implements import in interface is late library mixin native new null of on operator part required rethrow return sealed set show static super switch sync this throw true try typedef var void when while with yield",
	[1] = "bool double int num String List Map Set Iterable Future Stream Object dynamic Never Type void Null Runes Symbol",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTLINE"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTLINEDOC"] = {
		id = 2,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTBLOCK"] = {
		id = 3,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTBLOCKDOC"] = {
		id = 4,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING_SQ"] = {
		id = 5,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING_DQ"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["TRIPLE_STRING_SQ"] = {
		id = 7,
		fgColor = rgb(0xA06000),
		bgColor = rgb(0xFFFFFF),
	},
	["TRIPLE_STRING_DQ"] = {
		id = 8,
		fgColor = rgb(0xA06000),
		bgColor = rgb(0xFFFFFF),
	},
	["RAWSTRING_SQ"] = {
		id = 9,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["RAWSTRING_DQ"] = {
		id = 10,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["TRIPLE_RAWSTRING_SQ"] = {
		id = 11,
		fgColor = rgb(0xA06000),
		bgColor = rgb(0xFFFFFF),
	},
	["TRIPLE_RAWSTRING_DQ"] = {
		id = 12,
		fgColor = rgb(0xA06000),
		bgColor = rgb(0xFFFFFF),
	},
	["ESCAPECHAR"] = {
		id = 13,
		fgColor = rgb(0xA00000),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 14,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 16,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 20,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["METADATA"] = {
		id = 22,
		fgColor = rgb(0x808000),
		bgColor = rgb(0xFFFFFF),
	},
	["KW_PRIMARY"] = {
		id = 23,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KW_SECONDARY"] = {
		id = 24,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["KW_TYPE"] = {
		id = 26,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["STRINGEOL"] = {
		id = 27,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFEEEE),
	},
}
return L
