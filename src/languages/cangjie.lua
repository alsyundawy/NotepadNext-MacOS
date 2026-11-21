local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "
L.comment = { "/*", "*/" }

L.extensions = {
	"cj",
}

L.properties = {
	["lexer.cpp.track.preprocessor"] = "0",
	["styling.within.preprocessor"] = "1",
}

L.keywords = {
	[0] = "package import class interface struct enum func var let mut init main prop get set static open override sealed abstract private protected public if else match case while for in break continue return throw try catch finally spawn synchronized unsafe as is true false None Some",
	[1] = "Unit Nothing Bool Int8 Int16 Int32 Int64 IntNative UInt8 UInt16 UInt32 UInt64 UIntNative Float16 Float32 Float64 Rune String Array ArrayList HashMap HashSet Option Result",
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
	["COMMENTLINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTDOC"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
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
	["WORD2"] = {
		id = 16,
		fgColor = rgb(0x800080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
}

return L
