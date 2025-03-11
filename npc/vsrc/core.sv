import "DPI-C" function void stop_sim(input bit stop, input int ret_value);
import "DPI-C" function int unsigned pmem_read_sim(input int paddr);
import "DPI-C" function void pmem_write(
  input int waddr, input int wdata, input byte wmask);
module core(
  input clk,
  input rst,
  // to & from mem
  input [31:0] inst,
  output [31:0] fetch_PC
);

  wire [9:0] alu_opcode;
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

  wire mem_valid;
  wire mem_ren;
  wire mem_wen;
  wire [31:0] raddr;
  wire [31:0] waddr;
  wire [7:0] mem_wmask;
  wire [31:0] mem_wdata;
  reg [31:0] mem_rdata;

  // csr
  wire [11:0] csr_addr;
  wire [31:0] csr_wdata;
  wire csr_wen;
  wire [31:0] csr_rdata;
  wire csr_ecall;
  wire csr_mret;
  wire [31:0] csr_mtvec;
  wire [31:0] csr_mepc;
  wire [31:0] csr_rdata;

  always @(*) begin
    stop_sim(stop, ret_value);
  end

  assign raddr = alu_res;
  assign waddr = alu_res;

  always @(negedge clk) begin
    if (mem_valid) begin // 有读写请求时
      if(mem_ren)
        mem_rdata <= pmem_read_sim(raddr);
      if (mem_wen) begin // 有写请求时
        pmem_write(waddr, mem_wdata, mem_wmask);
      end
    end
    else begin
      mem_rdata <= 0;
    end
  end

  export "DPI-C" function get_sv_reg;
  function int get_sv_reg(int reg_id);
        if (reg_id < 0 || reg_id >= 32) return -1;
        return regs[reg_id];
  endfunction

  export "DPI-C" function jump_flag;
  function bit jump_flag();
        return jump || csr_ecall || csr_mret;
  endfunction

  export "DPI-C" function jump_target;
  function int jump_target();
        return fetch_PC;
  endfunction


  IFU u_IFU(
      .clk      (clk      ),
      .rst      (rst      ),
      .fetch_PC (fetch_PC ),
      .jump     (jump     ),
      .jump_addr(jump_addr),
      .pc       (pc       ),
      .csr_ecall(csr_ecall),
      .csr_mret (csr_mret ),
      .csr_mtvec(csr_mtvec),
      .csr_mepc (csr_mepc )
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
      .mem_addr   (alu_res    ),
      .mem_rdata  (mem_rdata  ),
      .mem_wdata  (mem_wdata  ),
      .mem_wmask  (mem_wmask  ),
      .mem_wen    (mem_wen    ),
      .mem_valid  (mem_valid  ),
      .mem_ren    (mem_ren    ),
      .b_target   (alu_res    ),
      .csr_addr   (csr_addr   ),
      .csr_wdata  (csr_wdata  ),
      .csr_rdata  (csr_rdata ),
      .csr_wen    (csr_wen    ),
      .csr_ecall  (csr_ecall  ),
      .csr_mret   (csr_mret   ),
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

  csr u_csr(
      .clk       (clk       ),
      .rst       (rst       ),
      .csr_addr  (csr_addr  ),
      .csr_wdata (csr_wdata ),
      .csr_wen   (csr_wen   ),
      .csr_rdata (csr_rdata ),
      .pc        (pc        ),
      .csr_ecall (csr_ecall ),
      .csr_mtvec (csr_mtvec ),
      .csr_mepc  (csr_mepc  )
  );
  
  
endmodule
