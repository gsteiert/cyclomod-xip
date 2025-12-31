module xip (
    input wire clk,
    input wire sck,
    input wire csn,
    inout wire [3:0] d,
    output wire led
);

localparam IDLE  = 0;
localparam CMD   = 1;
localparam ADD5  = 2;
localparam ADD4  = 3;
localparam ADD3  = 4;
localparam ADD2  = 5;
localparam ADD1  = 6;
localparam ADD0  = 7;
localparam WAIT1 = 8;
localparam WAIT0 = 9;
localparam DATA1 = 10;
localparam DATA0 = 11;

    reg [15:0] state, next_state;
    reg wrcmd, wen, den;
    reg [23:0] addr;
    reg [3:0] wdatah;
    reg [3:0] dout;
	reg q = 1'b0;
    reg [15:0] count = 0;

    wire [7:0] rdata, rdatb;

assign d = (den) ? dout : 4'hZ;

assign led = (q)? 1'b0 : 1'bZ;

always @(posedge clk) begin
    count <= count + 1;
    q <= (rdatb > count[15:8]);
end

always @(posedge sck or posedge csn) begin
    if (csn) state <= (1<<IDLE);
    else state <= next_state;
end

always @(posedge sck or posedge csn) begin
    if (csn) begin
        wrcmd <= 1'b0;
        wen <= 1'b0;
        addr <= 24'h000000;
        wdatah <= 4'h0;
        dout <= 4'h00;
    end else begin
        if (state[CMD]) wrcmd <= ~d[0];
        if (state[ADD5]) addr[23:20] <= d;
        if (state[ADD4]) addr[19:16] <= d;
        if (state[ADD3]) addr[15:12] <= d;
        if (state[ADD2]) addr[11:8] <= d;
        if (state[ADD1]) addr[7:4] <= d;
        if (state[ADD0]) addr[3:0] <= d;
        if (state[WAIT0] || state[DATA0]) begin
            addr <= addr + 24'h1;
            dout <= rdata[7:4];
        end
        if (state[DATA1]) begin
            wdatah <= d;
            dout <= rdata[3:0];
        end
        if (wrcmd) begin
            wen <= state[DATA1];
            den <= 1'b0;
        end else begin
            wen <= 1'b0;
            den <= (state[WAIT0] || state[DATA1] || state[DATA0]);
        end
    end
end

always @(*) begin
    next_state = state;
    case (1'b1)
        state[IDLE]: begin
            if (d == 4'h0) next_state = (1 << CMD);
        end
        state[CMD]: begin
            if (d[3:1] == 3'h1) next_state = (1 << ADD5);
        end
        state[ADD5]: begin
            next_state = (1 << ADD4);
        end
        state[ADD4]: begin
            next_state = (1 << ADD3);
        end
        state[ADD3]: begin
            next_state = (1 << ADD2);
        end
        state[ADD2]: begin
            next_state = (1 << ADD1);
        end
        state[ADD1]: begin
            next_state = (1 << ADD0);
        end
        state[ADD0]: begin
            next_state = (wrcmd) ? (1 << DATA1) : (1 << WAIT1);
        end
        state[WAIT1]: begin
            next_state = (1 << WAIT0);
        end
        state[WAIT0]: begin
            next_state = (1 << DATA1);
        end
        state[DATA1]: begin
            next_state = (1 << DATA0);
        end
        state[DATA0]: begin
            next_state = (1 << DATA1);
        end
        default: next_state = (1<<IDLE);
    endcase
end


dpram	xip_dpram (
	.address_a ( addr[14:0] ),
	.address_b ( 15'h0008 ),
	.clock_a ( sck ),
	.clock_b ( clk ),
	.data_a ( {1'b1, wdatah, d} ),
	.data_b ( {1'b0, 8'hA5} ),
	.wren_a ( wen ),
	.wren_b ( 1'b0 ),
	.q_a ( rdata ),
	.q_b ( rdatb )
	);


endmodule