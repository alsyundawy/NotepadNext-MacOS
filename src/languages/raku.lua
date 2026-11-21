local L = {}

L.lexer = "raku"

L.singleLineComment = "# "

L.extensions = {
	"raku",
	"rakumod",
	"p6",
	"pl6",
	"pm6",
}

L.keywords = {
	[0] = "my our has sub method regex token rule role class grammar package module use need unit constant state subset return if elsif else unless given when default while until for loop gather take try catch quiet leave keep undo",
	[1] = "Int Str Num Rat Bool Array Hash List Set Bag Any Mu",
}

return L
