module IDU (
        input clk,
        input rst,
        // from IFU
        input [31:0] inst,
        // to & from EXU
        output alu_opcode,
        output [31:0] alu_src1,
        output [31:0] alu_src2,
        input [31:0] alu_res
    );
    // decode
    wire [6:0] opcode;
    wire [2:0] fun;
    wire [4:0] rs1;
    wire [4:0] rd;
    wire [31:0] I_imm;

    wire inst_I;
    // wire [31:0] alu_src1;
    // wire [31:0] alu_src2;
    // wire [31:0] alu_res;
    // wire alu_opcode;

    wire rf_wen;
    wire [4:0]  rf_waddr;
    wire [31:0] rf_wdata;
    wire [4:0]  rf_raddr;
    wire [31:0] rf_rdata;


    assign opcode = inst[6:0];
    assign fun = inst[14:12];
    assign rs1 = inst[19:15];
    assign rd = inst[11:7];
    assign I_imm = {{20{inst[31]}},inst[31:20]};

    assign inst_I = opcode == 7'b0010011 && fun == 3'b000;

    assign alu_src1 = inst_I ? rf_rdata : 32'h0;
    assign alu_src2 = inst_I ? I_imm : 32'h0;
    assign alu_opcode = inst_I;

    // regfile
    assign rf_raddr = rs1;
    assign rf_waddr = rd;
    assign rf_wen = inst_I;
    assign rf_wdata = alu_res;
    regiser u_regiser(
                .clk   (clk   ),
                .rst   (rst   ),
                .wen   (rf_wen   ),
                .waddr (rf_waddr ),
                .wdata (rf_wdata ),
                .raddr (rf_raddr ),
                .rdata (rf_rdata )
            );


endmodule
