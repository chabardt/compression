#include <utils.h>
#include "../compression.h"

typedef struct {
	void* symbol ;
	char* translation ;
	int occurence ;
} symbol ;

typedef struct {
	int sigma_size ;
	pqueue* symbol ;
} code ;

void forward_analyze (code* c, void** alphabet, int symbol_size, void* (*read) ());
void huffman_tree (code* c, void ** alphabet, int symbol_size) ;
char* code_to_str (code) ;
char* look_up_translation (char* symbol, code c) ;

void huffman_encoding (void** alphabet, int symbol_size, int size, void* (*read) (), void (*write) (char*), void (*rewind) ()) {
	char* translation ;
	char* symbol ;

	code c ;
	forward_analyze (&c, alphabet, symbol_size, read) ;
	huffman_tree (&c, alphabet, symbol_size) ;
	write (code_to_str (c)) ;

	rewind () ;

	while ((symbol = read () ) != NULL) {
		translation = look_up_translation(symbol, c) ;
		write (translation) ;
	}
}

void read_code (code *c, int symbol_size, void* (*read) ()) ;
void translate (code c, void* (*read) (), void (*write) ()) ;

void huffman_decoding (int symbol_size, void* (*read) (), void (*write) (char*)) {
	code c ;
	read_code (&c, symbol_size, read) ;
	translate (c, read, write) ;
}

void forward_analyze (code* c, void** alphabet, int symbol_size, void* (*read) ()) {

}

void huffman_tree (code* c, void ** alphabet, int symbol_size) {

}

char* code_to_str (code) {

}

char* look_up_translation (char* symbol, code c) {

}

void read_code (code *c, int symbol_size, void* (*read) ()) {

}

void translate (code c, void* (*read) (), void (*write) ()) {

}

