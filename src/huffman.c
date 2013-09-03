#include <utils.h>
#include "huffman.h"

typedef struct {
	void* symbol ;
	char* translation ;
	int occurence ;
} symbol ;

typedef struct {
	int sigma_size ;
	pqueue* symbol ;
} code ;

forward_analyze (code* c, void** alphabet, int symbol_size, (void*) (read) ());
huffman_tree (code* c, void ** alphabet, int symbol_size) ;
look_up_translation (char* symbol, code c);

void huffman_encoding (void** alphabet, int symbol_size, (void*) (read) (), (void) (write) (char*), (void) (rewind) ()) {
	int i=0 ;
	char* translation ;
	char* symbol ;

	code c;
	forward_analyze (&c, alphabet, symbol_size, size, read) ;
	huffman_tree (&c, alphabet, symbol_size) ;
	write (code_to_str (code)) ;

	rewind () ;

	for (symbol = read () ) {
		translation = look_up_translation(symbol, code) ;
		write (translation) ;
	}
}

read_code (void** alphabet, int symbol_size, (void*) (read) ()) ;
translate (void** alphabet, int symbol_size, (void*) (read) (), (void) (write) ()) ;

void huffman_decoding (void** alphabet, int symbol_size, (void*) (read) (), (void) (write) (char*)) {
	read_code (alphabet, symbol_size, read) ;
	translate (alphabet, symbol_size, read, write) ;
}
