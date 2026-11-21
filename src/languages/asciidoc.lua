local L = {}

L.lexer = "asciidoc"

L.singleLineComment = "// "

L.extensions = {
	"adoc",
	"asciidoc",
}

L.keywords = {
	[0] = "include ifdef ifndef endif tag end link image footnote callout audio video",
	[1] = "note tip warning caution important caption toc doctype author title",
}

return L
