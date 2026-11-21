local L = {}

L.lexer = "conf"

L.singleLineComment = "# "

L.extensions = {
	"conf",
	"cfg",
}

L.keywords = {
	[0] = "server listen server_name location root index proxy_pass alias error_page access_log include set",
	[1] = "on off true false yes no enable disable all none",
}

return L
