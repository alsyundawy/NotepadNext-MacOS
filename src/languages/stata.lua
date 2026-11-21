local L = {}

L.lexer = "stata"

L.singleLineComment = "// "

L.extensions = {
	"do",
	"ado",
	"dta",
}

L.keywords = {
	[0] = "clear use save merge append drop keep gen egen replace sort by quietly noisily if else foreach forvalues while display quietly regress correlate summarize tabulate list",
	[1] = "byte int long float double str",
}

return L
