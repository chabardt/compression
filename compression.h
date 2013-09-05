#ifndef _COMPRESSION_H_
#define _COMPRESSION_H_

#include <stddef.h>
#include <utils.h>

typedef struct {
	size_t sigma_size ;
	size_t symbol_size ;
	unsigned char *symbol_eof ;
	pqueue *table ; // priority queue of entries.
} code ;

code* huffman_analyze (		unsigned char *alphabet,
				size_t alphabet_size,
				size_t symbol_size,
				unsigned char* symbol_eof,
				void (*read) (unsigned char*),
				void (*rewind) ()) ;
/*
code* arithmetic_analyze (	unsigned char* alphabet,
				size_t alphabet_size,
				size_t symbol_size,
				size_t symbol_eof,
				void (*read) (unsigned char*),
				void (*rewind) ()) ;
*/
void encode (			code *c,
				void (*read) (unsigned char*),
				void (*write) (unsigned char*, size_t)) ;
void decode (			code *c,
				void (*read) (unsigned char*),
				void (*write) (unsigned char*, size_t)) ;
void code_delete (		code *c) ;

#endif
