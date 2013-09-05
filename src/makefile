SRC	= $(wildcard *.c)
OBJ	= $(SRC:.c=.o)
HEA	= $(wildcard *.h) ../$(HEADER)

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) $(AFLAGS) ../$(LIB) $(OBJ)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f *.o




