#include <hls_stream.h>
#include "duc.h"

memt_t process_write_req(memt_t *arr, hls::stream<memt_t> &in)
{
	id_t transaction = in.read();
	addr_t base_addr = in.read();
	memt_t length = in.read();

	for (unsigned int i = 0; i < length; i++)
	{
		arr[base_addr + i] = in.read();
	}

	return transaction;
}

void process_read_req(memt_t *arr, hls::stream<memt_t> &in, hls::stream<memt_t> &out)
{
	addr_t base_addr = in.read();
	memt_t length = in.read();
	out.write(base_addr); // Unnecessary but might help with debugging
	out.write(length);
	for (unsigned int i = 0; i < length; i++)
	{
		out.write(arr[base_addr + i]);
	}
}

void stream2mem(memt_t *arr, hls::stream<memt_t> &in, hls::stream<memt_t> &out)
{

#pragma HLS INTERFACE m_axi port = arr offset = direct bundle = p0
#pragma HLS interface ap_ctrl_none port = return
	while (1)
	{
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
	}
}
