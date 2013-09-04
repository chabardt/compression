SRC	= $(wildcard *.c)
OBJ	= $(SRC:.c=.o)
EXE	= $(SRC:.c=.exe)

all: $(EXE)

%.exe: %.c
	$(CC) $(CFLAGS) -o ../$@ $< ../$(LIB) -L$(LIB_PATH) -lutils

clean:
	rm -f ../*.exe
