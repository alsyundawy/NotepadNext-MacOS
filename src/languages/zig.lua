local L = {}

L.lexer = "zig"

L.singleLineComment = "// "

L.extensions = {
	"zig",
	"zon",
}

L.keywords = {
	[0] = "addrspace align allowzero and anyframe anytype asm async await break callconv catch comptime const continue defer do dump nosuspend noalias noinline opaque or orelse packed pub resume return struct suspend switch test threadlocal try union unreachable usingnamespace var volatile while error",
	[1] = "anyerror anyopaque bool c_char c_int c_long c_longdouble c_longlong c_short c_uint c_ulong c_ulonglong c_ushort comptime_float comptime_int f128 f16 f32 f64 f80 false i128 i16 i32 i64 i8 isize noreturn null true type u128 u16 u32 u64 u8 undefined usize void",
	[2] = "@abs @addWithOverflow @addrSpaceCast @alignCast @alignOf @atomicLoad @atomicRmw @atomicStore @bitCast @bitOffsetOf @bitReverse @bitSizeOf @breakpoint @byteSwap @cImport @cInclude @cUndef @call @canImplicitCast @ceil @clz @compileError @compileLog @cos @ctz @divExact @divFloor @divTrunc @embedFile @enumFromInt @errorCast @errorFromInt @errorName @errorReturnTrace @exp @exp2 @export @extern @field @fieldParentPtr @floor @frameAddress @hasDecl @hasField @import @inComptime @intCast @intFromBool @intFromEnum @intFromError @intFromFloat @intFromPtr @log @log10 @log2 @max @memcpy @memset @min @mod @mulAdd @mulWithOverflow @offsetOf @panic @popCount @prefetch @ptrCast @ptrFromInt @rem @returnAddress @round @select @setAlignStack @setCold @setEvalBranchQuota @setFloatMode @setRuntimeSafety @shlExact @shlWithOverflow @shrExact @sin @sizeOf @splat @sqrt @src @subWithOverflow @tagName @tan @trap @truncate @trunc @typeInfo @typeName @typeof @unionInit @vectorType @wasmMemoryGrow @wasmMemorySize",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTLINE"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTLINEDOC"] = {
		id = 2,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENTLINETOP"] = {
		id = 3,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["NUMBER"] = {
		id = 4,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 5,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["CHARACTER"] = {
		id = 6,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 7,
		fgColor = rgb(0x808080),
		bgColor = rgb(0xFFFFFF),
	},
	["MULTISTRING"] = {
		id = 8,
		fgColor = rgb(0xA06000),
		bgColor = rgb(0xFFFFFF),
	},
	["ESCAPECHAR"] = {
		id = 9,
		fgColor = rgb(0xA00000),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIER"] = {
		id = 10,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["FUNCTION"] = {
		id = 11,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["BUILTIN_FUNCTION"] = {
		id = 12,
		fgColor = rgb(0x800080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KW_PRIMARY"] = {
		id = 13,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KW_SECONDARY"] = {
		id = 14,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
	},
	["KW_TERTIARY"] = {
		id = 15,
		fgColor = rgb(0x800080),
		bgColor = rgb(0xFFFFFF),
	},
	["KW_TYPE"] = {
		id = 16,
		fgColor = rgb(0x8000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["IDENTIFIER_STRING"] = {
		id = 17,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
	},
	["STRINGEOL"] = {
		id = 18,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFEEEE),
	},
}
return L
