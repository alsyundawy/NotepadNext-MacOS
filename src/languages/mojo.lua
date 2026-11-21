local L = {}

L.lexer = "python"

L.singleLineComment = "# "

L.extensions = {
	"mojo",
	"🔥",
}

L.keywords = {
	[0] = "and as assert async await break fn def let var struct trait comptime alias raises parameter ref inout borrow owned in is not or pass return try while for if elif else continue with yield delete",
	[1] = "Int Int8 Int16 Int32 Int64 UInt UInt8 UInt16 UInt32 UInt64 Float16 Float32 Float64 Bool String SIMD DType Pointer AnyType NoneType",
	[2] = "__init__ __copyinit__ __moveinit__ __del__ __getitem__ __setitem__ __len__ __str__ __repr__ print len range abort",
}

return L
