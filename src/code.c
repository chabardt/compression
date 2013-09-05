#include "code.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

entry* entry_new (	size_t symbol_size,
			unsigned char* _s,
			unsigned char* _k,
			size_t _l,
			unsigned int _o) 
{
	entry *e = NULL ;
	e = (entry*) malloc (sizeof (*e) ) ;
	assert (e != NULL) ;
	e->s = (unsigned char*) malloc (symbol_size) ;
	e->k = (unsigned char*) malloc (_l) ;
	memcpy (e->s, _s, symbol_size) ; memcpy (e->k, _k, _l) ; e->l = _l ; e->occurence = _o ;
	return e;
}

void entry_delete (entry* e) 
{
	if (e != NULL) {
		free (e->s) ;
		free (e->k) ;
		free (e) ;
	}
}

int entry_cmp (	const void* e1,
		const void *e2) 
{
	return ((entry*)e2)->occurence - ((entry*)e1)->occurence;
}

void entry_display (const void* e) 
{
	int i ;
	fprintf (stderr, "%s ---> %d ", ((entry*)e)->s, ((entry*)e)->l) ;
	for (i=0; i<(((entry*)e)->l)/8+1; ++i) {
		fprintf (stderr, "%d ", *((entry*)e)->k) ;
	}
	fprintf (stderr, "\n") ;
}

entry* look_up_entry (unsigned char* symbol, code *c) {
	assert (symbol != NULL) ;
	int i ;
	for (i=0; i<c->sigma_size; ++i) {
		if (memcmp (symbol, ((entry*)c->table->data[i])->s, c->symbol_size) == 0) {
			return ((entry*)c->table->data[i]);
		}
	}

	return NULL;
}

code* forward_analyze (	unsigned char *alphabet,
			size_t alphabet_size,
			size_t symbol_size,
			unsigned char *symbol_eof,
			void (*read) (unsigned char*),
			void (*rewind) ())
{
	// use of a array to avoid successiv insertion in pqueue.
	code* c = (code*) malloc (sizeof(code)) ; 
	entry **array = calloc (alphabet_size, sizeof(entry*)) ;
	unsigned char *symbol = malloc (symbol_size*sizeof(unsigned char)) ;
	unsigned int i ; // used for looping.
	int tmp ;

	read (symbol) ;

	while (memcmp(symbol, symbol_eof, symbol_size) != 0) {
		tmp = 1 ;
		i = 0 ;
		while (i < alphabet_size*symbol_size && (tmp = memcmp (symbol, alphabet+i*symbol_size, symbol_size)) != 0) {
			i++ ;
		}
		if (tmp == 0) {
			if (array[i] == NULL) {
				array[i] = entry_new(symbol_size, symbol, NULL, 0, 1);
			} else {
				array[i]->occurence ++ ;
			}
		}
		read (symbol) ;
	}

	c->table = pqueue_new_with_data (entry_cmp, (void**) array, alphabet_size) ;
	c->sigma_size = c->table->size ;
	c->symbol_size = symbol_size ;
	c->symbol_eof = (unsigned char*) malloc (symbol_size * sizeof (unsigned char)) ;
	memcpy(c->symbol_eof, symbol_eof, symbol_size) ;

	for (i=0; i<alphabet_size; i++) ;
	free (symbol) ;
	// free (array) ; IMPORTANT TO DO ! memory leak i guess from here, need to modify pqueue to use memcpy.

	rewind () ;

	return c; 
}

/*
	binary number of binary_size bits.
	buffer of buffer_size bytes
	offset can be negative.
	return the remaining capacity of buffer. if negativ then buffer overflow
*/
int write_binary_in_buffer (unsigned char *binary, size_t binary_size, unsigned char *buffer, size_t buffer_size, size_t offset) {
	int i = 0 ;
	while (i < ceil(binary_size/8)) {
		if (offset/8.+i < buffer_size && floor((float)offset/8.)+i >= 0) {
			buffer[offset/8+i] |= binary[i/8] >> (offset%8) ;
		}
		if (offset/8+i+1 < buffer_size && floor((float)offset/8.)+i+1 >= 0) {
			buffer[offset/8+i+1] |= binary[i/8] << (8-(offset%8)) ;
		}
		++i ;
	}
	return buffer_size - (offset + binary_size) ;
}

void encode (	code *c,
		void (*read) (unsigned char*),
		void (*write) (unsigned char*, size_t)) 
{
	entry *e ;
	unsigned char* symbol = (unsigned char*) malloc (c->symbol_size * sizeof (unsigned char)) ;
//	unsigned int size_encoded = 0 ;
	size_t buffer_size = 1 ;
	unsigned int size_encoded = 0 ;
	unsigned char* buffer = calloc (buffer_size, sizeof (unsigned char) ) ;
	int pos=0, res, i ;

	pqueue_display (c->table, entry_display) ;

	read (symbol) ;
	while (memcmp (symbol, c->symbol_eof, c->symbol_size) != 0) {
		e = look_up_entry (symbol, c) ;

		res = write_binary_in_buffer (e->k, e->l, buffer, buffer_size, pos) ;
		if (res <= 0) {
			// buffer is full. We send it and set the buffer to 0.
			write (buffer, buffer_size) ;
			for (i=0; i<buffer_size; ++i) {
				buffer[i] = 0 ;
			}
			pos = 0 ;
		} else pos += e->l ;
		if ( res < 0) {
			// the current symbol wasn't written complety in buffer so we write the remaining in the zero-ed buffer with a negativ offset
			write_binary_in_buffer (e->k, e->l, buffer, buffer_size, res) ;
			pos = res+e->l ;
		}

		size_encoded += e->l ;
		read (symbol) ;
	}

	fprintf (stderr, "size of compressed data : %d\n", size_encoded) ;

	free (symbol) ;
}

void decode (	code *c,
		void (*read) (unsigned char*),
		void (*write) (unsigned char*, size_t)) 
{
	
}

void code_delete (code *c) 
{
	if (c != NULL) {
		pqueue_delete (c->table) ;
		free (c) ;
	}
}
