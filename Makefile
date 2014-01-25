IDIR =/usr/local/include
CC=gcc
CFLAGS=-I$(IDIR) -L$(LDIR)

LDIR =/usr/local/lib

LIBS=-lmongoc

OBJ=replica_set_verifier.c
OBJ_=$(basename $(OBJ))

replica_set_verifier: $(OBJ)
	gcc --std=c99 -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean install

install:
	echo "$(OBJ_)"

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 