#include <hls_stream.h>
#include <ap_int.h>
#include "duc.h"

void to_network(hls::stream<ap_uint<RV_ADDR_WIDTH>> &in_cmd_payload_address,
                hls::stream<ap_uint<RV_DATA_WIDTH>> &in_cmd_payload_data,
                hls::stream<ap_uint<4>> &in_cmd_payload_mask,
                hls::stream<ap_uint<3>> &in_cmd_payload_size,
                hls::stream<bool> &in_cmd_payload_write,
                hls::stream<bool> &in_cmd_payload_uncached,
                hls::stream<bool> &in_cmd_payload_last,
                hls::stream<memt_t> &out_serialized)
{
    static memt_t transaction_id = 0;

    // Addr and wr
    ap_uint<RV_ADDR_WIDTH> base_addr = in_cmd_payload_address.read();
    bool base_write = in_cmd_payload_write.read();
    
    // Burst size and last
    unsigned int burst_len = 0;
    if (base_write) {
        while (!in_cmd_payload_last.read()) {
            burst_len++;
            in_cmd_payload_address.read();
            in_cmd_payload_write.read();
        }
    } else {
        burst_len = in_cmd_payload_size.read();
        in_cmd_payload_last.read();
    }

    // Write the output to network
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
        out_serialized.write(in_cmd_payload_mask.read());
        // Assume all the masks are the same
        for (unsigned int i = 0; i < burst_len; i++)
        {
            out_serialized.write(in_cmd_payload_data.read());
        }
        // drain
        for (unsigned int i = 0; i < burst_len; i++)
        {
            in_cmd_payload_uncached.read();
            in_cmd_payload_size.read();
        }
        for (unsigned int i = 0; i < burst_len-1; i++)
        {
            in_cmd_payload_mask.read();
        }
    }
    else
    {
        // drain
        in_cmd_payload_mask.read();
        in_cmd_payload_data.read();
        in_cmd_payload_uncached.read();
    }

}

void from_network(hls::stream<memt_t> &in_serialized,
                  hls::stream<ap_uint<RV_DATA_WIDTH>> &out_rsp_payload_data,
                  hls::stream<bool> &out_rsp_payload_last)
{
    memt_t op = in_serialized.read();

    if (op == WRITE_RESP)
    {
        memt_t transaction_resp = in_serialized.read();
        out_rsp_payload_data.write(0);
        out_rsp_payload_last.write(false);
    }
    else if (op == READ_RESP)
    {
        // TODO: the read side
        memt_t transaction_resp = in_serialized.read();
        addr_t base_addr = in_serialized.read();
        memt_t length = in_serialized.read();

        for (unsigned int i = 0; i < length; i++)
        {
            out_rsp_payload_data.write(in_serialized.read());
            out_rsp_payload_last.write(i == length - 1);
        }
    }
}

void mem2stream(hls::stream<ap_uint<RV_ADDR_WIDTH>> &in_cmd_payload_address,
                hls::stream<ap_uint<RV_DATA_WIDTH>> &in_cmd_payload_data,
                hls::stream<ap_uint<4>> &in_cmd_payload_mask,
                hls::stream<ap_uint<3>> &in_cmd_payload_size,
                hls::stream<bool> &in_cmd_payload_write,
                hls::stream<bool> &in_cmd_payload_uncached,
                hls::stream<bool> &in_cmd_payload_last,
                hls::stream<memt_t> &in_serialized,
                hls::stream<ap_uint<RV_DATA_WIDTH>> &out_rsp_payload_data,
                hls::stream<bool> &out_rsp_payload_last,
                hls::stream<memt_t> &out_serialized)
{
#pragma HLS interface ap_ctrl_none port = return
    while (1)
    {
        if (!in_cmd_payload_address.empty() && !in_cmd_payload_write.empty())
            to_network(in_cmd_payload_address, in_cmd_payload_data, in_cmd_payload_mask, in_cmd_payload_size, in_cmd_payload_write, in_cmd_payload_uncached, in_cmd_payload_last, out_serialized);

        if (!in_serialized.empty())
            from_network(in_serialized, out_rsp_payload_data, out_rsp_payload_last);
    }
}