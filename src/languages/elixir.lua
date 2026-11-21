local L = {}

L.lexer = "ruby"

L.singleLineComment = "# "

L.extensions = {
	"ex",
	"exs",
}

L.keywords = {
	[0] = "def defp defmodule defprotocol defimpl defmacro defstruct quote unquote do end if else unless cond case with try catch after rescue receive fn",
	[1] = "true false nil and or not when in use alias import require",
}

return L
