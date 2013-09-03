#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include <stddef.h>

void huffman_encoding (void** alphabet, int symbol_size, int size, void* (*read) (), void (*write) (char*), void (*rewind) ()) ;
void huffman_decoding (int symbol_size, void* (*read) (), void (*write) (char*)) ;

#endif
