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

int max(int table[]) {
	int idx = 0;

	for (int i = 1; i <= 0xf; i++) if (table[idx] < table[i]) idx = i;

	return idx;
}

int main(int argc, char* argv[])
{
	unsigned int DC[16][16] = { 0, };
	int Plaintext, Ciphertext;
	
//	Plaintext = 0x26B7;
//	Plaintext = 0x0000;


	// DC Table
	for (int x = 0; x <= 0xf; x++) {
		for (int x_prime = 0; x_prime <= 0xf; x_prime++) {
			int y, y_prime;

			y = Sbox[x];
			y_prime = Sbox[x_prime];

			DC[y ^ y_prime][x ^ x_prime] += 1;
		}
	}

	
	int P, P2, RP, RP2;
	int keytable[4][16] = { 0, };

	//for keytable0
	for (int i = 0; i <= 0xffff; i++) {
		P = i;
		P2 = P ^ 0x2000;

		Encryption(P, &RP);
		Encryption(P2, &RP2);
		int test = RP ^ RP2;
		if ((test & 0xf) && !((test >> 4) & 0xf) && !((test >> 8) & 0xf) && !(test >> 12)) {
			for (int key = 0; key <= 0xf; key++) {
				int tRP = (RP & 0xf) ^ key;
				int tRP2 = (RP2 & 0xf) ^ key;

				tRP = InverseSbox[tRP];
				tRP2 = InverseSbox[tRP2];

				if ((tRP ^ tRP2) == 0x9) keytable[0][key] += 1;
			}
		}
	}

	// for keytable1
	for (int i = 0; i <= 0xffff; i++) {
		P = i;
		P2 = P ^ 0x100;
		
		Encryption(P, &RP);
		Encryption(P2, &RP2);
		int test = RP ^ RP2;
		if (!(test & 0xf) && !((test >> 4) & 0xf) && ((test >> 8) & 0xf) && !(test >> 12)) {
			for (int key = 0; key <= 0xf; key++) {
				int tRP = ((RP >> 8) & 0xf) ^ key;
				int tRP2 = ((RP2 >> 8) & 0xf) ^ key;

				tRP = InverseSbox[tRP];
				tRP2 = InverseSbox[tRP2];

				if ((tRP ^ tRP2) == 0x4) keytable[1][key] += 1;
			}
		}
	}

	// for keytable2
	for (int i = 0; i <= 0xffff; i++) {
		P = i;
		P2 = P ^ 0x100;

		Encryption(P, &RP);
		Encryption(P2, &RP2);
		int test = RP ^ RP2;
		if (!(test & 0xf) && ((test >> 4) & 0xf) && !((test >> 8) & 0xf) && !(test >> 12)) {
			for (int key = 0; key <= 0xf; key++) {
				int tRP = ((RP >> 4) & 0xf) ^ key;
				int tRP2 = ((RP2 >> 4) & 0xf) ^ key;

				tRP = InverseSbox[tRP];
				tRP2 = InverseSbox[tRP2];

				if ((tRP ^ tRP2) == 0x4) keytable[2][key] += 1;
			}
		}
	}


	// for keytable3
	for (int i = 0; i <= 0xffff; i++ ) {
		P = i;
		P2 = P ^ 0x1000;

		Encryption(P, &RP);
		Encryption(P2, &RP2);
		int test = RP ^ RP2;
		if (test&0xf && !((test>>4)&0xf) && !((test>>8)&0xf) && !(test>>12)) {
			for (int key = 0; key <= 0xf; key++) {
				int tRP = (RP & 0xf) ^ key;
				int tRP2 = (RP2 & 0xf) ^ key;

				tRP = InverseSbox[tRP];
				tRP2 = InverseSbox[tRP2];

				if ((tRP ^ tRP2) == 0x4) keytable[3][key] += 1;
			}
		}
	}

	printf("Key: 0x%X%X%X%X", max(keytable[0]), max(keytable[1]), max(keytable[2]), max(keytable[3]));
	
	printf("\n===== DC Table =====\n");
	
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
	}
	
	// Encryption(Plaintext, &Ciphertext);
	
	// printf("Plaintext = %04X, Ciphertext = %04X\n", Plaintext, Ciphertext);

	return 0;
}