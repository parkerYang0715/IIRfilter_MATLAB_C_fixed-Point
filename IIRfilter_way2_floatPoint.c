#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define input_Len 41
#define IIR_filter_Len 7
static double IIR_coeff_B[IIR_filter_Len] = {0.007585113,0.04551068,0.1137767,0.1517022,0.1137767,0.04551068,0.007585113};
static double IIR_coeff_A[IIR_filter_Len] = {1,-1.485052,1.603616,-0.9240610,0.3592332,-0.07561177,0.007322802};

void IIR_filter_floatPoint(const double *b, const double *a, size_t filterLength, const double *in, double *out, size_t length) {
    const double a0 = a[0];
    const double *a_end = &a[filterLength-1];
    const double *out_start = out;
    a++;
    out--;
    size_t m;
    for (m = 0; m < length; m++) {
        const double *b_macc = b;
        const double *in_macc = in;
        const double *a_macc = a;
        const double *out_macc = out;
        double b_tmp = (*in_macc--) * (*b_macc++);
        double a_tmp = 0;
        while (a_macc <= a_end && out_macc >= out_start) {
            b_tmp += (*in_macc--) * (*b_macc++);
            a_tmp += (*out_macc--) * (*a_macc++);
        }
        *++out = (b_tmp - a_tmp) / a0;
        in++;
    }
}

int main(){
	
	double *sig = calloc(input_Len,sizeof(double));
	double *IIR_sig = calloc(input_Len,sizeof(double));
	
	printf("\n ------ input-----\n");
	for (uint16_t k=0;k<input_Len;k++){
		sig[k]=0.7*sin(0.15*M_PI*k)+0.1*sin(0.95*M_PI*k)+0.2*sin(0.77*M_PI*k);
		printf("sig[%d]=%f\n",k,*(sig+k));
		//printf("%f\n",*(sig+k));
	}
	IIR_filter_floatPoint(IIR_coeff_B, IIR_coeff_A, IIR_filter_Len, sig, IIR_sig, input_Len);
	printf("\n ------ IIR output-----\n");
	for (uint16_t k;k<input_Len;k++){
		printf("IIR_sig[%d]=%f\n",k,*(IIR_sig+k));
		//printf("%f\n",*(IIR_sig+k));
	}
	
	
	printf("sizeof(size_t)= %d\n",sizeof(size_t));
	printf("sizeof(unsigned long)= %d\n",sizeof(unsigned long));
	printf("sizeof(unsigned)= %d\n",sizeof(unsigned));
	printf("sizeof(unsigned short)= %d\n",sizeof(unsigned short));
	// size_t is an unsigned type. So, it cannot represent any negative values(<0). 
	// You use it when you are counting something, and are sure that it cannot be negative. 
	// For example, strlen() returns a size_t because the length of a string has to be at least 0.
	return 0 ;
}