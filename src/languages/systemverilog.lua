local L = {}

L.lexer = "verilog"

L.singleLineComment = "// "

L.extensions = {
	"sv",
	"svh",
}

L.keywords = {
	[0] = "always always_comb always_ff always_latch and assign begin break case casex casez class clocking config const constraint context continue covergroup default defparam design disable do edge else end endcase endclass endclocking endconfig endfunction endgenerate endinterface endmodule endpackage endprimitive endprogram endproperty endspecify endtable endtask enum event export extern final for force foreach forever fork forkjoin function generate genvar if iff ifnone import initial inout input inside instance integer interface intersect join join_any join_none local localparam logic macromodule matches modport module nand negedge new nmos nor noshowcancelled not notif0 notif1 or output package packed parameter pmos posedge primitive priority program property protected pull0 pull1 pulldown pullup rcmos real realtime ref reg release repeat return rnmos rpmos rtran rtranif0 rtranif1 scalared sequence shortint shortreal showcancelled signed small solve specify specparam static string strong0 strong1 struct super supply0 supply1 table tagged task this time timeprecision timeunit tran tranif0 tranif1 tri tri0 tri1 triand trior trireg type typedef union unique unique0 unsigned use uwire var vectored virtual void wait wait_order wand weak0 weak1 while wildcard wire with wor xnor xor",
	[1] = "logic bit byte int shortint longint integer time real realtime string",
}

return L
