#include <ap_int.h>
#include <hls_stream.h>

typedef struct {
  short int data;
  ap_uint<1> last;
} in_t;

typedef struct {
  float data;
  ap_uint<1> last;
} out_t;

const short int cost[32] = {32, 31, 30, 27, 23, 18, 12, 6, 0, -6, -12, -18, -23, -27, -30, -31, -32, -31, -30, -27, -23, -18, -12, -6, 0, 6, 12, 18, 23, 27, 30, 31};
const short int sint[32] = {0, 6, 12, 18, 23, 27, 30, 31, 32, 31, 30, 27, 23, 18, 12, 6, 0, -6, -12, -18, -23, -27, -30, -31, -32, -31, -30, -27, -23, -18, -12, -6};
const float pi = 3.1415926535897932384626;

/*
	II = cost[index] * p[0] - sint[index] * p[1]
   	QQ = sint[index] * p[0] + cost[index] * p[1]
*/

void mixcarr(hls::stream<in_t> &strm_in, hls::stream<out_t> &strm_out, float *freq, float *ti, float *phi0)
{
	short int MUL, ix;
	in_t in;
	out_t out;

    float phi = (32*phi0)/(2*pi);
    float ps = 32*(*freq)*(*ti); /* phase step */

	do {
		in = strm_in.read();
		ix = ((short int) phi) & 0x1F;
		MUL = cost[ix]*in.data;


	} while();


	MUL[0] =
}
