/*
 * Replica Set Verifier: check if your replica set is running
 *
 * SAMPLE BUILD:
 * gcc --std=c99 -I/usr/local/include -L/usr/local/lib -o replica_set_verifier replica_set_verifier.c -lmongoc
 *
 * REQUIREMENTS:
 * LIBBSON 0.4.0
 * MONGO-C-DRIVER 0.8.1
 * 
 * CONFIG FILE:
 * replica_set_verifier.conf 
 *
 * EXECUTE:
 * sudo ./replica_set_verifier
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "mongo.h"

int main(int argc, char* argv[]){
    FILE *fp= NULL;
    FILE *pid_file= NULL;
    pid_t process_id = 0;
    pid_t sid = 0;
    // Create child process
    process_id = fork();
    // Indication of fork() failure
    if (process_id < 0){
        printf("fork failed!\n");
        // Return failure in exit status
        exit(1);
    }
    // PARENT PROCESS. Need to kill it.
    if (process_id > 0){
        printf("process_id of child process %d \n", process_id); 
        // Create a file with pid into itself
        pid_file = fopen ("rs_verifier.pid", "w+");
        fprintf(pid_file, "%d", process_id);
        fflush(pid_file);
        // return success in exit status
        exit(0);
    }
    //unmask the file mode
    umask(0);
    //set new session
    sid = setsid();
    if(sid < 0){
        // Return failure
        exit(1);
    }
    // Open a log file in write mode.
    fp = fopen ("/var/log/rs_verifier.log", "w+");
    // Change the current working directory to root.
    chdir("/");
    // Close stdin. stdout and stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    // Mongo connection
    mongo conn[1];
    int result;
    int timer;
    int TIMELIMIT = 10;
    int TIMEOUT = 5;

    mongo_replica_set_init( conn, "rs" );
    mongo_replica_set_add_seed( conn, "192.168.0.108", 27017 );
    mongo_replica_set_add_seed( conn, "192.168.0.109", 27017 );
    mongo_replica_set_add_seed( conn, "192.168.0.110", 27017 );

    // Trying to establish the connection
    result = mongo_replica_set_client( conn );
    timer = 0;
    while(result != MONGO_OK || timer==TIMELIMIT){
      switch ( conn->err ) {
        case MONGO_CONN_NO_SOCKET:    fprintf(fp, "no socket\n" ); break;
        case MONGO_CONN_FAIL:         fprintf(fp, "connection failed\n" ); break;
        case MONGO_CONN_ADDR_FAIL:    fprintf(fp, "error occured while calling getaddrinfo().\n" ); break;
        case MONGO_CONN_BAD_SET_NAME: fprintf(fp, "Given rs name doesn't match this replica set.\n" ); break;
        case MONGO_CONN_NO_PRIMARY:   fprintf(fp, "Can't find primary in replica set.\n" ); break;
        case MONGO_CONN_NOT_MASTER:   fprintf(fp, "not master\n" ); break;
      }
      fflush(fp);
      timer++;
      result = mongo_replica_set_client( conn );
      sleep(TIMEOUT); 
      fprintf(fp,"Checking the connection...\n");
    }  
    fflush(fp);
    fprintf(fp, "connection OK!.\n" );
    mongo_destroy( conn );
    fclose(fp);
    return (0);
}