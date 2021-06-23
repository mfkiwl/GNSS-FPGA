#include <hls_stream.h>

#include <ap_int.h>
#include <ap_axi_sdata.h>

#include <stdlib.h>
#include <stdio.h>

#define IN_BUS_SIZE  	512
#define IN_T_BITS		16
#define IN_T_BYTES		2						// IN_T_BITS/8

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

#define VEC_SIZE 		10

typedef struct ap_axis<IN_BUS_SIZE, 0, 0, 0> in_t;
typedef struct ap_axis<OUT_BUS_SIZE, 0, 0, 0> out_t;

const short int u[VEC_SIZE] = {1,1,1,1,1,1,1,1,1,1};
const short int v[VEC_SIZE] = {1,1,1,1,1,1,1,1,1,1};

static ap_int<OUT_BUS_SIZE> hw_dp = 0;
static ap_int<OUT_BUS_SIZE> sw_dp = 0;

// The top-level function
void axis_short_float_macc(hls::stream<in_t> &strm_in, hls::stream<out_t> &strm_out);

int main()
{
   hls::stream<in_t> in;
   hls::stream<out_t> out;
   int err = 0;

   in_t tmpa, tmpb; out_t tmpo;

   /* Data */
   for(int k = 0; k < VEC_SIZE; k += MAX_IN) {
	   tmpa.keep = 1;
	   for(int i = 0; i < MAX_IN && i < VEC_SIZE; i++) {
		   ((short int*) &(tmpa.data))[i] = u[i];
		   for(int j = 0; j < IN_T_BYTES - !i; j++)
			   tmpa.keep |= tmpa.keep << 1;
	   }
	   if(k >= VEC_SIZE - MAX_IN)
		   tmpa.last = 1;
	   in.write(tmpa);
   }
   for(int k = 0; k < VEC_SIZE; k += MAX_IN) {
	   tmpb.keep = 1;
	   for(int i = 0; i < MAX_IN && i < VEC_SIZE; i++) {
		   ((short int*) &(tmpb.data))[i] = v[i];
		   for(int j = 0; j < IN_T_BYTES - !i; j++)
			   tmpb.keep |= tmpb.keep << 1;
	   }
	   if(k >= VEC_SIZE - MAX_IN)
		   tmpb.last = 1;
	   in.write(tmpb);
   }

   /* HW */
   axis_short_float_macc(in, out);
   tmpo = out.read();

   /* SW */
   for(int i = 0; i < VEC_SIZE; i++)
	   sw_dp += u[i]*v[i];

   /* Check */
   hw_dp = tmpo.data;
   printf("%d == %d ?\n", (int) hw_dp, (int) sw_dp);
   err += (hw_dp - sw_dp) != 0;

   return err;
}
