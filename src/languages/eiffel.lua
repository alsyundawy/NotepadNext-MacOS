local L = {}

L.lexer = "eiffel"

L.singleLineComment = "-- "

L.extensions = {
	"e",
}

L.keywords = {
	[0] = "across agent alias all and as assign attribute check class convert create creation current debug deferred do else elseif end ensure expanded export external false feature from frozen if implies indexing inherit inspect invariant is like local loop not note obsolete old once or precondition quotient real_constant redefine rename require rescue result retry select separate then true undefine until variant void when xor",
	[1] = "BOOLEAN CHARACTER INTEGER REAL STRING ARRAY LIST HASH_TABLE POINTER",
}

return L
