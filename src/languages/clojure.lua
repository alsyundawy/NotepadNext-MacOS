local L = {}

L.lexer = "lisp"

L.singleLineComment = "; "

L.extensions = {
	"clj",
	"cljs",
	"cljc",
	"edn",
}

L.keywords = {
	[0] = "def defn defmacro defmulti defmethod defprotocol defrecord deftype fn let loop recur if if-not when when-not do case cond try catch finally ns require use import quote",
	[1] = "true false nil vector list hash-map hash-set atom ref agent deref conj assoc dissoc get count map filter reduce",
}

return L
