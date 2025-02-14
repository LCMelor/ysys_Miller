module EXU (
    input [9:0] opcode,
    input [31:0] src1,
    input [31:0] src2,
    output [31:0] res
);
    wire alu_add;
    wire alu_sub;
    wire alu_and;
    wire alu_or;
    wire alu_xor;
    wire alu_sltu;
    wire alu_slt;
    wire alu_srl;
    wire alu_sll;
    wire alu_sra;

    wire [31:0] add_sub_res;
    wire [31:0] and_res;
    wire [31:0] or_res;
    wire [31:0] xor_res;
    wire [31:0] sltu_res;
    wire [31:0] slt_res;
    wire [31:0] sll_res;
    wire [31:0] sr_res;
    wire [63:0] sr64_res;

    // control code
    assign alu_add = opcode[0];
    assign alu_sub = opcode[1];
    assign alu_and = opcode[2];
    assign alu_or  = opcode[3];
    assign alu_xor = opcode[4];
    assign alu_sltu = opcode[5];
    assign alu_srl = opcode[6];
    assign alu_sll = opcode[7];
    assign alu_sra = opcode[8];
    assign alu_slt = opcode[9];

    // 32-bit adder
    wire [31:0] adder_a;
    wire [31:0] adder_b;
    wire adder_cin;
    wire [31:0] adder_res;
    wire adder_cout;

    assign adder_a = src1;
    assign adder_b = (alu_sub | alu_sltu | alu_slt) ? ~src2 : src2;
    assign adder_cin = (alu_sub | alu_sltu | alu_slt) ? 1'b1 : 1'b0;
    assign {adder_cout, adder_res} = adder_a + adder_b + {32'b0, adder_cin};

    assign add_sub_res = adder_res;

    // logic operate
    assign and_res = src1 & src2;
    assign or_res = src1 | src2;
    assign xor_res = src1 ^ src2;

    // slt
    assign slt_res[31:1] = 31'b0;
    assign slt_res[0] = (src1[31] & ~src2[31]) |
                        ((src1[31] ~^ src2[31]) & adder_res[31]);

    // sltu
    assign sltu_res[31:1] = 31'b0;
    assign sltu_res[0] = ~adder_cout;

    // shift
    assign sr64_res = {{32{alu_sra & src1[31]}}, src1[31:0]} >> src2[4:0];
    assign sr_res   = sr64_res[31:0];

    assign sll_res = src1 << src2[4:0];

    // final result mux
    assign res = ({32{alu_add|alu_sub}} & add_sub_res)
                  | ({32{alu_slt      }} & slt_res)
                  | ({32{alu_sltu      }} & sltu_res)
                  | ({32{alu_and       }} & and_res)
                  | ({32{alu_or        }} & or_res)
                  | ({32{alu_xor       }} & xor_res)
                  | ({32{alu_sll       }} & sll_res)
                  | ({32{alu_srl|alu_sra}} & sr_res);

endmodule