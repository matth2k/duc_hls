`default_nettype none
`timescale 1 ns / 1 ps

module hls_bridge #(parameter integer DATA_WIDTH = 32,
                    parameter integer DATA_ADDR_WIDTH = 32)
                   (input wire clk,
                    input wire [DATA_ADDR_WIDTH-1:0] io_bus_cmd_payload_address,        // read or write address
                    input wire [DATA_WIDTH-1:0] io_bus_cmd_payload_data,                // write data
                    input wire [3:0] io_bus_cmd_payload_mask,                           // byte addressable write?
                    input wire io_bus_cmd_payload_write,                                // we
                    input wire io_bus_cmd_payload_uncached,
                    input wire [2:0] io_bus_cmd_payload_size,
                    input wire io_bus_cmd_payload_last,
                    input wire io_bus_cmd_valid,                                        // follows cmd_fire in almost every case
                    input wire rst,                                                     // reset
                    output wire io_bus_cmd_ready,                                       // am I okay to receive more requests? 1 initally
                    output wire [DATA_WIDTH-1:0] io_bus_rsp_payload_data,               // sent read data
                    output wire io_bus_rsp_payload_last,
                    output wire io_bus_rsp_valid,                                       // response data is on the bus
                    input wire [DATA_WIDTH-1:0] io_bus_rsp_payload_data_V_dout,
                    input wire io_bus_rsp_payload_data_V_empty_n,
                    output wire io_bus_rsp_payload_data_V_read,
                    input wire io_bus_rsp_payload_last_V_dout,
                    input wire io_bus_rsp_payload_last_V_empty_n,
                    output wire io_bus_rsp_payload_last_V_read,
                    output wire [DATA_ADDR_WIDTH-1:0] io_bus_cmd_payload_address_V_din,
                    input wire io_bus_cmd_payload_address_V_full_n,
                    output wire io_bus_cmd_payload_address_V_write,
                    output wire [DATA_WIDTH-1:0] io_bus_cmd_payload_data_V_din,
                    input wire io_bus_cmd_payload_data_V_full_n,
                    output wire io_bus_cmd_payload_data_V_write,
                    output wire [3:0] io_bus_cmd_payload_mask_V_din,
                    input wire io_bus_cmd_payload_mask_V_full_n,
                    output wire io_bus_cmd_payload_mask_V_write,
                    output wire io_bus_cmd_payload_write_V_din,
                    input wire io_bus_cmd_payload_write_V_full_n,
                    output wire io_bus_cmd_payload_write_V_write,
                    output wire io_bus_cmd_payload_uncached_V_din,
                    input wire io_bus_cmd_payload_uncached_V_full_n,
                    output wire io_bus_cmd_payload_uncached_V_write,
                    output wire [2:0] io_bus_cmd_payload_size_V_din,
                    input wire io_bus_cmd_payload_size_V_full_n,
                    output wire io_bus_cmd_payload_size_V_write,
                    output wire io_bus_cmd_payload_last_V_din,
                    input wire io_bus_cmd_payload_last_V_full_n,
                    output wire io_bus_cmd_payload_last_V_write);
    
    wire hls_full           = ~io_bus_cmd_payload_address_V_full_n | ~io_bus_cmd_payload_data_V_full_n | ~io_bus_cmd_payload_mask_V_full_n | ~io_bus_cmd_payload_write_V_full_n | ~io_bus_cmd_payload_uncached_V_full_n | ~io_bus_cmd_payload_size_V_full_n | ~io_bus_cmd_payload_last_V_full_n;
    assign io_bus_cmd_ready = ~hls_full & ~rst;
    
    wire hls_write                             = io_bus_cmd_valid & ~rst;
    assign io_bus_cmd_payload_address_V_write  = hls_write;
    assign io_bus_cmd_payload_data_V_write     = hls_write;
    assign io_bus_cmd_payload_mask_V_write     = hls_write;
    assign io_bus_cmd_payload_write_V_write    = hls_write;
    assign io_bus_cmd_payload_uncached_V_write = hls_write;
    assign io_bus_cmd_payload_size_V_write     = hls_write;
    assign io_bus_cmd_payload_last_V_write     = hls_write;
    
    assign io_bus_cmd_payload_write_V_din    = io_bus_cmd_payload_write;
    assign io_bus_cmd_payload_mask_V_din     = io_bus_cmd_payload_mask;
    assign io_bus_cmd_payload_data_V_din     = io_bus_cmd_payload_data;
    assign io_bus_cmd_payload_address_V_din  = io_bus_cmd_payload_address;
    assign io_bus_cmd_payload_uncached_V_din = io_bus_cmd_payload_uncached;
    assign io_bus_cmd_payload_size_V_din     = io_bus_cmd_payload_size;
    assign io_bus_cmd_payload_last_V_din     = io_bus_cmd_payload_last;
    
    wire hls_empty                        = ~io_bus_rsp_payload_data_V_empty_n | ~io_bus_rsp_payload_last_V_empty_n;
    assign io_bus_rsp_payload_data_V_read = ~hls_empty & ~rst;
    assign io_bus_rsp_payload_last_V_read = ~hls_empty & ~rst;
    
    assign io_bus_rsp_valid        = ~hls_empty & ~rst;
    assign io_bus_rsp_payload_data = io_bus_rsp_payload_data_V_dout;
    assign io_bus_rsp_payload_last = io_bus_rsp_payload_last_V_dout;
    
endmodule
    
