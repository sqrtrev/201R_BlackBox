// TestAppDll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int Sbox[16] = {0xE, 0x3, 0x0, 0x7, 0x2, 0xC, 0xF, 0xB, 0x5, 0xA, 0x6, 0x9, 0x8, 0x1, 0x4, 0xD};
int InverseSbox[16] = {0x2, 0xD, 0x4, 0x1, 0xE, 0x8, 0xA, 0x3, 0xC, 0xB, 0x9, 0x7, 0x5, 0xF, 0x0, 0x6};

extern "C" __declspec(dllexport) void Substitution(int* p, int* c)
{
	*c = (Sbox[(*p>>12 & 0xf)]<<12) | (Sbox[(*p>>8 & 0xf)]<<8) | (Sbox[(*p>>4  & 0xf)]<<4 ) | (Sbox[(*p    & 0xf)]   ) ;
}

extern "C" __declspec(dllexport) void Permutation(int* p, int* c)
{
	*c = ((*p>>15 & 1)<<15) | ((*p>>11 & 1)<<14) | ((*p>>7  & 1)<<13) | ((*p>>3  & 1)<<12) |
		((*p>>14 & 1)<<11) | ((*p>>10 & 1)<<10) | ((*p>>6  & 1)<<9 ) | ((*p>>2  & 1)<<8 ) |
		((*p>>13 & 1)<<7 ) | ((*p>>9  & 1)<<6 ) | ((*p>>5  & 1)<<5 ) | ((*p>>1  & 1)<<4 ) |
		((*p>>12 & 1)<<3 ) | ((*p>>8  & 1)<<2 ) | ((*p>>4  & 1)<<1 ) | ((*p     & 1)    ) ;
}

extern "C" __declspec(dllexport) void Substitution_Inverse(int* p, int* c)
{
	*c = (InverseSbox[(*p>>12 & 0xf)]<<12) | (InverseSbox[(*p>>8 & 0xf)]<<8) | 
		 (InverseSbox[(*p>>4  & 0xf)]<<4 ) | (InverseSbox[(*p    & 0xf)]   ) ;
}

extern "C" __declspec(dllimport) void Substitution(int* p, int* c);
extern "C" __declspec(dllimport) void Substitution_Inverse(int* p, int* c);
extern "C" __declspec(dllimport) void Permutation(int* p, int* c);
extern "C" __declspec(dllimport) void Encryption(int P, int* C);


int main(int argc, char* argv[])
{
	unsigned int DC[16][16] = { 0, };
	int Plaintext, Ciphertext;
	
//	Plaintext = 0x26B7;
//	Plaintext = 0x0000;


	// DC Table
	for (int x = 0; x <= 0xf; x++) {
		for (int x_prime = 0; x_prime <= 0xf; x_prime++) {
			int y, y_prime, input;

			input = x ^ x_prime;
			y = Sbox[x];
			y_prime = Sbox[x_prime];

			DC[y ^ y_prime][input] += 1;
		}
	}

	
	int P, P2, RP, RP2;
	int tmp[16] = { 0, };
	for (int i = 0; i <= 0xffff; i++ ) {
		P = i;
		P2 = P ^ 0x3000;

		Encryption(P, &RP);
		Encryption(P2, &RP2);

		if ((P & 0xf) == 0x3 && (RP & 0xf) == 0x3) {
			for (int key = 0; key <= 0xf; key++) {
				int tRP = (RP & 0xf) ^ key;
				int tRP2 = (RP2 & 0xf) ^ key;

				tRP = InverseSbox[tRP];
				tRP2 = InverseSbox[tRP2];

				if ((tRP ^ tRP2) == 0x3) tmp[key] += 1;
			}
		}
	}

	for (int i = 0; i <= 0xf; i++) printf("%X\t", i);
	printf("\n");
	for (int i = 0; i <= 0xf; i++) printf("%d\t", tmp[i]);
	
	// Get Plaintext and Ciphertext cases
	/*
	printf("\t");
	for (int i = 0; i <= 0xf; i++)
		printf("%X\t", i);
	printf("\n");
	for (int i = 0; i <= 0xf; i++) {
		printf("%X\t", i);
		for (int j = 0; j <= 0xf; j++) {
			printf("%d\t", DC[j][i]);
		}
		printf("\n");
	}*/
	
	// Encryption(Plaintext, &Ciphertext);
	
	// printf("Plaintext = %04X, Ciphertext = %04X\n", Plaintext, Ciphertext);

	return 0;
}

