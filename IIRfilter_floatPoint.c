#include <time.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define input_Len 41
#define FIR_FILTER_LENGTH 17
#define IIR_B 7
#define IIR_A 6
static float coefficients[FIR_FILTER_LENGTH] = {0,0.0015989647,0.0054936779,-0.0075196516,-0.039788735,-0.028071220,0.096058505,0.28288723,0.375,0.28288723,0.096058505,-0.028071220,-0.039788735,-0.0075196516,0.0054936779,0.0015989647,0};
static float IIR_coeff_B[IIR_B] = {0.007585113,0.04551068,0.1137767,0.1517022,0.1137767,0.04551068,0.007585113};
static float IIR_coeff_A[IIR_A] = {-1.485052,1.603616,-0.9240610,0.3592332,-0.07561177,0.007322802};
typedef struct{
	float buf[FIR_FILTER_LENGTH];
	float out;	 //current filter output
} FIRFilter;  
typedef struct{
	float prevOut[IIR_A]; // previous output
	float prevIn[IIR_B]; // previous input
	float out;	 //current filter output
} IIRFilter;  
void FIRFilter_Init(FIRFilter *fir){
	// reset everything to 0
	/* clear filter buffer */
	for (uint8_t n=0;n<FIR_FILTER_LENGTH;n++){
		fir->buf[n]=0.0f;
	}
	/* clear filter output*/
	fir->out =0.0f;
}
void IIRFilter_Init(IIRFilter *iir){
	// reset everything to 0
	for (uint8_t n=0;n<IIR_A;n++){
		iir->prevOut[n]=0.0f;
	}
	for (uint8_t n=0;n<IIR_B;n++){
		iir->prevIn[n]=0.0f;
	}	
	iir->out =0.0f;
}
float FIRFilter_Update(FIRFilter *fir,float inp){
	//inp: current input
	//return: current output
	/* move memory */
	for(uint8_t n=FIR_FILTER_LENGTH-1;n>0;n--)
		fir->buf[n] = fir->buf[n-1];
	/* store the lastest sample in buffer */
	fir->buf[0] = inp;
	
	/* compute output*/
	fir->out = 0.0f;

	for(uint8_t n=0; n< FIR_FILTER_LENGTH;n++){
		fir->out += coefficients[n] * fir->buf[n];
	}
	return fir->out;
}
float IIRFilter_Update(IIRFilter *iir,float inp){
	/* move memory */
	for(uint8_t n=IIR_B-1;n>0;n--)
		iir->prevIn[n] = iir->prevIn[n-1];
	/* store the lastest input in buffer */
	iir->prevIn[0] = inp;
	for(uint8_t n=IIR_A-1;n>0;n--)
		iir->prevOut[n] = iir->prevOut[n-1];
	//* store the lastest output in buffer */
	iir->prevOut[0] = iir->out;
	
	/* compute output*/
	iir->out = 0.0f;

	for(uint8_t n=0; n< IIR_B;n++){
		iir->out += IIR_coeff_B[n] * iir->prevIn[n];
	}
	for(uint8_t n=0; n< IIR_A;n++){
		iir->out -= IIR_coeff_A[n] * iir->prevOut[n];
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
	float *sig = calloc(input_Len,sizeof(float));
	float *FIR_sig = calloc(input_Len,sizeof(float));
	float *IIR_sig = calloc(input_Len,sizeof(float));
	printf("\n ------ input-----\n");
	for (uint16_t k=0;k<input_Len;k++){
		sig[k]=0.7*sin(0.15*M_PI*k)+0.1*sin(0.95*M_PI*k)+0.2*sin(0.77*M_PI*k);
		printf("sig[%d]=%f\n",k,*(sig+k));
		//printf("%f\n",*(sig+k));
	}
	printf("\n ------ FIR output-----\n");
	clock_t start_t, end_t, total_t;
	uint16_t k;
	start_t = clock();

	for (k = 0;k<input_Len;k++){
		FIR_sig[k] = FIRFilter_Update(fir,sig[k]);
		printf("FIR_sig[%d]=%f\n",k,*(FIR_sig+k));
		//printf("%f\n",*(FIR_sig+k));
	}
	printf("\n ------ IIR output-----\n");
	for (k = 0;k<input_Len;k++){
		IIR_sig[k] = IIRFilter_Update(iir,sig[k]);
		printf("IIR_sig[%d]=%f\n",k,*(IIR_sig+k));
		//printf("%f\n",*(IIR_sig+k));
	}
	end_t = clock();
	total_t = (double)(end_t - start_t) ;
	printf("\n linear Buffer Total time taken by CPU: %f \n",(float)total_t/ CLOCKS_PER_SEC  );
	printf("FIR_sig[%d]=%f\n",k-1,*(FIR_sig+k-1));
	free(fir);
	return 0;
}