module register (
    input clk,
    input rst,
    input wen,
    input [4:0] waddr,
    input [31:0] wdata,
    input [4:0] raddr1,
    input [4:0] raddr2,
    output [31:0] rdata1,
    output [31:0] rdata2,
    output [31:0] ret_value,
    // DPI-C
    output [31:0] regs [31:0]
);
    reg [31:0] rf [31:0];

    assign regs = rf;

    always @(posedge clk) begin
        if(rst) begin
            for(integer i=0; i<32; i=i+1) begin
                rf[i] <= 32'h0;
            end
        end
        else begin
            if(wen) begin
                rf[waddr] <= wdata;
            end
        end
    end

    assign rdata1 = raddr1 == 0 ? 0 : rf[raddr1];
    assign rdata2 = raddr2 == 0 ? 0 : rf[raddr2];

    assign ret_value = rf[10];
    
endmodule