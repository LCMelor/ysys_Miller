module core(
  input clk,
  input rst,
  // to & from mem
  input [31:0] inst,
  output [31:0] pc
);

  wire  alu_opcode;
  wire [31:0] alu_src1;
  wire [31:0] alu_src2;
  wire [31:0] alu_res;
  

  IFU u_IFU(
      .clk      (clk      ),
      .rst      (rst      ),
      .fetch_PC (pc )
  );
  
  IDU u_IDU(
      .clk        (clk        ),
      .rst        (rst        ),
      .inst       (inst       ),
      .alu_opcode (alu_opcode ),
      .alu_src1   (alu_src1   ),
      .alu_src2   (alu_src2   ),
      .alu_res    (alu_res    )
  );
  
  EXU u_EXU(
      .opcode (alu_opcode ),
      .src1   (alu_src1   ),
      .src2   (alu_src2   ),
      .res    (alu_res    )
  );
  
endmodule
