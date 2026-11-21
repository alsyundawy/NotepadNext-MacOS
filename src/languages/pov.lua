local L = {}

L.lexer = "pov"

L.singleLineComment = "// "

L.extensions = {
	"pov",
	"inc",
}

L.keywords = {
	[0] = "camera light_source object background sphere box cylinder cone plane torus blob mesh texture pigment finish normal interior color rgb rgbf rgbt rotate translate scale union intersection difference merge declare default version",
	[1] = "ambient diffuse specular roughness reflection refraction phong metallic",
}

return L
