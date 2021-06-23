#include <hls_stream.h>

#include <ap_int.h>
#include <ap_axi_sdata.h>

#define IN_BUS_SIZE  	512
#define IN_T_BITS		16
#define IN_T_BYTES		2						// IN_T_BITS/8
#define IN_T_MASK		0xFF

#define MAX_IN 			32						// IN_BUS_SIZE/IN_T_BITS
#define MAX_IN_BITS		5 						/* log2(MAX_IN) */
#define MAX_VEC			16384
#define MAX_VEC_BITS	14 						/* log2(MAX_VEC) */
#define MAX_MEM			512 					// MAX_VEC*(IN_T_BITS/IN_BUS_SIZE)

#define MULT_SIZE		32						// 2*IN_T_BITS
#define ACC_SIZE		42						// MULT_SIZE + 10 // log2(MAX_MEM + 1)

#define OUT_BUS_SIZE  	64
#define OUT_T_BITS		64

#define MAX_OUT			1						// OUT_BUS_SIZE/OUT_T_BITS

typedef struct ap_axis<IN_BUS_SIZE, 0, 0, 0> in_t;
typedef struct ap_axis<OUT_BUS_SIZE, 0, 0, 0> out_t;

// The top-level function
void axis_short_float_macc(hls::stream<in_t> &strm_in, hls::stream<out_t> &strm_out)
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS interface axis port=strm_in
#pragma HLS INTERFACE axis port=strm_out

   in_t tmp;
   out_t tmpo;

   //ap_int<IN_T_BITS> a, b;
   int a = 0, b = 0;

   ap_int<MULT_SIZE> mult;
   ap_int<ACC_SIZE> acc;
   ap_int<IN_BUS_SIZE> localmem[MAX_MEM];

   static ap_uint<MAX_VEC_BITS> i, pkts;
   static ap_uint<MAX_IN_BITS> j;

   for (i = 0; i < MAX_MEM; i++) {
	   tmp = strm_in.read();
	   localmem[i] = tmp.data;
	   if (tmp.last == 1) break ;
   }
   pkts = ++i;

   do {
	   acc = 0;

	   for (i = 0; i < pkts; i ++) {
		   tmp = strm_in.read();
		   for(j = 0; j < MAX_IN; j++)
		   {
			   #pragma HLS unroll factor=32
			   a = (localmem[i] >> IN_T_BITS*j) & IN_T_MASK;
			   b = (tmp.data    >> IN_T_BITS*j) & IN_T_MASK;

			   mult = a*b;
			   acc += mult;

			   if(tmp.keep >> IN_T_BYTES*j == 0) break;
		   }
	   }

	   tmpo.last = tmp.last;
	   tmpo.data = acc;
	   strm_out.write(tmpo);

   } while(tmp.last != 1);
}
