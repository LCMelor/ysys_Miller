import "DPI-C" function void stop_sim(input bit stop, input int ret_value);
module core(
  input clk,
  input rst,
  // to & from mem
  input [31:0] inst,
  output [31:0] fetch_PC
);

  wire  alu_opcode;
  wire [31:0] alu_src1;
  wire [31:0] alu_src2;
  wire [31:0] alu_res;

  // DPI-C
  wire stop;
  wire [31:0] ret_value;
  wire [31:0] regs [31:0];

  wire jump;
  wire [31:0] jump_addr;
  wire [31:0] pc;

  always @(*) begin
    stop_sim(stop, ret_value);
  end

  export "DPI-C" function get_sv_reg;
  function int get_sv_reg(int reg_id);
        if (reg_id < 0 || reg_id >= 32) return -1;
        return regs[reg_id];
  endfunction

  export "DPI-C" function jump_flag;
  function bit jump_flag();
        return jump;
  endfunction

  export "DPI-C" function jump_target;
  function int jump_target();
        return jump_addr;
  endfunction


  IFU u_IFU(
      .clk      (clk      ),
      .rst      (rst      ),
      .fetch_PC (fetch_PC ),
      .jump     (jump     ),
      .jump_addr(jump_addr),
      .pc       (pc       )
  );
  
  IDU u_IDU(
      .clk        (clk        ),
      .rst        (rst        ),
      .inst       (inst       ),
      .inst_pc    (pc         ),
      .jump       (jump       ),
      .jump_addr  (jump_addr  ),
      .alu_opcode (alu_opcode ),
      .alu_src1   (alu_src1   ),
      .alu_src2   (alu_src2   ),
      .alu_res    (alu_res    ),
      .stop       (stop       ),
      .ret_value  (ret_value  ),
      .regs       (regs       )
  );
  
  EXU u_EXU(
      .opcode (alu_opcode ),
      .src1   (alu_src1   ),
      .src2   (alu_src2   ),
      .res    (alu_res    )
  );
  
endmodule
