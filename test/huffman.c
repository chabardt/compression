#include "../compression.h"
#include <stdio.h>
#include <string.h>

FILE* fp_in ;
FILE* fp_out ;

void test_read (char* c) {
	*c = fgetc (fp_in) ;
}

void test_write (char * c) {
	fputc (*c, fp_out) ;
}

void test_rewind () {
	rewind (fp_in) ;
}

int main (int argc, char* argv[]) {
	if (argc < 2) return 0 ;
	int i = 0 ;

	char ascii[128] ;
	for (i=0; i<128; ++i) {
		ascii [i] = i;
	}

	for (i=1; i<argc; ++i) {
		fp_in = fopen (argv[i], "r") ;
		if (fp_in == NULL) { fprintf (stderr, "error opening file %s.\n", argv[i]) ;}
		else {
			fp_out = fopen ("huffman_out", "w+") ;
			huffman_encoding (ascii, 128, 1, test_read, test_write, test_rewind) ;
			huffman_decoding (test_read, test_write) ;
			fclose (fp_in) ;
			fclose (fp_out) ; 
		}
	} 
}
