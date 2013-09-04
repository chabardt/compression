#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include <stddef.h>

void huffman_encoding (unsigned char* alphabet, int alphabet_size, int symbol_size, int symbol_eof, void (*read) (unsigned char*), void (*write) (unsigned char*, int), void (*rewind) ()) ;
void huffman_decoding (void (*read) (unsigned char*), void (*write) (unsigned char*, int)) ;

#endif
