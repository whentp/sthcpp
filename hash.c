#include "hash.h"
 
void prepareCryptTable() { 
	unsigned int seed = 0x00100001, index1, index2, i;
	for( index1 = 0; index1 < 0x100; index1++ ) { 
		for( index2 = index1, i = 0; i < 5; i++, index2 += 0x100 ) { 
			unsigned int temp1, temp2;
			seed = (seed * 125 + 3) % 0x2AAAAB;
			temp1 = (seed & 0xFFFF) << 0x10;
			seed = (seed * 125 + 3) % 0x2AAAAB;
			temp2 = (seed & 0xFFFF);
			cryptTable[index2] = ( temp1 | temp2 ); 
		} 
	} 
}

unsigned int hashString(const char *raw_string, unsigned int dwHashType ){ 
	unsigned char *key  = (unsigned char *)raw_string;
	unsigned int seed1 = 0x7FED7FED;
	unsigned int seed2 = 0xEEEEEEEE;
	int ch;

	while( *key != 0 )
	{ 
		ch = *key++;
		seed1 = cryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3; 
	}
	return seed1; 
}

unsigned int hashMem(const char *raw_string, size_t len, unsigned int dwHashType ){ 
	unsigned char *key  = (unsigned char *)raw_string;
	unsigned int seed1 = 0x7FED7FED;
	unsigned int seed2 = 0xEEEEEEEE;
	int ch;

	while((len--)> 0){ 
		ch = *key++;
		seed1 = cryptTable[(dwHashType << 8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3; 
	}
	return seed1; 
}

unsigned int cryptTable[0x500];///< required by hashString and hashString
