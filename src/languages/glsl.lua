local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"glsl",
	"vert",
	"frag",
	"geom",
	"comp",
}

L.keywords = {
	[0] = "attribute const uniform varying layout centroid flat smooth noperspective in out inout break continue do for while switch case default if else discard return struct precision highp mediump lowp",
	[1] = "void bool int uint float vec2 vec3 vec4 bvec2 bvec3 bvec4 ivec2 ivec3 ivec4 uvec2 uvec3 uvec4 mat2 mat3 mat4 sampler2D sampler3D samplerCube",
}

return L
