#include "../compression.h"
#include <stdio.h>
#include <string.h>

FILE* fp_in ;
FILE* fp_out ;

void test_read (unsigned char* c) {
	char c2 = fgetc (fp_in) ;
	memcpy(c, &c2, sizeof(char)) ;
}

void test_write (unsigned char * c, int length) {
	int i;
	for (i=0; i<length; i++) { 
		fputc ((char)*(c+i), fp_out) ;
	}
}

void test_rewind () {
	rewind (fp_in) ;
}

int main (int argc, char* argv[]) {
	if (argc < 2) return 0 ;
	int i = 0 ;

	unsigned char ascii[256] ; // including EOF symbol.
	for (i=0; i<256; ++i) {
		ascii [i] = i ;
	}

	for (i=1; i<argc; ++i) {
		fp_in = fopen (argv[i], "r") ;
		if (fp_in == NULL) { fprintf (stderr, "error opening file %s.\n", argv[i]) ;}
		else {
			fp_out = fopen ("huffman_out", "wb+") ;
			huffman_encoding (ascii, 256, 1, 255, test_read, test_write, test_rewind) ;
			huffman_decoding (test_read, test_write) ;
			fclose (fp_in) ;
			fclose (fp_out) ; 
		}
	} 
}
