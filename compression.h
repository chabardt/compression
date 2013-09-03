#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include <stddef.h>

void huffman_encoding (void** alphabet, int alphabet_size, int symbol_size, int size, void* (*read) (), void (*write) (char*), void (*rewind) ()) ;
void huffman_decoding (void* (*read) (), void (*write) (char*)) ;

#endif
