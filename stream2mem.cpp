#include <hls_stream.h>
#include "duc.h"

void process_read_req(memt_t *arr, hls::stream<memt_t> &in) {
	id_t transaction = in.read();
	addr_t base_addr = in.read();
	base_addr |= (in.read() << 32);
	memt_t length = in.read();

	for (unsigned int i = 0; i < length; i++) {
		arr[base_addr + i] = in.read();
	}
}

void stream2mem(memt_t *arr, hls::stream<memt_t> &in, hls::stream<memt_t> &out) {

#pragma HLS INTERFACE m_axi port = arr offset = slave bundle = p0
#pragma HLS interface ap_ctrl_none port=return
	while (1) {
		switch (in.read()) {
		case WRITE_REQ:
			process_read_req(arr, in);
			out.write(WRITE_RESP);
			break;

		case READ_REQ:
			process_read_req(arr, in);
			out.write(READ_RESP);
			break;

		}
	}
	// RETURN;

}
