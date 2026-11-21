local L = {}

L.lexer = "troff"

L.singleLineComment = ".\" "

L.extensions = {
	"tr",
	"roff",
	"man",
}

L.keywords = {
	[0] = "TH SH SS PP LP IP TP HP RS RE B I BI BR IB RB IR RI SM SB",
	[1] = "so ps ft vs in ll ls ti nf fi ce br sp",
}

return L
