#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define input_Len 21
#define IIR_filter_Len 7
static int16_t IIR_coeff_B[IIR_filter_Len] = {124,746,1864,2485,1864,746,124};
static int16_t IIR_coeff_A[IIR_filter_Len] = {(1<<14),-24331,26274,-15140,5886,-1239,120};

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


void IIR_filter_fixedPoint(const int16_t *b, const int16_t *a, size_t filterLength, const int16_t *in, int16_t *out, size_t length) {
    const int16_t a0 = a[0];
    const int16_t *a_end = &a[filterLength-1];
    const int16_t *out_start = out;
    a++;
    out--;
    size_t m;
    for (m = 0; m < length; m++) {
        const int16_t *b_macc = b;
        const int16_t *in_macc = in;
        const int16_t *a_macc = a;
        const int16_t *out_macc = out;
        int16_t b_tmp = q_mul( (*in_macc--) , (*b_macc++) );
        int16_t a_tmp = 0;
		uint16_t whileLoop=0;
        while (a_macc <= a_end && out_macc >= out_start) {
            b_tmp = q_add_sat(b_tmp , q_mul( (*in_macc--) , (*b_macc++)) );
            a_tmp = q_add_sat(a_tmp, q_mul((*out_macc--) , (*a_macc++)) );
        }
        *++out = q_add_sat(b_tmp , -a_tmp) ;
        in++;
    }
}

int main(){
	
	int16_t *sig = calloc(input_Len,sizeof(int16_t));
	int16_t *IIR_sig = calloc(input_Len,sizeof(int16_t));
	
	printf("\n ------ input-----\n");
	for (uint16_t k=0;k<input_Len;k++){
		sig[k]=(0.7*sin(0.15*M_PI*k)+0.1*sin(0.95*M_PI*k)+0.2*sin(0.77*M_PI*k))*(1<<14);
		printf("sig[%d]=%d\n",k,*(sig+k));
		//printf("%f\n",*(sig+k));
	}
	IIR_filter_fixedPoint(IIR_coeff_B, IIR_coeff_A, IIR_filter_Len, sig, IIR_sig, input_Len);
	printf("\n ------ IIR output-----\n");
	for (uint16_t k;k<input_Len;k++){
		printf("IIR_sig[%d]=%f\n",k,(float) *(IIR_sig+k)/(1<<14));
		//printf("%f\n",*(IIR_sig+k));
	}
	
	return 0 ;
}