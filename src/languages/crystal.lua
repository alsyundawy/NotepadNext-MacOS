local L = {}

L.lexer = "ruby"

L.singleLineComment = "# "

L.extensions = {
	"cr",
}

L.keywords = {
	[0] = "abstract alias annotation asm as as? atomic begin break case class def do else elsif end ensure enum extend false for fun getter if include in instance_sizeof is_a? lib macro module next nil nil? offsetof out pointerof previous_def private property protected rescue responds_to? return select self setter sizeof struct super then true type typeof uninitialized union unless until verbatim when while yield",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["ERROR"] = {
		id = 1,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTLINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["POD"] = {
		id = 3,
		fgColor = rgb(0x004000),
		bgColor = rgb(0xC0FFC0),
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["WORD"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
	},
	["CHARACTER"] = {
		id = 7,
		fgColor = rgb(0x808000),
		bgColor = rgb(0xFFFFFF),
	},
	["CLASSNAME"] = {
		id = 8,
		fgColor = rgb(0x800080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["DEFNAME"] = {
		id = 9,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 11,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
}

return L
