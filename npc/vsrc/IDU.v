module IDU (
        input clk,
        input rst,
        // to & from IFU
        input [31:0] inst,
        input [31:0] inst_pc,
        output jump,
        output [31:0] jump_addr,
        // to & from EXU
        output alu_opcode,
        output [31:0] alu_src1,
        output [31:0] alu_src2,
        input [31:0] alu_res,
        // to stop sim & judge status
        output stop,
        output [31:0] ret_value,
        // DPI-C
        output [31:0] regs [31:0]
    );
    // decode
    wire [6:0] opcode;
    wire [2:0] fun;
    wire [4:0] rs1;
    wire [4:0] rd;
    wire [11:0] imm;
    wire [31:0] I_imm;
    wire [31:0] U_imm;
    wire [31:0] J_imm;

    // control
    wire inst_I_type;
    wire inst_J_type;
    wire inst_auipc;
    wire inst_lui;
    wire inst_jalr;
    wire inst_ebreak;

    wire [31:0] seq_pc;

    // regfile
    wire rf_wen;
    wire [4:0]  rf_waddr;
    wire [31:0] rf_wdata;
    wire [4:0]  rf_raddr;
    wire [31:0] rf_rdata;

    // decode
    assign opcode = inst[6:0];
    assign fun = inst[14:12];
    assign rs1 = inst[19:15];
    assign rd = inst[11:7];
    assign imm = inst[31:20];
    assign I_imm = {{20{inst[31]}},inst[31:20]};
    assign U_imm = {inst[31:12],12'b0};
    assign J_imm = {{11{inst[31]}},inst[31],inst[19:12],inst[20],inst[30:21], 1'b0};

    // control
    assign inst_I_type = opcode == 7'b0010011 && fun == 3'b000;
    assign inst_J_type = opcode == 7'b1101111;
    assign inst_auipc = opcode == 7'b0010111;
    assign inst_lui = opcode == 7'b0110111;
    assign inst_jalr = opcode == 7'b1100111 && fun == 3'b000;
    assign inst_ebreak = opcode == 7'b1110011 && fun == 3'b000 && rd == 5'b00000 && rs1 == 5'b00000 && imm == 12'b000000000001;

    assign seq_pc = inst_pc + 32'h4;

    // to IFU
    assign jump = inst_J_type || inst_jalr;
    assign jump_addr = inst_jalr ? rf_rdata + I_imm : inst_pc + J_imm;

    // alu
    assign alu_src1 = {32{inst_I_type}} & rf_rdata |
                    {32{inst_auipc}} & inst_pc |
                    {32{inst_lui}} & 32'h0;
    assign alu_src2 = {32{inst_I_type}} & I_imm |
                    {32{inst_auipc | inst_lui}} & U_imm;
    assign alu_opcode = inst_I_type || inst_auipc || inst_lui || inst_jalr || inst_J_type;

    // regfile
    assign rf_raddr = rs1;
    assign rf_waddr = rd;
    assign rf_wen = inst_I_type || inst_auipc || inst_lui || inst_jalr || inst_J_type;
    assign rf_wdata = (inst_jalr | inst_J_type) ? seq_pc : alu_res;
    register u_register(
                .clk   (clk   ),
                .rst   (rst   ),
                .wen   (rf_wen   ),
                .waddr (rf_waddr ),
                .wdata (rf_wdata ),
                .raddr (rf_raddr ),
                .rdata (rf_rdata ),
                .ret_value(ret_value),
                .regs (regs)
            );

    assign stop = inst_ebreak;
endmodule
