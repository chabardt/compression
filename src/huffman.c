#include <utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../compression.h"

typedef struct {
	unsigned char* s ; // symbol
	unsigned char* k ; // translation
	int l ; // length of translation
	int occurence ;
} entry ;

entry* entry_new (int symbol_size, unsigned char* _s, unsigned char* _k, int _l, int _o) {
	entry *e = NULL ;
	e = (entry*) malloc (sizeof (*e) ) ;
	assert (e != NULL) ;
	e->s = (unsigned char*) malloc (symbol_size) ;
	e->k = (unsigned char*) malloc (_l) ;
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
	return ((entry*)e2)->occurence - ((entry*)e1)->occurence;
}

void entry_display (const void* e) {
	fprintf (stderr, "%c ---> %d\n", ((entry*)e)->s[0], ((entry*)e)->occurence) ;
}

typedef struct {
	int sigma_size ;
	int symbol_size ;
	pqueue* table ; // priority queue of entries.
} code ;

int forward_analyze (code* c, unsigned char* alphabet, int alphabet_size, int symbol_size, int symbol_eof, void (*read) (unsigned char*));
void huffman_tree (code* c) ;
unsigned char* code_to_output (code c, void write (unsigned char*, int)) ;
entry* look_up_entry (unsigned char* symbol, code* c) ;

void huffman_encoding (unsigned char* alphabet, int alphabet_size, int symbol_size, int symbol_eof, void (*read) (unsigned char*), void (*write) (unsigned char*, int), void (*rewind) ()) {
	entry* e ;
	unsigned char* symbol = malloc (symbol_size*sizeof(unsigned char)) ;
	unsigned char* buffer ;
	int size_encoded = 0 ;

	code c ;
	int number_of_read_symbol = forward_analyze (&c, alphabet, alphabet_size, symbol_size, symbol_eof, read) ;
	huffman_tree (&c) ;
//	code_to_output (c, write) ;

	rewind () ;

	fprintf (stderr, "size of text : %d\n", number_of_read_symbol*8);


	buffer = malloc (symbol_size*number_of_read_symbol*sizeof(unsigned char)) ;
	read (symbol) ;
	while (memcmp(symbol, alphabet+symbol_eof*symbol_size, symbol_size) != 0) {
		e = look_up_entry(symbol, &c) ;
		size_encoded += e->l;	
		read (symbol) ;
	}
	fprintf (stderr, "size of compressed data : %d\n", size_encoded) ;
//	write (e->k, e->l) ;

	free (symbol) ;
}

void read_code (code *c, void (*read) (unsigned char*)) ;
void translate (code c, void (*read) (unsigned char*), void (*write) (unsigned char*, int)) ;

void huffman_decoding (void (*read) (unsigned char*), void (*write) (unsigned char*, int)) {
	code c ;
	read_code (&c, read) ;
	translate (c, read, write) ;
}

