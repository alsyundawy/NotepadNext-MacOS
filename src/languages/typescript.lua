local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"ts",
	"tsx",
	"mts",
	"cts",
}

L.properties = {
	["lexer.cpp.backquoted.strings"] = "1",
}

L.keywords = {
	[0] = "abstract as async await break case catch class const constructor continue debugger declare default delete do else enum export extends false finally for from function get if implements import in instanceof interface is keyof let module namespace new null of package private protected public readonly return set static super switch symbol this throw true try type typeof var void while with yield satisfies infer override using accessor",
	[1] = "any boolean bigint never number object string symbol unknown void Array Date RegExp Map Set Promise Record Partial Required Readonly Pick Omit Exclude Extract NonNullable Parameters ReturnType InstanceType Awaited Uppercase Lowercase Capitalize Uncapitalize",
	[2] = "param returns return type typedef property template see link example throws deprecated",
	[3] = "alert console document window global globalThis process setTimeout setInterval clearTimeout clearInterval",
}

L.styles = {
	["DEFAULT"] = {
		id = 11,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["INSTRUCTION WORD"] = {
		id = 5,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["TYPE WORD"] = {
		id = 16,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRINGRAW"] = {
		id = 20,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xC0C0C0),
	},
	["CHARACTER"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 10,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["VERBATIM"] = {
		id = 13,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["REGEX"] = {
		id = 14,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE"] = {
		id = 2,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT DOC"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT LINE DOC"] = {
		id = 15,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT DOC KEYWORD"] = {
		id = 17,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["COMMENT DOC KEYWORD ERROR"] = {
		id = 18,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
}
return L
