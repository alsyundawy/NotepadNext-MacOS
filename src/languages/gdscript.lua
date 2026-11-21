local L = {}

L.lexer = "gdscript"

L.singleLineComment = "# "

L.extensions = {
	"gd",
}

L.keywords = {
	[0] = "if elif else for while match break continue pass return class class_name extends is as self tool signal func static const enum var onready export setget breakpoint",
	[1] = "void bool int float String Vector2 Vector3 Rect2 Transform Transform2D Color Plane Quat AABB Basis Node Node2D Spatial Viewport CanvasItem",
}

return L
