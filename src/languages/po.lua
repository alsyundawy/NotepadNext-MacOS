local L = {}

L.lexer = "po"

L.singleLineComment = "# "

L.extensions = {
	"po",
	"pot",
}

L.keywords = {
	[0] = "msgid msgid_plural msgstr msgctxt fuzzy",
	[1] = "c-format python-format qt-format php-format",
}

return L
