local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"proto",
}

L.keywords = {
	[0] = "syntax proto2 proto3 import package option message enum service rpc returns reserved extensions to extend repeated optional required oneof map stream",
	[1] = "double float int32 int64 uint32 uint64 sint32 sint64 fixed32 fixed64 sfixed32 sfixed64 bool string bytes",
}

return L
