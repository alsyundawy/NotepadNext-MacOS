local L = {}

L.lexer = "java"

L.singleLineComment = "// "

L.extensions = {
	"groovy",
	"gvy",
	"gy",
	"gsh",
}

L.keywords = {
	[0] = "as assert def trait it in class interface enum extends implements package import public protected private static final abstract transient volatile native strictfp synchronized return if else switch case default for while do break continue throw throws try catch finally new this super",
	[1] = "byte short char int long float double boolean void String Object List Map Set BigInteger BigDecimal",
}

return L
