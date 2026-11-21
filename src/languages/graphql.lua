local L = {}

L.lexer = "cpp"

L.singleLineComment = "# "

L.extensions = {
	"graphql",
	"gql",
}

L.keywords = {
	[0] = "query mutation subscription type input interface union enum scalar schema directive on fragment extend implements true false null",
	[1] = "Int Float String Boolean ID",
	[2] = "@include @skip @deprecated @specifiedBy",
}

return L
