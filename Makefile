IDIR =/usr/local/include
CC=gcc
CFLAGS=-I$(IDIR) -L$(LDIR)

LDIR =/usr/local/lib

LIBS=-lmongoc

OBJ=replica_set_verifier.c
OBJ_=$(basename $(OBJ))

MKFILE_PATH = $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR = $(shell pwd)

REAL_DIR = /opt/$(OBJ_)
CONF_PATH = /etc/$(OBJ_).conf

replica_set_verifier: $(OBJ)
	gcc --std=c99 -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: install

install:
	mkdir -p $(REAL_DIR)
	cp $(CURRENT_DIR)/$(OBJ_) $(REAL_DIR)/$(OBJ_)
	cp $(CURRENT_DIR)/$(OBJ_).template $(CONF_PATH)
	cp $(CURRENT_DIR)/serviwer.sh $(REAL_DIR)/serviwer.sh
	rm $(CURRENT_DIR)/$(OBJ_)