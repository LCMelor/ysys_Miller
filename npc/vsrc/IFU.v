module IFU (
    input clk,
    input rst,
    // to mem
    output [31:0] fetch_PC
);
    reg [31:0] PC;
    wire [31:0] seq_PC;

    assign seq_PC = PC + 32'h4;
    assign fetch_PC = PC;

    always @(posedge clk) begin
        if (rst) begin
            PC <= 32'h80000000;
        end else begin
            PC <= seq_PC;
        end 
    end
    
endmodule