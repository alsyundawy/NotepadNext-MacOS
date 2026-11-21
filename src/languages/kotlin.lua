local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"kt",
	"kts",
}

L.keywords = {
	[0] = "as break class continue do else false for fun if in interface is null object package return super this throw true try typealias val var when while by catch constructor delegate dynamic field file finally get import init noinline open operator out param property receiver set suspend where actual abstract annotation companion const crossinline data enum expect external final inline inner internal lateinit noinline open operator override private protected public reified sealed suspend tailrec vararg yield",
	[1] = "Boolean Byte Short Int Long Float Double Char String Unit Any Nothing Array List Set Map Sequence MutableList MutableSet MutableMap CoroutineContext Job Deferred Flow Channel",
	[2] = "println print require check error with apply let run also takeIf takeUnless lazy repeat launch async runBlocking",
}

return L
