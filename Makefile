IDIR =/usr/local/include
CC=gcc
CFLAGS=-I$(IDIR) -L$(LDIR)

ODIR=obj
LDIR =/usr/local/lib

LIBS=-lmongoc

OBJ=replica_set_verifier.c

replica_set_verifier: $(OBJ)
	gcc --std=c99 -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 