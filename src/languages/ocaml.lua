local L = {}

L.lexer = "caml"

L.singleLineComment = "(* *)"

L.extensions = {
	"ml",
	"mli",
}

L.keywords = {
	[0] = "and as assert begin class constraint do done downto else end exception external false for fun function functor if in include inherit initializer land lazy let lor lsl lsr lxor match method mod module mutable new nonrec object of open or private rec sig struct then to true try type val virtual when while with",
	[1] = "int float bool char string unit list array option",
}

return L
