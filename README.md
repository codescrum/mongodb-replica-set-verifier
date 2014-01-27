Replica Set Verifier
====================

RSV is a unix service that checks if your replica set is running, this chunk of code is based on C.

####SAMPLE BUILD:

    $ make
    $ sudo make install

####REQUIREMENTS:

LIBBSON 0.4.0

MONGO-C-DRIVER 0.8.1

####CONFIG FILE:

    $ /etc/replica_set_verifier.conf 

####EXECUTE:

    $ su -c "cd /opt/replica_set_verifier; ./replica_set_verifier"
