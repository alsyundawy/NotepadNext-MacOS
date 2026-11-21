local L = {}

L.lexer = "json"

L.first_line = {
	"^{[\r\n]",
}

L.extensions = {
	"json5",
}

L.properties = {
	["lexer.json.escape.sequence"] = "1",
	["lexer.json.allow.comments"] = "1",
}

L.keywords = {
	[0] = "false null true NaN Infinity -Infinity undefined",
	[1] = "@id @context @type @value @language @container @list @set @reverse @index @base @vocab @graph",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 1,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 2,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRINGEOL"] = {
		id = 3,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["PROPERTYNAME"] = {
		id = 4,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["ESCAPESEQUENCE"] = {
		id = 5,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["LINECOMMENT"] = {
		id = 6,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["BLOCKCOMMENT"] = {
		id = 7,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 8,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["URI"] = {
		id = 9,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 4,
	},
	["COMPACTIRI"] = {
		id = 10,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORD"] = {
		id = 11,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["LDKEYWORD"] = {
		id = 12,
		fgColor = rgb(0x800080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["ERROR"] = {
		id = 13,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFFFF),
	},
}

return L
