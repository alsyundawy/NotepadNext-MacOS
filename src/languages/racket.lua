local L = {}

L.lexer = "scheme"

L.singleLineComment = "; "

L.extensions = {
	"rkt",
}

L.keywords = {
	[0] = "define define-syntax lambda let let* letrec if cond case begin and or set! delay quote quasiquote unquote unquote-splicing syntax-rules provide require struct class interface",
	[1] = "#t #f null cons car cdr list vector string boolean? number? pair? symbol?",
}

return L