int forward_analyze (code* c, unsigned char* alphabet, int alphabet_size, int symbol_size, int eof_symbol, void (*read) (unsigned char*)) {
	entry** array = calloc (alphabet_size, sizeof(entry*)) ; // use of a array to avoid successiv insertion in pqueue. 
	unsigned char* symbol = malloc (symbol_size*sizeof(unsigned char)) ;
	int i ; // used for looping.
	int result =0 ;
	int tmp ;

	read (symbol) ;


	while (memcmp(symbol, alphabet+eof_symbol*symbol_size, symbol_size) != 0) {
		result++;

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

	free (symbol) ;

	return result; 
}

// structure for huffman binary tree.
typedef struct htree htree ;
struct htree {
	unsigned char* symbol ;
	unsigned char* code ; 
	int weight ; // basically the occurence of symbol or added weight of childs.
	htree* parent ;
	htree* left ;
	htree* right ;
} ;

htree* htree_new (int symbol_size, unsigned char* symbol, int weight, htree* parent, htree* left, htree* right ) {
	htree *t = NULL ;
	t = (htree*) malloc (sizeof (*t) ) ;
	assert (t != NULL) ;
	if (symbol != NULL) { 
		t->symbol = (unsigned char*) malloc (symbol_size) ;
		memcpy (t->symbol, symbol, symbol_size) ;
	}
	t->parent = parent; t->left = left ; t->right = right ;
	t->weight = weight ;

	return t ;
}

void htree_delete (htree* t) {
	if (t != NULL) {
		free (t->symbol) ;
		if (t->left != NULL) htree_delete (t->left) ;
		if (t->right != NULL) htree_delete (t->right) ;

		free (t) ; 
	} 
}

int htree_cmp (const void* t1, const void* t2) {
	if (t1 == NULL && t2 == NULL) return 0 ;
	if (t1 == NULL) return -((htree*)t2)->weight ;
	if (t2 == NULL) return ((htree*)t1)->weight ;

	return ((htree*)t2)->weight - ((htree*)t1)->weight ;
}

// end of structure

void huffman_tree (code* c) {
	pqueue* merged_forest ;
	pqueue* single_node_forest ;

	single_node_forest = pqueue_new (c->table->size, htree_cmp) ;
	merged_forest = pqueue_new (c->table->size, htree_cmp) ; // Check for correct maximal size of parents. This is just a guess.
	htree* t1,* t2 ; // to build the huffman tree.
	htree* root, *current; // to get the code for each symbol in the second pass.

	int i=0 ;
	// copy because element already heapified in c->table... 
	for (i=0; i<c->table->size; ++i) {
		single_node_forest->data[i] = (void*)htree_new (c->symbol_size, ((entry*)c->table->data[i])->s, ((entry*)c->table->data[i])->occurence, NULL, NULL, NULL) ;
	}
	single_node_forest->size = c->table->size ;

	while (pqueue_size (single_node_forest) + pqueue_size (merged_forest) != 1 ) {
		if (htree_cmp (pqueue_peek(single_node_forest), pqueue_peek (merged_forest)) > 0 ) {
			t1 = pqueue_pop (single_node_forest) ;
		} else {
			t1 = pqueue_pop (merged_forest) ;

		}
		if (htree_cmp (pqueue_peek(single_node_forest), pqueue_peek (merged_forest)) > 0 ) {
			t2 = pqueue_pop (single_node_forest) ;

		} else {
			t2 = pqueue_pop (merged_forest) ;

		}
		current = htree_new (0, NULL, t1->weight + t2->weight, NULL, t1, t2) ;
		t1->parent = current ;
		t2->parent = current ;
		pqueue_insert (merged_forest, (void*) current ) ;
		
	}
	
	if (pqueue_size (merged_forest) == 1) {
		root = pqueue_pop (merged_forest) ;
	} else {
		root = pqueue_pop (single_node_forest) ;
	}

	// Get code for each symbol. Might be done more efficiently ... O ( n² ). anyway n smaller than the size of entire input to check. this way we amortize the look up for a symbol as it's already sorted in table by frequencies.
	current = root;
	i = 0 ;
	size_t upper_bound = 15; // should set the size to something closer to the real limit depending on |sigma|.
	unsigned char* tmp = (unsigned char *) malloc (upper_bound*sizeof(unsigned char)) ; 	
	entry * e;
	int tmp_l;

	for (i=0; i<15; ++i) {tmp[i] = 0 ;} 
	i=0 ;

	while (root->left != NULL || root->right != NULL) {
		// exploring top down: until a leaf is reached.
		while (current->left != NULL || current->right != NULL) {
			// first on left.
			if (current->left != NULL) {
				current = current->left ;
				i++;
			}
			// then on right.
			else if (current->right != NULL) {
				current = current->right ;
				// adding one at ith position in temp
				tmp[i/8] |= 128 >> i%8; 
				i++;
			}
		}
		// we reached a leaf. We have the code for symbol, just need to copy it in the right place.
		e = look_up_entry (current->symbol, c) ;
		e->l = i;
		tmp_l = ceilf((float)i/8.) ;
		e->k = malloc (tmp_l*sizeof(unsigned char)) ;
		memcpy (e->k, tmp, tmp_l) ;

		// going back to next unexplored direction.
		while (current->parent != NULL && current->parent->right == current) {
			current = current->parent ;
			htree_delete (current->right) ;
			current->right = NULL ;
			i-- ;
			tmp[i/8] &= ~(128 >> i%8) ;
		}
		if (current == root) break ;
		current = current->parent ;
		htree_delete (current->left) ;
		current->left = NULL ;
		i-- ; 


	}
}

void code_to_str (code c, void (*write) (unsigned char*, int)) {
}

entry* look_up_entry (unsigned char* symbol, code *c) {
	assert (symbol != NULL) ;
	int i=0;
	for (i=0; i<c->sigma_size; ++i) {
		if (memcmp (symbol, ((entry*)c->table->data[i])->s, c->symbol_size) == 0) {
			return ((entry*)c->table->data[i]);
		}
	}

	return NULL;
}

void read_code (code *c, void (*read) (unsigned char*)) {

}

void translate (code c, void (*read) (unsigned char*), void (*write) (unsigned char*, int)) {

}

