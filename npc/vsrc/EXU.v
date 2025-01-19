module EXU (
    input opcode,
    input [31:0] src1,
    input [31:0] src2,
    output [31:0] res
);
    assign res = opcode ? src1 + src2 : 32'h0;
endmodule