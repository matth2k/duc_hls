#include <hls_stream.h>
#include <ap_int.h>
#include "duc.h"

memt_t process_write_req(memt_t *arr, hls::stream<memt_t> &in)
{
	id_t transaction = in.read();
	addr_t base_addr = in.read();
	memt_t length = in.read();
	ap_uint<3> word_type = in.read();
	ap_uint<4> mask = in.read(); // mask to apply to the input
	memt_t write_mask = 0;		 // mask to apply to DRAM
	if (word_type == 0)
	{
		write_mask = ~(0xff << ((base_addr & 3) * 8));
	}
	else if (word_type == 1)
	{
		if ((base_addr & 0x3) == 0)
			write_mask = 0xff00;
		else
			write_mask = 0x00ff;
	}
	for (unsigned int i = 0; i < length; i++)
	{
		if (word_type == 2)
			arr[(base_addr >> 2) + i] = in.read();
		else
			arr[(base_addr >> 2) + i] = (arr[(base_addr >> 2) + i] & write_mask) | (in.read() & (~write_mask)); // TODO strb apply mask
	}

	return transaction;
}

void process_read_req(memt_t *arr, hls::stream<memt_t> &in, hls::stream<memt_t> &out)
{
	addr_t base_addr = in.read();
	memt_t length = in.read();
	ap_uint<3> word_type = in.read();
	out.write(base_addr); // Unnecessary but might help with debugging
	out.write(length);
	for (unsigned int i = 0; i < length; i++)
	{
		out.write(arr[(base_addr >> 2) + i]);
	}
}

void stream2mem(memt_t *arr, hls::stream<memt_t> &in, hls::stream<memt_t> &out)
{


#ifdef FREE_RUNNING
// #pragma HLS interface ap_ctrl_none port = return
#endif

#ifdef PRFLOW
#pragma HLS INTERFACE m_axi port = arr offset = direct bundle = p0
#pragma HLS interface axis register both port = in
#pragma HLS interface axis register both port = out
#else
#pragma HLS INTERFACE m_axi port = arr offset = direct bundle = p0
#endif

#ifdef FREE_RUNNING
	while (1)
	{
#endif
		memt_t op = in.read();
		if (op == WRITE_REQ)
		{
			memt_t transaction = process_write_req(arr, in);
			out.write(WRITE_RESP);
			out.write(transaction);
		}
		else if (op == READ_REQ)
		{
			out.write(READ_RESP);
			out.write(in.read()); // Transaction ID
			process_read_req(arr, in, out);
		}
#ifdef FREE_RUNNING
	}
#endif
}
