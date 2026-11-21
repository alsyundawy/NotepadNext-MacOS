local L = {}

L.lexer = "hex"

L.singleLineComment = "; "

L.extensions = {
	"hex",
	"bin",
}

L.keywords = {
	[0] = "OFFSET BYTES ASCII DATA CHECKSUM RECORD TYPE",
	[1] = "START END EXTENDED LINEAR SEGMENT",
}

return L
