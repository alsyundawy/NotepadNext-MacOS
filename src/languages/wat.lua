local L = {}

L.lexer = "lisp"

L.singleLineComment = ";; "

L.extensions = {
	"wat",
	"wast",
}

L.keywords = {
	[0] = "module func param result local type import export table memory global data elem start offset mut block loop if then else end br br_if br_table call call_indirect return drop select nop unreachable",
	[1] = "i32 i64 f32 f64 v128 funcref externref anyref eqref i8 i16 i32.const i64.const f32.const f64.const",
	[2] = "i32.load i64.load f32.load f64.load i32.store i64.store f32.store f64.store i32.add i32.sub i32.mul i32.div_s i32.div_u i64.add i64.sub i64.mul memory.size memory.grow",
}

return L