/*
int main(int argc, char* argv[])
{
	int Plaintext, Ciphertext;

	Plaintext = 0x26B7;
	//   Plaintext = 0x0000;
	int percent1[16] = { 0, };
	for (int i = 0;; i++) {

		int test = i;
		int test2 = test ^ 0x1000;
		int rtest, rtest2;
		Encryption(test, &rtest);
		Encryption(test2, &rtest2);
		if (((rtest ^ rtest2) >> 12) & 0xf && !(((rtest ^ rtest2) >> 4) & 0xf) && !(((rtest ^ rtest2)) & 0xf) && !(((rtest ^ rtest2) >> 8) & 0xf)) {
			//printf("0x%04x,0x%04x   0x%04x,0x%04x\n", test, rtest, test2, rtest2);
			for (int j = 0; j < 16; j++) {
				int rrtest = ((rtest >> 12) & 0xf) ^ j;
				int rrtest2 = ((rtest2 >> 12) & 0xf) ^ j;
				rrtest = InverseSbox[rrtest];
				rrtest2 = InverseSbox[rrtest2];
				if ((rrtest ^ rrtest2) == 0x4) {
					//printf("%x OK\n", j);
					percent1[j]++;
				}
			}
		}

		if (i == 0xffff)break;
	}
	for (int j = 0; j < 16; j++)printf("%d ", percent1[j]);
	printf("\n");

	int percent2[16] = { 0, };
	for (int i = 0;; i++) {

		int test = i;
		int test2 = test ^ 0x500;
		int rtest, rtest2;
		Encryption(test, &rtest);
		Encryption(test2, &rtest2);
		if (((rtest ^ rtest2) >> 8) & 0xf && !(((rtest ^ rtest2)) & 0xf) && !(((rtest ^ rtest2) >> 12) & 0xf) && !(((rtest ^ rtest2) >> 4) & 0xf)) {
			//printf("0x%04x,0x%04x   0x%04x,0x%04x\n", test, rtest, test2, rtest2);
			for (int j = 0; j < 16; j++) {
				int rrtest = ((rtest >> 8) & 0xf) ^ j;
				int rrtest2 = ((rtest2 >> 8) & 0xf) ^ j;
				rrtest = InverseSbox[rrtest];
				rrtest2 = InverseSbox[rrtest2];
				if ((rrtest ^ rrtest2) == 0x4) {
					//printf("%x OK\n", j);
					percent2[j]++;
				}
			}
		}

		if (i == 0xffff)break;
	}
	for (int j = 0; j < 16; j++)printf("%d ", percent2[j]);
	printf("\n");

	int percent3[16] = { 0, };
	for (int i = 0;; i++) {

		int test = i;
		int test2 = test ^ 0x100;
		int rtest, rtest2;
		Encryption(test, &rtest);
		Encryption(test2, &rtest2);
		if (((rtest ^ rtest2) >> 4) & 0xf && !(((rtest ^ rtest2)) & 0xf) && !(((rtest ^ rtest2) >> 12) & 0xf) && !(((rtest ^ rtest2) >> 8) & 0xf)) {
			//printf("0x%04x,0x%04x   0x%04x,0x%04x\n", test, rtest, test2, rtest2);
			for (int j = 0; j < 16; j++) {
				int rrtest = ((rtest >> 4) & 0xf) ^ j;
				int rrtest2 = ((rtest2 >> 4) & 0xf) ^ j;
				rrtest = InverseSbox[rrtest];
				rrtest2 = InverseSbox[rrtest2];
				if ((rrtest ^ rrtest2) == 0x4) {
					//printf("%x OK\n", j);
					percent3[j]++;
				}
			}
		}

		if (i == 0xffff)break;
	}
	for (int j = 0; j < 16; j++)printf("%d ", percent3[j]);
	printf("\n");

	int percent4[16] = { 0, };
	for (int i = 0;; i++) {

		int test = i;
		int test2 = test ^ 0x1000;
		int rtest, rtest2;
		Encryption(test, &rtest);
		Encryption(test2, &rtest2);
		if (((rtest ^ rtest2)) & 0xf && !(((rtest ^ rtest2) >> 4) & 0xf) && !(((rtest ^ rtest2) >> 12) & 0xf) && !(((rtest ^ rtest2) >> 8) & 0xf)) {
			//printf("0x%04x,0x%04x   0x%04x,0x%04x\n", test, rtest, test2, rtest2);
			for (int j = 0; j < 16; j++) {
				int rrtest = ((rtest) & 0xf) ^ j;
				int rrtest2 = ((rtest2) & 0xf) ^ j;
				rrtest = InverseSbox[rrtest];
				rrtest2 = InverseSbox[rrtest2];
				if ((rrtest ^ rrtest2) == 0x4) {
					//printf("%x OK\n", j);
					percent4[j]++;
				}
			}
		}

		if (i == 0xffff)break;
	}
	for (int j = 0; j < 16; j++)printf("%d ", percent4[j]);
	printf("\n");
	//Encryption(Plaintext, &Ciphertext);

	//printf("Plaintext = %04X, Ciphertext = %04X\n", Plaintext, Ciphertext);

	return 0;
}*/