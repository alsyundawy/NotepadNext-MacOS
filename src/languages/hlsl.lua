local L = {}

L.lexer = "cpp"

L.singleLineComment = "// "

L.extensions = {
	"hlsl",
	"fx",
}

L.keywords = {
	[0] = "cbuffer tbuffer register packoffset texture sampler SamplerState Texture2D TextureCube struct typedef return if else for while do switch case default break continue discard inline uniform volatile",
	[1] = "void bool int uint float double half min16float min10float min16int min12int min16uint vector matrix float2 float3 float4 int2 int3 int4 uint2 uint3 uint4",
}

return L
