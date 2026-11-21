local L = {}

L.lexer = "asm"

L.singleLineComment = "; "

L.extensions = {
	"ll",
}

L.keywords = {
	[0] = "define declare target datalayout triple type alloca load store fence cmpxchg atomicrmw getelementptr ret br switch indirectbr invoke resume unreachable add fadd sub fsub mul fmul udiv sdiv fdiv urem srem frem shl lshr ashr and or xor extractelement insertelement shufflevector extractvalue insertvalue phi select call va_arg landingpad trunc zext sext fptrunc fpext fptoui fptosi uitofp sitofp ptrtoint inttoptr bitcast addrspacecast icmp fcmp eq ne ugt uge ult ule sgt sge slt sle oeq ogt oge olt ole one ord ueq une uno true false void half float double x86_fp80 fp128 ppc_fp128 label metadata x86_mmx x86_amx",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x008000),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMBER"] = {
		id = 2,
		fgColor = rgb(0xFF8000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING"] = {
		id = 3,
		fgColor = rgb(0x800000),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATOR"] = {
		id = 4,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 5,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["CPUINSTRUCTION"] = {
		id = 6,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
}

return L
