#include <stdio.h>
#include <stdint.h>
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

void setRandomBit(uint32_t* bitArray, int i, int j) {
	int randomBit = rand() % 2;
	bitArray[i] = (bitArray[i] & ~(1 << j)) | (randomBit << j);
}

uint32_t setBit(uint32_t num, int pos) {
	return num | (1 << pos);
}

uint32_t clearBit(uint32_t num, int pos) {
	return num & ~(1 << pos);
}

uint32_t flipBit(uint32_t num, int i) {
	return num ^ (1 << i);
}

int main() {
	uint32_t x[16] = { 0 };
	uint32_t y[16] = { 0 };
	uint32_t z[16] = { 0 };
	uint32_t od[16] = { 0 };

	uint32_t NK = 0;
	NK = 1 << 10;
	int n = 7;
	uint32_t DIB = 1 << n;
	int bitPositions[n] = {15*32+16,15*32+4,15*32+3,15*32+15,15*32+29,15*32+5,15*32+13};

	uint32_t count = 0;
	double p = 0.0;

	printf("The DIB set: iv112, iv100, iv99, iv111, iv125, iv101, and iv109\n\n");
	printf("The number of random keys is %d\n\n",NK);
	printf("The experimental result of the first experiment of Page 15 is outputted as follows:\n\n");
	

	for (uint32_t iKey = 0; iKey < NK; iKey++) {
		x[0] = 0x61707865;
		x[1] = 0x3320646e;
		x[2] = 0x79622d32;
		x[3] = 0x6b206574;
		for (int i = 4; i < 12; i++) {
			x[i] = gen();
		}
		int flag = 0;
		for (int i = 12; i < 16; i++) {
			x[i] = gen();
		}
		for(uint32_t iDIB = 0; iDIB < DIB; iDIB++) {
			for (int i = 0; i < n; i++) {
				if (iDIB & (1 << i)) {
					x[bitPositions[i]/32] = setBit(x[bitPositions[i]/32], bitPositions[i]%32);
				} else {
					x[bitPositions[i]/32] = clearBit(x[bitPositions[i]/32], bitPositions[i]%32);
				}
			}

			for(int i = 0; i < 16; i++) {
				y[i] = x[i];
				z[i] = x[i];
			}

			y[15]=flipBit(y[15],5);
			y[15]=flipBit(y[15],17);

			QR(y[0], y[4], y[ 8], y[12]);
			QR(y[1], y[5], y[ 9], y[13]);
			QR(y[2], y[6], y[10], y[14]);
			QR(y[3], y[7], y[11], y[15]);

			QR(z[0], z[4], z[ 8], z[12]);
			QR(z[1], z[5], z[ 9], z[13]);
			QR(z[2], z[6], z[10], z[14]);
			QR(z[3], z[7], z[11], z[15]);

			for(int i = 0; i < 16; i++) {
				od[i] = z[i] ^ y[i];
			}

			if((od[3]==0x00002002)&&(od[7]==0x00100010)&&(od[11]==0x20000002)&&(od[15]==0x20200000))  {
				flag=1;
				break;
			}
		}

		if(flag == 1) {
			count++;
		}
	}
	p=(double)count/NK;

	printf("%d keys can form a right pair and the success probability is %f\n",count,p);


}