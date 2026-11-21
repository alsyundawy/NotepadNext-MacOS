local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"bicep",
}

L.keywords = {
	[0] = "param var resource module output existing targetScope if for in with true false null metadata type func import as",
	[1] = "string int bool array object secureString secureObject",
	[2] = "@description @secure @allowed @minLength @maxLength @minValue @maxValue @metadata @batchSize resourceGroup subscription deployment tenant environment uniqueString concat format guid base64",
}

return L
