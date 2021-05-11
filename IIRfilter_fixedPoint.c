#include <time.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define input_Len 41
#define FIR_FILTER_LENGTH 17
#define IIR_B 7
#define IIR_A 6
static int16_t coefficients[FIR_FILTER_LENGTH] = {0,26,90	-123,-652,-460,1574,4635,6144,4635,1574,-460,-652,-123,90,26,0};
static int16_t IIR_coeff_B[IIR_B] = {124,746,1864,2485,1864,746,124};
static int16_t IIR_coeff_A[IIR_A] ={+24331,-26274,+15140,-5886,+1239,-120};
int8_t Q = 14;  // Q refers to the fractional part's number of bits
#define K   (1 << (Q - 1))
// saturate to range of int16_t
int16_t sat16(int32_t x)
{
	if (x > 0x7FFF) {
		printf("val>0x7FFF saturate!!\n");
		return 0x7FFF;
	}
	else if (x < -0x8000){
		printf("val<-0x8000 saturate!!\n");
		return -0x8000;
	}
	else return (int16_t)x;
}
int16_t q_add_sat(int16_t a, int16_t b)
{
    int16_t result;
    int32_t tmp;

    tmp = (int32_t)a + (int32_t)b;
    if (tmp > 0x7FFF)
        tmp = 0x7FFF;
    if (tmp < -1 * 0x8000)
        tmp = -1 * 0x8000;
    result = (int16_t)tmp;

    return result;
}
int16_t q_mul(int16_t a, int16_t b)
{
    int16_t result;
    int32_t temp;

    temp = (int32_t)a * (int32_t)b; // result type is operand's type
    // Rounding; mid values are rounded up
    temp += K;
    // Correct by dividing by base and saturate result
    result = sat16(temp >> Q);

    return result;
}


typedef struct{
	int16_t buf[FIR_FILTER_LENGTH];
	int16_t out;	 //current filter output
} FIRFilter; 
typedef struct{
	int16_t prevOut[IIR_A]; // previous output
	int16_t prevIn[IIR_B]; // previous input
	int16_t out;	 //current filter output
} IIRFilter; 
 
void FIRFilter_Init(FIRFilter *fir){
	// reset everything to 0
	/* clear filter buffer */
	for (uint8_t n=0;n<FIR_FILTER_LENGTH;n++){
		fir->buf[n]=0;
	}
	
	/* clear filter output*/
	fir->out =0;
}
void IIRFilter_Init(IIRFilter *iir){
	// reset everything to 0
	for (uint8_t n=0;n<IIR_A;n++){
		iir->prevOut[n]=0;
	}
	for (uint8_t n=0;n<IIR_B;n++){
		iir->prevIn[n]=0;
	}	
	iir->out =0;
}

int16_t FIRFilter_Update_fixed(FIRFilter *fir,int16_t inp){
	//inp: current input
	//return: current output
	
	/* move memory */
	for(uint8_t n=FIR_FILTER_LENGTH-1;n>0;n--)
		fir->buf[n] = fir->buf[n-1];
	/* store the lastest sample in buffer */
	fir->buf[0] = inp;
	
	/* compute output*/
	fir->out = 0;

	for(uint8_t n=0; n< FIR_FILTER_LENGTH;n++){
		fir->out = q_add_sat( fir->out , q_mul(coefficients[n], fir->buf[n]) )  ; 
	}
	return fir->out;
}

int16_t IIRFilter_Update_fixed(IIRFilter *iir,int16_t inp){
	for(uint8_t n=IIR_B-1;n>0;n--)
		iir->prevIn[n] = iir->prevIn[n-1];
	/* store the lastest input in buffer */
	iir->prevIn[0] = inp;
	for(uint8_t n=IIR_A-1;n>0;n--)
		iir->prevOut[n] = iir->prevOut[n-1];
	//* store the lastest output in buffer */
	iir->prevOut[0] = iir->out;
	
	/* compute output*/
	iir->out = 0;

	for(uint8_t n=0; n< IIR_B;n++){
		iir->out = q_add_sat(iir->out , q_mul(IIR_coeff_B[n] , iir->prevIn[n]) );
	}
	for(uint8_t n=0; n< IIR_A;n++){
		iir->out = q_add_sat(iir->out ,  q_mul(IIR_coeff_A[n] , iir->prevOut[n]) );
	}
	return iir->out;
}

int main(){
	FIRFilter *fir = NULL;
	IIRFilter *iir = NULL;
	fir = (FIRFilter*) malloc(sizeof(FIRFilter) );
	iir = (IIRFilter*) malloc(sizeof(IIRFilter) );
	FIRFilter_Init(fir);
	IIRFilter_Init(iir);
	int16_t *sig = calloc(input_Len,sizeof(int16_t));
	int16_t *FIR_sig = calloc(input_Len,sizeof(int16_t));
	int16_t *IIR_sig = calloc(input_Len,sizeof(int16_t));
	printf("\n ------ input-----\n");
	for (uint16_t k=0;k<input_Len;k++){
		sig[k]=(0.7*sin(0.15*M_PI*k)+0.1*sin(0.95*M_PI*k)+0.2*sin(0.77*M_PI*k))*(1<<Q);
		printf("sig[%d]=%d\n",k,*(sig+k));
		//printf("%f\n",*(sig+k));
	}
	printf("\n ------ FIR output-----\n");
	clock_t start_t, end_t, total_t;
	uint16_t k;
	start_t = clock();

	for (k = 0;k<input_Len;k++){
		FIR_sig[k] = FIRFilter_Update_fixed(fir,sig[k]);
		printf("FIR_sig[%d]=%f\n",k,(float)*(FIR_sig+k)/(1<<Q) );
		//printf("%f\n",*(FIR_sig+k));
	}
	printf("\n ------ IIR output-----\n");
	for (k = 0;k<input_Len;k++){
		IIR_sig[k] = IIRFilter_Update_fixed(iir,sig[k]);
		printf("IIR_sig[%d]=%f\n",k,(float)*(IIR_sig+k)/(1<<Q) );
		//printf("%f\n",*(IIR_sig+k));
	}
	end_t = clock();
	total_t = (double)(end_t - start_t) ;
	printf("\n linear Buffer Total time taken by CPU: %f \n",(float)total_t/ CLOCKS_PER_SEC  );
	free(fir);
	return 0;
}