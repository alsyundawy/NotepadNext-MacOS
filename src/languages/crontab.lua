local L = {}

L.lexer = "crontab"

L.singleLineComment = "# "

L.extensions = {
	"cron",
	"crontab",
}

L.keywords = {
	[0] = "@reboot @yearly @annually @monthly @weekly @daily @midnight @hourly SHELL PATH MAILTO HOME",
	[1] = "jan feb mar apr may jun jul aug sep oct nov dec sun mon tue wed thu fri sat",
}

return L
