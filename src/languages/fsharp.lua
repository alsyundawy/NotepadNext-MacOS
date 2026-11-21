local L = {}

L.lexer = "fsharp"

L.singleLineComment = "// "

L.extensions = {
	"fs",
	"fsi",
	"fsx",
}

L.keywords = {
	[0] = "abstract and as assert base begin class default delegate do done downcast downto elif else end exception extern false finally for fun function global if in inherit inline interface internal lazy let match member module mutable namespace new not null of open or override private public rec return static struct then to true try type upcast use val void when while with yield",
	[1] = "bool byte sbyte int16 uint16 int uint int64 uint64 nativeint unativeint decimal float32 float char string unit obj",
}

return L
