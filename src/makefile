SRC	= $(wildcard *.c)
OBJ	= $(SRC:.c=.o)
HEA	= $(wildcard *.h) ../$(HEADER)

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(AFLAGS) ../$(LIB) $(OBJ)

huffman.o: huffman.c
	$(CC) -o $@ -c $< -lutils $(CFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.o




