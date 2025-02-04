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
  wire stop;
  wire [31:0] ret_value;

  wire jump;
  wire [31:0] jump_addr;
  wire [31:0] pc;

  always @(*) begin
    stop_sim(stop, ret_value);
  end


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
      .ret_value  (ret_value  )
  );
  
  EXU u_EXU(
      .opcode (alu_opcode ),
      .src1   (alu_src1   ),
      .src2   (alu_src2   ),
      .res    (alu_res    )
  );
  
endmodule
