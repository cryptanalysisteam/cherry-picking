#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <random>
using namespace std;

#define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QR(a, b, c, d) (					\
        a += b,  d ^= a,  d = ROTL(d,16),	\
        c += d,  b ^= c,  b = ROTL(b,12),	\
        a += b,  d ^= a,  d = ROTL(d, 8),	\
        c += d,  b ^= c,  b = ROTL(b, 7))

#define GET_BIT(a,p) (((a)>>(p))&0x1)
mt19937 gen((unsigned int) time(NULL));

void BubbleSortABS(double *arr,int len) {
	double temp;
	int numSorted,indexToBubble;

	for(numSorted=0; numSorted<len-1; numSorted++)
		for(indexToBubble=0; indexToBubble<len-numSorted-1; indexToBubble++)
			if(fabs(arr[indexToBubble])<fabs(arr[indexToBubble+1])) {
				temp=arr[indexToBubble];
				arr[indexToBubble]=arr[indexToBubble+1];
				arr[indexToBubble+1]=temp;
			}
}

double GetMedianABS(double *array,uint64_t NS) {
	BubbleSortABS(array,NS);
	if(NS%2==1) return array[NS/2];
	else return (0.5*(fabs(array[NS/2])+fabs(array[NS/2-1])));
}



int main() {
	uint64_t NK;
	uint64_t NIV;
	NK=((uint64_t)1<<6);
	NIV=((uint64_t)1<<36);
	printf("For the 11th linear approximation, we randomly generate 2^{%f} keys and 2^{%f} IVs, the experimental result is outputted as follows:\n\n",log2(NK),log2(NIV));

	uint64_t ctr11[NK];
	memset(ctr11,0,NK*sizeof(uint64_t));

	uint64_t ctr12[NK];
	memset(ctr12,0,NK*sizeof(uint64_t));

	double cor11[NK] = { 0 };
	double cor12[NK] = { 0 };

	uint32_t X[16] = { 0 };
	uint32_t XX[16] = { 0 };
	uint32_t XXX[16] = { 0 };

	uint32_t OD[16] = { 0 };
	uint32_t ID[16] = { 0 };

	ID[ 3] = 0x00002002;
	ID[ 7] = 0x00100010;
	ID[11] = 0x20000002;
	ID[15] = 0x20200000;

	double res11 = 0.0, res12 = 0.0;

	int indexOD11[] = {7*32+0, 7*32+3, 7*32+4, 7*32+20};
	int indexOD12[] = {2*32+0, 2*32+2, 2*32+4, 8*32+20, 13*32+3, 13*32+4};

	int length11 = sizeof(indexOD11) / sizeof(indexOD11[0]);
	int length12 = sizeof(indexOD12) / sizeof(indexOD12[0]);
	
	int i = 0;

	for(uint64_t iKey = 0; iKey < NK; iKey++) {

		X[0] = 0x61707865;
		X[1] = 0x3320646e;
		X[2] = 0x79622d32;
		X[3] = 0x6b206574;

		for(i = 4; i < 12; i++) {
			X[i] = gen();
		}

		for(uint64_t iIV = 0; iIV < NIV; iIV++) {
			for(i = 12; i < 16; i++) {
				X[i] = gen();
			}

			for(i = 0; i < 16; i++) {
				XX[i] = X[i];
			}

			QR(XX[0], XX[4], XX[ 8], XX[12]);
			QR(XX[1], XX[5], XX[ 9], XX[13]);
			QR(XX[2], XX[6], XX[10], XX[14]);
			QR(XX[3], XX[7], XX[11], XX[15]);

			for(i = 0; i < 16; i++) {
				XXX[i] = XX[i] ^ ID[i];
			}

			QR(XX[0], XX[5], XX[10], XX[15]);
			QR(XX[1], XX[6], XX[11], XX[12]);
			QR(XX[2], XX[7], XX[ 8], XX[13]);
			QR(XX[3], XX[4], XX[ 9], XX[14]);

			QR(XX[0], XX[4], XX[ 8], XX[12]);
			QR(XX[1], XX[5], XX[ 9], XX[13]);
			QR(XX[2], XX[6], XX[10], XX[14]);
			QR(XX[3], XX[7], XX[11], XX[15]);

			QR(XXX[0], XXX[5], XXX[10], XXX[15]);
			QR(XXX[1], XXX[6], XXX[11], XXX[12]);
			QR(XXX[2], XXX[7], XXX[ 8], XXX[13]);
			QR(XXX[3], XXX[4], XXX[ 9], XXX[14]);

			QR(XXX[0], XXX[4], XXX[ 8], XXX[12]);
			QR(XXX[1], XXX[5], XXX[ 9], XXX[13]);
			QR(XXX[2], XXX[6], XXX[10], XXX[14]);
			QR(XXX[3], XXX[7], XXX[11], XXX[15]);

			for (i = 0; i < 16; i++) {
				OD[i] = XX[i] ^ XXX[i];
			}

			int od11 = GET_BIT(OD[indexOD11[0]/32],indexOD11[0]%32);
			for(i = 1; i < length11; i++) {
				od11 = od11 ^ GET_BIT(OD[indexOD11[i]/32],indexOD11[i]%32);
			}

			int od12 = GET_BIT(OD[indexOD12[0]/32],indexOD12[0]%32);
			for(i = 1; i < length12; i++) {
				od12 = od12 ^ GET_BIT(OD[indexOD12[i]/32],indexOD12[i]%32);
			}

			if(od11 == 0) {
				ctr11[iKey]++;
			}

			if(od12 == 0) {
				ctr12[iKey]++;
			}

		}

		cor11[iKey] = 2.0*((double)ctr11[iKey]/(double)NIV)-1.0;
		cor12[iKey] = 2.0*((double)ctr12[iKey]/(double)NIV)-1.0;
	}
	res11 = GetMedianABS(cor11,NK);
	res12 = GetMedianABS(cor12,NK);
	

	printf("The correlation of gamma0 is %f = 2^%f\n\n",res11,log2(abs(res11)));
	printf("The correlation of gamma1 is %f = 2^%f\n\n",res12,log2(abs(res12)));
	printf("The correlation of the linear approximation for this linear mask is %f = 2^%f\n",res11*res12,log2(abs(res11*res12)));

}


