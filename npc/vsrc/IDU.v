module IDU (
        input clk,
        input rst,
        // to & from IFU
        input [31:0] inst,
        input [31:0] inst_pc,
        output jump,
        output [31:0] jump_addr,
        // to & from EXU
        output [9:0] alu_opcode,
        output [31:0] alu_src1,
        output [31:0] alu_src2,
        input [31:0] alu_res,
        // to & from MEM
        input [31:0] mem_addr,
        input [31:0] mem_rdata,
        output [31:0] mem_wdata,
        output [7:0] mem_wmask,
        output mem_wen,
        output mem_valid,
        // from EXU
        input [31:0] b_target,
        // to stop sim & judge status
        output stop,
        output [31:0] ret_value,
        // DPI-C
        output [31:0] regs [31:0]
    );
    // decode
    wire [6:0] opcode;
    wire [2:0] fun3;
    wire [6:0] fun7;
    wire [4:0] rs1;
    wire [4:0] rs2;
    wire [4:0] rd;
    wire [11:0] imm;
    wire [31:0] I_imm;
    wire [31:0] U_imm;
    wire [31:0] J_imm;
    wire [31:0] S_imm;
    wire [31:0] B_imm;

    // control
    wire inst_I_type;
    wire inst_load;
    wire inst_store;
    wire inst_J_type;
    wire inst_R_type;
    wire inst_B_type;
    wire inst_auipc;
    wire inst_lui;
    wire inst_jalr;
    wire inst_ebreak;

    wire inst_add;
    wire inst_sub;
    wire inst_sll;
    wire inst_sltu;
    wire inst_slt;
    wire inst_xor;
    wire inst_srl;
    wire inst_sra;
    wire inst_or;
    wire inst_and;

    wire inst_lb;
    wire inst_lh;
    wire inst_lw;
    wire inst_lbu;
    wire inst_lhu;
    wire inst_sb;
    wire inst_sh;
    wire inst_sw;

    wire inst_beq;
    wire inst_bne;
    wire inst_blt;
    wire inst_bge;
    wire inst_bltu;
    wire inst_bgeu;

    wire inst_addi;
    wire inst_slti;
    wire inst_sltiu;
    wire inst_xori;
    wire inst_ori;
    wire inst_andi;
    wire inst_slli;
    wire inst_srli;
    wire inst_srai;

    wire [31:0] seq_pc;

    // regfile
    wire rf_wen;
    wire [4:0]  rf_waddr;
    wire [31:0] rf_wdata;
    wire [4:0]  rf_raddr1;
    wire [4:0]  rf_raddr2;
    wire [31:0] rf_rdata1;
    wire [31:0] rf_rdata2;

    // decode
    assign opcode = inst[6:0];
    assign fun3 = inst[14:12];
    assign fun7 = inst[31:25];
    assign rs1 = inst[19:15];
    assign rs2 = inst[24:20];
    assign rd = inst[11:7];
    assign imm = inst[31:20];
    assign I_imm = {{20{inst[31]}},inst[31:20]};
    assign U_imm = {inst[31:12],12'b0};
    assign J_imm = {{11{inst[31]}},inst[31],inst[19:12],inst[20],inst[30:21], 1'b0};
    assign S_imm = {{20{inst[31]}}, inst[31:25], inst[11:7]};
    assign B_imm = {{19{inst[31]}}, inst[31], inst[7], inst[30:25], inst[11:8], 1'b0};

    // control
    assign inst_I_type = opcode == 7'b0010011;
    assign inst_J_type = opcode == 7'b1101111;
    assign inst_R_type = opcode == 7'b0110011;
    assign inst_load = opcode == 7'b0000011;
    assign inst_store = opcode == 7'b0100011;
    assign inst_B_type = opcode == 7'b1100011;
    assign inst_auipc = opcode == 7'b0010111;
    assign inst_lui = opcode == 7'b0110111;
    assign inst_jalr = opcode == 7'b1100111 && fun3 == 3'b000;
    assign inst_ebreak = opcode == 7'b1110011 && fun3 == 3'b000 && rd == 5'b00000 && rs1 == 5'b00000 && imm == 12'b000000000001;

    assign seq_pc = inst_pc + 32'h4;

    // R type
    assign inst_add = inst_R_type & fun3 == 3'b000 & fun7 == 7'b0;
    assign inst_sub = inst_R_type & fun3 == 3'b0 & fun7 == 7'b0100000;
    assign inst_sll = inst_R_type & fun3 == 3'b001 & fun7 == 7'b0;
    assign inst_slt = inst_R_type & fun3 == 3'b010 & fun7 == 7'b0;
    assign inst_sltu = inst_R_type & fun3 == 3'b011 & fun7 == 7'b0;
    assign inst_xor = inst_R_type & fun3 == 3'b100 & fun7 == 7'b0;
    assign inst_srl = inst_R_type & fun3 == 3'b101 & fun7 == 7'b0;
    assign inst_sra = inst_R_type & fun3 == 3'b101 & fun7 == 7'b0100000;
    assign inst_or = inst_R_type & fun3 == 3'b110 & fun7 == 7'b0;
    assign inst_and = inst_R_type & fun3 == 3'b111 & fun7 == 7'b0;

    // I type
    assign inst_addi = inst_I_type & fun3 == 3'b000;
    assign inst_slti = inst_I_type & fun3 == 3'b010;
    assign inst_sltiu = inst_I_type & fun3 == 3'b011;
    assign inst_xori = inst_I_type & fun3 == 3'b100;
    assign inst_ori = inst_I_type & fun3 == 3'b110;
    assign inst_andi = inst_I_type & fun3 == 3'b111;
    assign inst_slli = inst_I_type & fun3 == 3'b001 & fun7 == 7'b0000000;
    assign inst_srli = inst_I_type & fun3 == 3'b101 & fun7 == 7'b0000000;
    assign inst_srai = inst_I_type & fun3 == 3'b101 & fun7 == 7'b0100000;

    wire [4:0] shamt;
    wire inst_I_shift;
    assign shamt = rs2;
    assign inst_I_shift = inst_slli | inst_srli | inst_srai;

    // load & store 
    assign inst_lb = inst_load & fun3 == 3'b0;
    assign inst_lh = inst_load & fun3 == 3'b001;
    assign inst_lw = inst_load & fun3 == 3'b010;
    assign inst_lbu = inst_load & fun3 == 3'b100;
    assign inst_lhu = inst_load & fun3 == 3'b101;
    assign inst_sb = inst_store & fun3 == 3'b0;
    assign inst_sh = inst_store & fun3 == 3'b001;
    assign inst_sw = inst_store & fun3 == 3'b010;

    wire [31:0] lw_data;
    wire [31:0] lh_data;
    wire [31:0] lhu_data;
    wire [31:0] lb_data;
    wire [31:0] lbu_data;
    wire [31:0] load_data;
    wire [31:0] sw_data;
    wire [31:0] sh_data;
    wire [31:0] sb_data;

    assign mem_valid = inst_load | inst_store;
    assign mem_wen = inst_store;
    assign mem_wmask = ({8{inst_lw | inst_sw}} & 8'b00001111) |
                ({8{inst_lh| inst_sh | inst_lhu}} & 8'b00000011) |
                ({8{inst_lb | inst_sb | inst_lbu}} & 8'b00000001);

    assign sw_data = rf_rdata2;
    assign sh_data = {16'b0, rf_rdata2[15:0]};
    assign sb_data = {24'b0, rf_rdata2[7:0]};
    assign lw_data = mem_rdata[31:0];
    assign lh_data = mem_addr[1] ? {{16{mem_rdata[31]}}, mem_rdata[31:16]} :
     {{16{mem_rdata[15]}}, mem_rdata[15:0]};
    assign lhu_data = mem_addr[1] ? {16'b0, mem_rdata[31:16]} :
     {16'b0, mem_rdata[15:0]};
    assign lb_data = mem_addr[1:0] == 2'b00 ? {{24{mem_rdata[7]}}, mem_rdata[7:0]} :
                    mem_addr[1:0] == 2'b01 ? {{24{mem_rdata[15]}}, mem_rdata[15:8]} :
                    mem_addr[1:0] == 2'b10 ? {{24{mem_rdata[23]}}, mem_rdata[23:16]} :
                    {{24{mem_rdata[31]}}, mem_rdata[31:24]};
    assign lbu_data = mem_addr[1:0] == 2'b00 ? {24'b0, mem_rdata[7:0]} :
                    mem_addr[1:0] == 2'b01 ? {24'b0, mem_rdata[15:8]} :
                    mem_addr[1:0] == 2'b10 ? {24'b0, mem_rdata[23:16]} :
                    {24'b0, mem_rdata[31:24]};

    assign mem_wdata = ({32{inst_sw}} & sw_data) |
                        ({32{inst_sh}} & sh_data) |
                        ({32{inst_sb}} & sb_data);

    assign load_data = ({32{inst_lw}} & lw_data) |
                        ({32{inst_lh}} & lh_data) |
                        ({32{inst_lhu}} & lhu_data) |
                        ({32{inst_lb}} & lb_data) |
                        ({32{inst_lbu}} & lbu_data);

    // B type
    assign inst_beq = inst_B_type & fun3 == 3'b000;
    assign inst_bne = inst_B_type & fun3 == 3'b001;
    assign inst_blt = inst_B_type & fun3 == 3'b100;
    assign inst_bge = inst_B_type & fun3 == 3'b101;
    assign inst_bltu = inst_B_type & fun3 == 3'b110;
    assign inst_bgeu = inst_B_type & fun3 == 3'b111;

    wire beq_t;
    wire bne_t;
    wire blt_t;
    wire bge_t;
    wire bltu_t;
    wire bgeu_t;
    wire [31:0] b_subres;
    wire b_cout;
    wire b_jump;

    assign {b_cout, b_subres} = rf_rdata1 + {1'b0, ~rf_rdata2} + 33'b1;
    assign beq_t = rf_rdata1 == rf_rdata2;
    assign bne_t = ~beq_t;
    assign blt_t = (rf_rdata1[31] & ~rf_rdata2[31]) |
                    ((rf_rdata1[31] ~^ rf_rdata2[31]) & b_subres[31]);
    assign bge_t = ~blt_t;
    assign bltu_t = ~b_cout;
    assign bgeu_t = ~bltu_t;

    assign b_jump = (inst_beq & beq_t) |
                    (inst_bne & bne_t) |
                    (inst_blt & blt_t) |
                    (inst_bge & bge_t) |
                    (inst_bltu & bltu_t) |
                    (inst_bgeu & bgeu_t);

    // to IFU
    assign jump = inst_J_type || inst_jalr || b_jump;
    assign jump_addr = inst_jalr ? rf_rdata1 + I_imm : 
                        b_jump ? b_target : inst_pc + J_imm;

    // alu
    assign alu_src1 = {32{inst_I_type | inst_R_type | inst_load | inst_store}} & rf_rdata1 |
                    {32{inst_auipc | inst_B_type}} & inst_pc |
                    {32{inst_lui}} & 32'h0;
    assign alu_src2 = {32{inst_I_type | inst_load & ~inst_I_shift}} & I_imm |
                    {32{inst_I_shift}} & {27'b0, shamt} |
                    {32{inst_R_type}} & rf_rdata2 |
                    {32{inst_auipc | inst_lui}} & U_imm |
                    {32{inst_store}} & S_imm |
                    {32{inst_B_type}} & B_imm;
    assign alu_opcode[0] = inst_addi || inst_auipc || inst_lui || inst_jalr || inst_J_type || inst_add | inst_store | inst_load | inst_B_type;
    assign alu_opcode[1] = inst_sub;
    assign alu_opcode[2] = inst_and | inst_andi;
    assign alu_opcode[3] = inst_or | inst_ori;
    assign alu_opcode[4] = inst_xor | inst_xori;
    assign alu_opcode[5] = inst_sltu | inst_sltiu;
    assign alu_opcode[6] = inst_srl | inst_srli;
    assign alu_opcode[7] = inst_sll | inst_slli;
    assign alu_opcode[8] = inst_sra | inst_srai;
    assign alu_opcode[9] = inst_slt | inst_slti;

    // regfile
    assign rf_raddr1 = rs1;
    assign rf_raddr2 = rs2;
    assign rf_waddr = rd;
    assign rf_wen = inst_I_type || inst_auipc || inst_lui || inst_jalr || inst_J_type || inst_R_type || inst_load;
    assign rf_wdata = (inst_jalr | inst_J_type) ? seq_pc : 
                        inst_load ? load_data : alu_res;
    register u_register(
                .clk   (clk   ),
                .rst   (rst   ),
                .wen   (rf_wen   ),
                .waddr (rf_waddr ),
                .wdata (rf_wdata ),
                .raddr1 (rf_raddr1 ),
                .raddr2(rf_raddr2),
                .rdata1 (rf_rdata1 ),
                .rdata2(rf_rdata2),
                .ret_value(ret_value),
                .regs (regs)
            );

    assign stop = inst_ebreak;
endmodule
