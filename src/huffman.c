#include "../compression.h"
#include "code.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct htree htree ;

struct htree {
	unsigned char *symbol ; // NULL if not a leaf.
	unsigned int weight ; // the occurence of symbol or added weight of childs.
	htree *parent ;
	htree *left ;
	htree *right ;
} ;

htree* htree_new (	size_t symbol_size,
			unsigned char *symbol,
			unsigned int weight,
			htree *parent, htree *left, htree *right )
{
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

void htree_delete (htree *t) {
	if (t != NULL) {
		free (t->symbol) ;
		if (t->left != NULL) htree_delete (t->left) ;
		if (t->right != NULL) htree_delete (t->right) ;

		free (t) ; 
	} 
}

int htree_cmp (const void *t1, const void *t2) {
	if (t1 == NULL && t2 == NULL) return 0 ;
	if (t1 == NULL) return -((htree*)t2)->weight ;
	if (t2 == NULL) return ((htree*)t1)->weight ;

	return ((htree*)t2)->weight - ((htree*)t1)->weight ;
}

htree* build_huffman_tree (code *c) ;
void explore_tree (code *c, htree *root) ;

code* huffman_analyze (	unsigned char *alphabet,
			size_t alphabet_size,
			size_t symbol_size,
			unsigned char *symbol_eof,
			void (*read) (unsigned char*),
			void (*rewind) ())
{

	code *c = forward_analyze (alphabet, alphabet_size, symbol_size, symbol_eof, read, rewind) ;
	htree* root = build_huffman_tree (c) ;
	explore_tree (c, root) ;

	return c ;
}

htree* build_huffman_tree (code *c) {
	pqueue *merged_forest ;
	pqueue *single_node_forest ;
	htree *t1, *t2, *new_tree ; // to build the huffman tree.
	int i=0 ;

	single_node_forest = pqueue_new (c->table->size, htree_cmp) ;
	merged_forest = pqueue_new (c->table->size, htree_cmp) ; // Check for correct maximal size of parents. This is just a guess.

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
		new_tree = htree_new (0, NULL, t1->weight + t2->weight, NULL, t1, t2) ;
		t1->parent = new_tree ;
		t2->parent = new_tree ;
		pqueue_insert (merged_forest, (void*) new_tree ) ;
		
	}
	
	if (pqueue_size (merged_forest) == 1) {
		return pqueue_pop (merged_forest) ;
	}
	return pqueue_pop (single_node_forest) ;
}

void explore_tree (code *c, htree *root) {
	entry *e ;
	htree *current = root ;
	//  should set this to a size related to |sigma|.
	size_t upper_bound = 15 ;
	unsigned char *tmp = (unsigned char*) calloc (upper_bound, sizeof (unsigned char)) ;
	size_t tmp_l ;
	int i=0 ;

	while (root->left != NULL || root->right != NULL) {
		// exploring top down, left right: until a leaf is reached.
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

