#!/bin/bash
rm -f replica_set_verifier
gcc --std=c99 -I/usr/local/include -L/usr/local/lib -o replica_set_verifier replica_set_verifier.c -lmongoc
#sudo cp replica_set_verifier.template /etc/replica_set_verifier.conf 
