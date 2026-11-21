local L = {}

L.lexer = "ruby"

L.singleLineComment = "# "

L.extensions = {
	"tf",
	"tfvars",
	"tofu",
}

L.keywords = {
	[0] = "terraform provider resource data variable output locals module check moved import dynamic content for_each count lifecycle provisioner connection",
	[1] = "string number bool list set map object tuple any default description type sensitive nullable validation precondition postcondition",
}

return L
