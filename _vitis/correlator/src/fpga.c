#include "sdr.h"

int init_dma(XAxiDma *dma, XAxiDma_Config *cfg)
{
	int err;

	cfg = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
    if(cfg == NULL) return -1;
	err = XAxiDma_CfgInitialize(dma, cfg);
    if(err) return -1;
    XAxiDma_IntrDisable(dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(dma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

    return 0;
}

/*
 * Sizes are in bytes
 *
 */

int transaction_macc(XAxiDma *dma, PL_IN_T *a, PL_IN_T *b, PL_OUT_T *out, int sizea, int sizeb, int sizeo)
{
	int status = 0;

	// (schedule) RX

	Xil_DCacheInvalidateRange(out, sizeo);
	status = XAxiDma_SimpleTransfer(dma, out, sizeo, XAXIDMA_DEVICE_TO_DMA);
	if(status) return status;

	// TX

	status = XAxiDma_SimpleTransfer(dma,   a, sizea, XAXIDMA_DMA_TO_DEVICE);
	if(status) return status;

	while(XAxiDma_Busy(dma, XAXIDMA_DMA_TO_DEVICE) == TRUE);

	status = XAxiDma_SimpleTransfer(dma,   b, sizeb, XAXIDMA_DMA_TO_DEVICE);
	if(status) return status;

	while(XAxiDma_Busy(dma, XAXIDMA_DMA_TO_DEVICE) == TRUE ||
		  XAxiDma_Busy(dma, XAXIDMA_DEVICE_TO_DMA) == TRUE);

	return 0;
}

/* dot products: d1={dot(a1,b1),dot(a1,b2),dot(a1,b3)},d2={...} ----------------
*-----------------------------------------------------------------------------*/
void pl_dotNM(XAxiDma *dma, short int **in1, short int **in2, double **out, int n, int N, int M)
{
    Xil_DCacheDisable();

    PL_OUT_T res = 0;

    int sizerx = sizeof(PL_OUT_T);
    int sizetx;

    for(int i = 0, k; i < N*M; i++) {

    	sizetx = MAX_VEC_SIZE*sizeof(PL_IN_T);
        for(k = 0; k + MAX_VEC_SIZE < n; k += MAX_VEC_SIZE) {
			transaction_macc(dma, in1[i] + k, in2[i] + k, &res, sizetx, sizetx, sizerx);
			*(out[i]) += res;
		}

		sizetx = (n - k)*sizeof(PL_IN_T);
		transaction_macc(dma, in1[i] + k, in2[i] + k, &res, sizetx, sizetx, sizerx);
		*(out[i]) += res;

    }

    Xil_DCacheEnable();
}

void pl_dot_22(const short *a1, const short *a2, const short *b1,
        const short *b2, int n, double *d1, double *d2)
{
    XAxiDma dma;
    XAxiDma_Config *cfg;

    const short* in1[6] = {a1, a1, a2, a2};
    const short* in2[6] = {b1, b2, b1, b2};
    double* 	 out[6] = {&d1[0], &d1[1], &d2[0], &d2[1]};

    d1[0]=d1[1]=d2[0]=d2[1]=0.0;

    if(init_dma(&dma, cfg)) {
    	printf("Failed to start DMA\n");
    	exit(-1);
    }

#if defined(COMPARE)
	time_t pl_ti, pl_tf;
	XTime_GetTime(&pl_ti);
#endif

	pl_dotNM(&dma, in1, in2, out, n, 2, 2);

#if defined(COMPARE)
	XTime_GetTime(&pl_tf);
#endif

#if defined(COMPARE)
	double t1[2], t2[2];

	XTime ps_ti, ps_tf;
	XTime_GetTime(&ps_ti);
    dot_22(a1, a2, b1, b2, n, t1, t2);
	XTime_GetTime(&ps_tf);

	printf("dot_22_PL_time_ms: %f\n", 1.0 * (pl_tf - pl_ti) / (COUNTS_PER_SECOND/1000));
	printf("dot_22_PS_time_ms: %f\n", 1.0 * (ps_tf - ps_ti) / (COUNTS_PER_SECOND/1000));
	printf("dot_22_PL: d1={%lf, %lf} d2={%lf, %lf}  \n", d1[0], d1[1], d2[0], d2[1]);
	printf("dot_22_PS: d1={%lf, %lf} d2={%lf, %lf}\n\n", t1[0], t1[1], t2[0], t2[1]);
#endif
}

/* dot products: d1={dot(a1,b1),dot(a1,b2),dot(a1,b3)},d2={...} ----------------
* args   : short  *a1       I   input short array
*          short  *a2       I   input short array
*          short  *b1       I   input short array
*          short  *b2       I   input short array
*          short  *b3       I   input short array
*          int    n         I   number of input data
*          short  *d1       O   output short array
*          short  *d2       O   output short array
* return : none
*-----------------------------------------------------------------------------*/
void pl_dot_23(const short *a1, const short *a2, const short *b1,
                   const short *b2, const short *b3, int n, double *d1,
                   double *d2)
{
    XAxiDma dma;
    XAxiDma_Config *cfg;

    const short* in1[6] = {a1, a1, a1, a2, a2, a2};
    const short* in2[6] = {b1, b2, b3, b1, b2, b3};
    double* 	 out[6] = {&d1[0], &d1[1], &d1[2], &d2[0], &d2[1], &d2[2]};

    d1[0]=d1[1]=d1[2]=d2[0]=d2[1]=d2[2]=0.0;

    if(init_dma(&dma, cfg)) {
    	printf("Failed to start DMA\n");
    	exit(-1);
    }

#if defined(COMPARE)
	time_t pl_ti, pl_tf;
	XTime_GetTime(&pl_ti);
#endif

	pl_dotNM(&dma, in1, in2, out, n, 2, 3);

#if defined(COMPARE)
	XTime_GetTime(&pl_tf);
#endif

#if defined(COMPARE)
	double t1[3], t2[3];

	XTime ps_ti, ps_tf;
	XTime_GetTime(&ps_ti);
    dot_23(a1, a2, b1, b2, b3, n, t1, t2);
	XTime_GetTime(&ps_tf);

	printf("dot_23_PL_time_ms: %f\n", 1.0 * (pl_tf - pl_ti) / (COUNTS_PER_SECOND/1000));
	printf("dot_23_PS_time_ms: %f\n", 1.0 * (ps_tf - ps_ti) / (COUNTS_PER_SECOND/1000));
	printf("dot_23_PL: d1={%lf, %lf, %lf} d2={%lf, %lf, %lf}  \n", d1[0], d1[1], d1[2], d2[0], d2[1], d2[2]);
	printf("dot_23_PS: d1={%lf, %lf, %lf} d2={%lf, %lf, %lf}\n\n", t1[0], t1[1], t1[2], t2[0], t2[1], t2[2]);
#endif
}
