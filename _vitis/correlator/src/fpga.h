#ifndef EXTRA_H
#define EXTRA_H

#define PSPL
#define COMPARE

#ifndef PSPL
#define PS
#endif

#define PL_OUT_T float
#define PL_IN_T short int

#include "xaxidma.h"
#include "xparameters.h"
#include "xil_printf.h"
#include <time.h>
#include "xtime_l.h"

#define SIMPLE_TRANSFER_MAXBYTES 16383
#define MAX_VEC_SIZE 512

void pl_dot_22(const short *a1, const short *a2, const short *b1, const short *b2, int n, double *d1, double *d2);
void pl_dot_23(const short *a1, const short *a2, const short *b1, const short *b2, const short *b3, int n, double *d1, double *d2);
void pl_dotNM(XAxiDma *dma, short int **in1, short int **in2, double **out, int n, int N, int M);

int transaction_macc(XAxiDma *dma, PL_IN_T *a, PL_IN_T *b, PL_OUT_T *out, int sizea, int sizeb, int sizeo);

#endif
