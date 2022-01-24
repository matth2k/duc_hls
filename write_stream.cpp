#include <hls_stream.h>
#include "duc.h"
#define TRANSACTION1 0x1
#define BASE_ADDR 0xDEADBEEF
#define READ_LEN 0x10

uint32_t in_seq[] = {READ_REQ, TRANSACTION1, BASE_ADDR, BASE_ADDR, READ_LEN};



void write_req(hls::stream<memt_t> &out) {

#pragma HLS interface ap_ctrl_none port=return
	for (int i = 0; i < 5; i++)
		out.write(in_seq[i]);

	for (int i = 0; i < READ_LEN; i++)
		out.write(i);

}
