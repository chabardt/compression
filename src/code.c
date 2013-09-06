#include "code.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

entry* entry_new (	size_t symbol_size,
			unsigned char* _s,
			binary *_b,
			unsigned int _o) 
{
	entry *e = NULL ;
	e = (entry*) malloc (sizeof (*e) ) ;
	assert (e != NULL) ;
	e->s = (unsigned char*) malloc (symbol_size) ;
	e->b = _b ;
	memcpy (e->s, _s, symbol_size) ; e->occurence = _o ;
	return e;
}

void entry_delete (entry* e) 
{
	if (e != NULL) {
		free (e->s) ;
		binary_delete (e->b) ;
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
	fprintf (stderr, "%s ---> ", ((entry*)e)->s) ;
	binary_display(((entry*)e)->b) ;
	fprintf (stderr, "\n") ;
}

entry* look_up_entry_symbol (unsigned char* symbol, code *c) {
	assert (symbol != NULL) ;
	int i ;
	for (i=0; i<c->sigma_size; ++i) {
		if (memcmp (symbol, ((entry*)c->table->data[i])->s, c->symbol_size) == 0) {
			return ((entry*)c->table->data[i]);
		}
	}

	return NULL;
}

entry* look_up_entry_binary (binary* b, code *c) {
	assert (b != NULL) ;
	int i ;
	for (i=0; i<c->sigma_size; ++i) {
		if (memcmp (b->bytes, ((entry*)c->table->data[i])->b->bytes, b->l) == 0) {
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


	do {
		read (symbol) ;
	
		tmp = 1 ;
		i = 0 ;
		while (i < alphabet_size*symbol_size && (tmp = memcmp (symbol, alphabet+i*symbol_size, symbol_size)) != 0) {
			i++ ;
		}
		if (tmp == 0) {
			if (array[i] == NULL) {
				array[i] = entry_new(symbol_size, symbol, NULL, 1);
			} else {
				array[i]->occurence ++ ;
			}
		}
	} while (memcmp(symbol, symbol_eof, symbol_size) != 0) ;

	c->table = pqueue_new_with_data (entry_cmp, (void**) array, alphabet_size) ;
	c->sigma_size = c->table->size ;
	c->symbol_size = symbol_size ;
	c->symbol_eof = (unsigned char*) malloc (symbol_size * sizeof (unsigned char)) ;
	memcpy(c->symbol_eof, symbol_eof, symbol_size) ;

	for (i=0; i<alphabet_size; i++) ;
	free (symbol) ;

	rewind () ;

	return c; 
}

void encode (	code *c,
		void (*read) (unsigned char*),
		void (*write) (unsigned char*, size_t)) 
{

	entry *e ;
	unsigned char* symbol = (unsigned char*) malloc (c->symbol_size * sizeof (unsigned char)) ;

	size_t buffer_size = 16 ; // 4 bytes
	binary* buffer = binary_new_with_str ("", buffer_size) ;
	int res, pos = 0 ;

	do {
		read (symbol) ;

		e = look_up_entry_symbol (symbol, c) ;

		res = binary_cpy (buffer, e->b, pos) ;

		if (res <= 0) {
			// buffer is full. We send it and set the buffer to 0.
			write (buffer->bytes, buffer->l/8) ;
			binary_clear (buffer) ;
			pos = 0 ;
		} else pos += e->b->l ;

		if ( res < 0) {
			// the current symbol wasn't written complety in buffer so we write the remaining in the zero-ed buffer with a negativ offset
			binary_cpy (buffer, e->b, 0-e->b->l-res) ;
			pos -= res ;
		}

		size_encoded += e->b->l ;
	} while (memcmp (symbol, c->symbol_eof, c->symbol_size) != 0) ;

	// Copying the end of the non-finished buffer.
	if (res > 0) {	
		write (buffer->bytes, ceil (((float)pos)/8.)) ;

	}

	free (symbol) ;
}

void decode (	code *c,
		void (*read) (unsigned char*),
		void (*write) (unsigned char*, size_t)) 
{
	entry* e ; 
	unsigned char* symbol = (unsigned char*) malloc (c->symbol_size * sizeof (unsigned char)) ;
	size_t buffer_size = 16 ;
	binary* buffer = binary_new_with_str ("", buffer_size) ;

	
	do {
		
	}

}

void code_delete (code *c) 
{
	if (c != NULL) {
		pqueue_delete (c->table) ;
		free (c) ;
	}
}
