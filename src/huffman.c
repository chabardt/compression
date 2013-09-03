#include <utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../compression.h"

typedef struct {
	void* s ; // symbol
	char* k ; // translation
	int l ; // length of translation
	int occurence ;
} entry ;

entry* entry_new (int symbol_size, void* _s, char* _k, int _l, int _o) {
	entry *e = NULL ;
	e = (entry*) malloc (sizeof (*e) ) ;
	assert (e != NULL) ;
	e->s = (void*) malloc (symbol_size) ;
	e->k = (void*) malloc (_l) ;
	memcpy (e->s, _s, symbol_size) ; memcpy (e->k, _k, _l) ; e->l = _l ; e->occurence = _o ;
	return e;
}

void entry_delete (entry* e) {
	if (e != NULL) {
		free (e->s) ;
		free (e->k) ;
		free (e) ;
	}
}

int entry_cmp (const void* e1, const void *e2) {
	return ((entry*)e1)->occurence - ((entry*)e2)->occurence;
}

typedef struct {
	int sigma_size ;
	int symbol_size ;
	pqueue* table ;
} code ;

void forward_analyze (code* c, void** alphabet, int alphabet_size, int symbol_size, void* (*read) ());
void huffman_tree (code* c, void ** alphabet, int alphabet_size, int symbol_size) ;
char* code_to_str (code c) ;
char* look_up_translation (char* symbol, code c) ;

void huffman_encoding (void** alphabet, int alphabet_size, int symbol_size, int size, void* (*read) (), void (*write) (char*), void (*rewind) ()) {
	char* translation ;
	void* symbol = NULL ;

	code c ;
	forward_analyze (&c, alphabet, alphabet_size, symbol_size, read) ;
	huffman_tree (&c, alphabet, alphabet_size, symbol_size) ;
	write (code_to_str (c)) ;

	rewind () ;

	while ((symbol = read () ) != NULL) {
		translation = look_up_translation(symbol, c) ;
		write (translation) ;
	}
}

void read_code (code *c, void* (*read) ()) ;
void translate (code c, void* (*read) (), void (*write) ()) ;

void huffman_decoding (void* (*read) (), void (*write) (char*)) {
	code c ;
	read_code (&c, read) ;
	translate (c, read, write) ;
}

void forward_analyze (code* c, void** alphabet, int alphabet_size, int symbol_size, void* (*read) ()) {
	entry** array = calloc (alphabet_size, sizeof(entry*)) ; // use of a array to avoid successiv insertion in pqueue. Plus we can reduced the size of the alphabet with only the used symbols.
	void* symbol = NULL ;
	int i=0 ; // used for looping.
	while ((symbol = read ()) != NULL) {
		while (i < alphabet_size && memcmp (symbol, alphabet[i], symbol_size) == 0) {
			i++;
		}
		if (memcmp (symbol, alphabet[i], symbol_size) == 0) {
			if (array[i] == NULL) {
				array[i] = entry_new(symbol_size, symbol, "", 0, 1);
			} else {
				array[i]->occurence ++ ;
			}
		}
	}

	c->symbol_size = symbol_size;

	c->table = pqueue_new_with_data (entry_cmp, (void**) array, alphabet_size) ;

}

void huffman_tree (code* c, void ** alphabet, int alphabet_size, int symbol_size) {
	pqueue merged_trees;
	pqueue single_node_trees;

//	node root;
}

char* code_to_str (code c) {
 	return NULL;
}

char* look_up_translation (char* symbol, code c) {
	return NULL;
}

void read_code (code *c, void* (*read) ()) {

}

void translate (code c, void* (*read) (), void (*write) ()) {

}

