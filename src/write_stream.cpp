#include <hls_stream.h>
#include "duc.h"
#define TRANSACTION1 0x1
#define BASE_ADDR 0x1
#define WRITE_LEN 0x100

uint32_t in_seq[] = {WRITE_REQ, TRANSACTION1, BASE_ADDR, BASE_ADDR, WRITE_LEN};

void write_req(hls::stream<memt_t> &out)
{

	//#pragma HLS interface ap_ctrl_none port=return

	for (int i = 0; i < 0x20; i++)
	{
		for (unsigned int i = 0; i < 5; i++)

			out.write(in_seq[i]);

		for (unsigned int i = 0; i < WRITE_LEN; i++)
			out.write(i);

		out.write(0);
		//		out.write(0);
		//		out.write(0);
	}
}
