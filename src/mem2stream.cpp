#include <hls_stream.h>
#include <ap_int.h>
#include "duc.h"

bool to_network(hls::stream<ap_uint<RV_ADDR_WIDTH>> &in_cmd_payload_address,
                hls::stream<ap_uint<RV_DATA_WIDTH>> &in_cmd_payload_data,
                hls::stream<ap_uint<4>> &in_cmd_payload_mask,
                hls::stream<bool> &in_cmd_payload_write,
                hls::stream<memt_t> &out_serialized)
{
    static memt_t transaction_id = 0;
    static bool next_occupied = false;
    static ap_uint<RV_ADDR_WIDTH> next_addr = 0;
    static bool next_write = false;
    ap_uint<RV_ADDR_WIDTH> base_addr = next_occupied ? next_addr : in_cmd_payload_address.read();
    bool base_write = next_occupied ? next_write : in_cmd_payload_write.read();
    next_occupied = false;

    unsigned int burst_len = 1;
    while (!in_cmd_payload_address.empty() && !in_cmd_payload_write.empty() && burst_len < MAX_BURST_LEN)
    {

        next_addr = in_cmd_payload_address.read();
        next_write = in_cmd_payload_write.read();
        bool increment = (base_write == next_write) && (next_addr == (base_addr + burst_len));
        if (increment)
        {
            burst_len++;
        }
        else
        {
            next_occupied = true;
            break;
        }
    }

    // Write the stream to network
    if (base_write)
    {
        out_serialized.write(WRITE_REQ);
    }
    else
    {
        out_serialized.write(READ_REQ);
    }
    out_serialized.write(transaction_id++);
    out_serialized.write(base_addr);
    out_serialized.write(burst_len);
    // write contents and draining FIFOs
    if (base_write)
    {
        for (unsigned int i = 0; i < burst_len; i++)
        {
            out_serialized.write(in_cmd_payload_data.read());
        }
        // drain
        for (unsigned int i = 0; i < burst_len; i++)
        {
            // TODO: Ignore mask, otherwise how do we serialize a mask?
            in_cmd_payload_mask.read();
        }
    }
    else
    {
        // drain
        for (unsigned int i = 0; i < burst_len; i++)
        {
            in_cmd_payload_data.read();
            in_cmd_payload_mask.read();
        }
    }

    return next_occupied;
}

void from_network(hls::stream<memt_t> &in_serialized,
                  hls::stream<bool> &out_rsp_valid,
                  hls::stream<ap_uint<RV_DATA_WIDTH>> &out_rsp_payload_data)
{
    memt_t op = in_serialized.read();
    
    if (op == WRITE_RESP)
    {
        memt_t transaction_resp = in_serialized.read();
        out_rsp_valid.write(true);
        out_rsp_payload_data.write(0);
    }
    else if (op == READ_RESP)
    {
        // TODO: the read side
        memt_t transaction_resp = in_serialized.read();
        addr_t base_addr = in_serialized.read();
	    memt_t length = in_serialized.read();
        
        for (unsigned int i = 0; i < length; i++) {
            out_rsp_valid.write(true);
            out_rsp_payload_data.write(in_serialized.read());
        }
    }
}

void mem2stream(hls::stream<ap_uint<RV_ADDR_WIDTH>> &in_cmd_payload_address,
                hls::stream<ap_uint<RV_DATA_WIDTH>> &in_cmd_payload_data,
                hls::stream<ap_uint<4>> &in_cmd_payload_mask,
                hls::stream<bool> &in_cmd_payload_write,
                hls::stream<memt_t> &in_serialized,
                hls::stream<bool> &out_rsp_valid,
                hls::stream<ap_uint<RV_DATA_WIDTH>> &out_rsp_payload_data,
                hls::stream<memt_t> &out_serialized)
{
#pragma HLS interface ap_ctrl_none port = return
    bool next_occupied = false;
    while (1)
    {
        if (next_occupied || (!in_cmd_payload_address.empty() && !in_cmd_payload_write.empty()))
            next_occupied = to_network(in_cmd_payload_address, in_cmd_payload_data, in_cmd_payload_mask, in_cmd_payload_write, out_serialized);

        if (!in_serialized.empty())
            from_network(in_serialized, out_rsp_valid, out_rsp_payload_data);
    }
}