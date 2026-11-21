local L = {}

L.lexer = "toml"

L.singleLineComment = "# "

L.extensions = {
	"toml",
}

L.keywords = {
	[0] = "true false inf nan nan.nan +inf -inf +nan -nan",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 2,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD"] = {
		id = 3,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["TABLE"] = {
		id = 5,
		fgColor = rgb(0x800080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEY"] = {
		id = 6,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["ERROR"] = {
		id = 7,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFEEEE),
	},
	["OPERATOR"] = {
		id = 8,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["STRING_SQ"] = {
		id = 9,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING_DQ"] = {
		id = 10,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["TRIPLE_STRING_SQ"] = {
		id = 11,
		fgColor = rgb(0xA06000),
		bgColor = rgb(0xFFFFFF),
	},
	["TRIPLE_STRING_DQ"] = {
		id = 12,
		fgColor = rgb(0xA06000),
		bgColor = rgb(0xFFFFFF),
	},
	["ESCAPECHAR"] = {
		id = 13,
		fgColor = rgb(0xA00000),
		bgColor = rgb(0xFFFFFF),
	},
	["DATETIME"] = {
		id = 14,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRINGEOL"] = {
		id = 15,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFEEEE),
	},
}
return L
