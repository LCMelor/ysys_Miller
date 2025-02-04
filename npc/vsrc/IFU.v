module IFU (
    input clk,
    input rst,
    // to mem
    output [31:0] fetch_PC,
    // to & from IDU
    input jump,
    input [31:0] jump_addr,
    output [31:0] pc
);
    reg [31:0] PC;
    wire [31:0] seq_pc;
    wire [31:0] next_pc;

    assign seq_pc = PC + 32'h4;
    assign fetch_PC = next_pc;
    assign next_pc = jump ? jump_addr : seq_pc;
    assign pc = PC;

    always @(posedge clk) begin
        if (rst) begin
            PC <= 32'h7ffffffc;
        end else begin
            PC <= next_pc;
        end 
    end
    
endmodule