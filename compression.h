#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include <stddef.h>

void huffman_encoding (char* alphabet, int alphabet_size, int symbol_size, void (*read) (char*), void (*write) (char*), void (*rewind) ()) ;
void huffman_decoding (void (*read) (char*), void (*write) (char*)) ;

#endif
