module csr (
        input clk,
        input rst,
        input [11:0] csr_addr,
        input [31:0] csr_wdata,
        input csr_wen,
        output [31:0] csr_rdata,
        input [31:0] pc,
        input csr_ecall,
        output [31:0] csr_mtvec,
        output [31:0] csr_mepc
    );
    reg [31:0] csr_reg [3:0];

    always @(posedge clk) begin
        if (rst) begin
            csr_reg[0] <= 32'h180;
            csr_reg[1] <= 32'h0;
            csr_reg[2] <= 32'h0;
            csr_reg[3] <= 32'h0;
        end
        else if (csr_wen) begin
            case (csr_addr)
                12'h300:
                    csr_reg[0] <= csr_wdata; // mstatus
                12'h305:
                    csr_reg[1] <= csr_wdata; // mtvec
                12'h341:
                    csr_reg[2] <= csr_wdata; // mepc
                12'h342:
                    csr_reg[3] <= csr_wdata; // mcause
                default: begin
                    csr_reg[0] <= csr_reg[0];
                    csr_reg[1] <= csr_reg[1];
                    csr_reg[2] <= csr_reg[2];
                    csr_reg[3] <= csr_reg[3];
                end
            endcase
        end
    end

    wire mstatus_r = csr_addr == 12'h300;
    wire mtvec_r = csr_addr == 12'h305;
    wire mepc_r = csr_addr == 12'h341;
    wire mcause_r = csr_addr == 12'h342;

    assign csr_rdata = csr_reg[0] & {32{mstatus_r}} |
           csr_reg[1] & {32{mtvec_r}} |
           csr_reg[2] & {32{mepc_r}} |
           csr_reg[3] & {32{mcause_r}};

    assign csr_mtvec = csr_reg[1];
    assign csr_mepc = csr_reg[2];

    always @(posedge clk) begin
        if(csr_ecall) begin
            csr_reg[2] <= pc; // mepc
            csr_reg[3] <= 32'h00000008; // mcause
        end
    end

endmodule
