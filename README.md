replica_set_verifier
====================

RSV is a unix service taht checks if your replica set is running, this chunk of code is based on C, this is not a release...

####SAMPLE BUILD:

gcc --std=c99 -I/usr/local/include -L/usr/local/lib -o replica_set_verifier replica_set_verifier.c -lmongoc

####REQUIREMENTS:


LIBBSON 0.4.0

MONGO-C-DRIVER 0.8.1

####CONFIG FILE:

replica_set_verifier.conf 

####EXECUTE:

sudo ./replica_set_verifier
