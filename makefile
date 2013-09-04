export CC	= gcc
export AR	= ar
export INC_PATH	= /home/theodore/Workspace/c99/include
export LIB_PATH	= /home/theodore/Workspace/c99/lib
export CFLAGS	= -I$(INC_PATH) -g -Wall -pedantic -std=c99
export AFLAGS	= -cvq
export DEST	= /home/theodore/Workspace/c99/

#export BIN	=
export LIB	= libcompression.a
export HEADER	= compression.h

$(LIB) :
	@(cd src && $(MAKE))

clean :
	@(cd src && $(MAKE) $@)
	rm -f $(LIB)
	rm -f *.exe

install :
	cp $(LIB) $(DEST)lib
#	cp $(BIN) $(DEST)bin
	cp $(HEADER) $(DEST)include

uninstall :
	rm $(DEST)lib/$(LIB) $(DEST)bin/$(BIN) $(DEST)include/$(HEADER)

test: $(LIB)
	@(cd test && $(MAKE))

.PHONY: clean install uninstall

