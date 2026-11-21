local L = {}

L.lexer = "julia"

L.singleLineComment = "# "

L.extensions = {
	"jl",
}

L.keywords = {
	[0] = "baremodule begin break catch const continue do else elseif end export false finally for function global if import let local macro module mutable primitive quote return struct true try type using where while abstract",
	[1] = "Any Array Bool Complex Dict Float16 Float32 Float64 Function Int Int128 Int16 Int32 Int64 Int8 Integer Nothing Number Real Set String Symbol Tuple UInt UInt128 UInt16 UInt32 UInt64 UInt8 Union Val Vector Void",
	[2] = "@assert @async @btime @doc @eval @inline @noinline @show @sync @test @time @threads @views",
}

return L
