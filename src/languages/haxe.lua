local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"hx",
}

L.keywords = {
	[0] = "class interface enum typedef abstract function var final public private static inline override dynamic untyped new this super cast return if else while do for break continue switch case default try catch throw import using package",
	[1] = "Void Int Float Bool String Dynamic Null Array Map",
}

return L
