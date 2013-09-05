#ifndef _CODE_H_
#define _CODE_H_

#include "../compression.h"

// Internal structure inside struct code priority queue.
typedef struct {
	unsigned char* s ; // symbol
	unsigned char* k ; // translation
	size_t l ; // length of translation
	unsigned int occurence ;
} entry ;

entry* entry_new (	size_t symbol_size,
			unsigned char* _s,
			unsigned char* _k,
			size_t _l,
			unsigned int _o) ;
void entry_delete (entry* e) ;
int entry_cmp (	const void* e1,
		const void *e2) ;
// use to print the content of pqueue (debugging)
void entry_display (const void* e) ;

entry* look_up_entry (unsigned char *symbol, code *c) ;

/*
	analyze text and fill out code struct except for fields k,l of each entries.
	Call by huffman and arithmetic analyze. 
*/
code* forward_analyze (	unsigned char* alphabet,
			size_t alphabet_size,
			size_t symbol_size,
			unsigned char* symbol_eof,
			void (*read) (unsigned char*),
			void (*rewind) ());


#endif
