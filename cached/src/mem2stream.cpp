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
    in_cmd_payload_last.read();

    ap_uint<3> word_type = in_cmd_payload_size.read(); // 0 = byte, 1 = half word, 10 = full word
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
    out_serialized.write(1); // Only support of burst 1 for now
    out_serialized.write(word_type);
    // write contents and draining FIFOs
    if (base_write)
    {
        out_serialized.write(in_cmd_payload_mask.read());
        out_serialized.write(in_cmd_payload_data.read());
        // drain
        in_cmd_payload_uncached.read();
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
        // write's don't get responses
    }
    else if (op == READ_RESP)
    {

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
#ifdef PRFLOW
#pragma HLS interface axis register both port = in_serialized
#pragma HLS interface axis register both port = out_serialized
#endif
    while (1)
    {
        if (!in_cmd_payload_address.empty() && !in_cmd_payload_write.empty())
            to_network(in_cmd_payload_address, in_cmd_payload_data, in_cmd_payload_mask, in_cmd_payload_size, in_cmd_payload_write, in_cmd_payload_uncached, in_cmd_payload_last, out_serialized);

        if (!in_serialized.empty())
            from_network(in_serialized, out_rsp_payload_data, out_rsp_payload_last);
    }
}