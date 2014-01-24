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
 * OUTPUT FILES:
 * replica_set_verifier (bin)
 *
 * OUTPUT FILES (when replica_set_verifier is excuted):
 * replica_set_verifier.log (/var/log)
 * replica_set_verfier.pid
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include "mongo.h"

#define MAXLEN 80
#define CONFIG_FILE "/etc/replica_set_verifier.conf"

/*
* create the struct which encapsulates each server config in replica set
*/
struct rs_server
{
  char address[MAXLEN];
  char port[MAXLEN];
}
  rs_server;

/*
* create the struct which encapsulates the standalone replica set config
*/
struct replica_set
{
  char name[MAXLEN];
  char number_of_servers[MAXLEN];
}
  replica_set;

/*
 * initialize one instance of server with default values
 */
void
initiate_rs_server (struct rs_server * server){
  strncpy (server->address, "127.0.0.1", MAXLEN);
  strncpy (server->port, "27017", MAXLEN);
}

/*
 * initialize one instance of replica_set with default values
 */
void
initiate_replica_set (struct replica_set * replica){
  strncpy (replica->name, "rs", MAXLEN);
  strncpy (replica->number_of_servers, "0", MAXLEN);
}


/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
char *trim(char *str){
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

/*---------- Config Parser Funcitons ----------*/
/*
 *  get replica set configuration
 */
void
replica_config (struct replica_set * replica, FILE *logger)
{
  char *s, buff[256];
  FILE *fp = fopen (CONFIG_FILE, "r");
  if (fp == NULL)
  {
    fprintf(logger, "/etc/replica_set_verifier.conf: no such file...");
    return;
  }
  while ((s = fgets (buff, sizeof buff, fp)) != NULL)
  {
    if (buff[0] == '\n' || buff[0] == '#')
      continue;
    char name[MAXLEN], value[MAXLEN];
    s = strtok (buff, "=");
    if (s==NULL)
      continue;
    else
      strncpy (name, s, MAXLEN);
    s = strtok (NULL, "=");
    if (s==NULL)
      continue;
    else
      strncpy (value, s, MAXLEN);
    trim (value);
    if (strcmp(name, "number_of_servers" )==0){
      strncpy (replica->number_of_servers, value, MAXLEN);
    }else if (strcmp(name, "name" )==0){
      strncpy (replica->name, value, MAXLEN);
    }
  }
  fclose (fp);
}



/*
 * create array of servers of replica set
 */
void
parse_config (struct rs_server *servers, int number_of_servers)
{
  char *s, buff[256], *a;
  FILE *fp = fopen (CONFIG_FILE, "r");
  int counter = 0;
  if (fp == NULL)
  {
    return;
  }
  while ((s = fgets (buff, sizeof buff, fp)) != NULL)
  {
    if (buff[0] == '\n' || buff[0] == '#')
      continue;
    char name[MAXLEN], value[MAXLEN];
    s = strtok (buff, "=");
    if (s==NULL)
      continue;
    else
      strncpy (name, s, MAXLEN);
    s = strtok (NULL, "=");
    if (s==NULL)
      continue;
    else
      strncpy (value, s, MAXLEN);
    trim (value);
    if (strcmp(name, "path")==0){
      char address[MAXLEN], port[MAXLEN];
      a = strtok (value, ":");

      if (a==NULL)
        continue;
      else
        strncpy (address, a, MAXLEN);
      a = strtok (NULL, ":");
      if (a==NULL)
        continue;
      else
        strncpy (port, a, MAXLEN);
      trim (port);
      strncpy (servers[counter].address, address, MAXLEN);
      strncpy (servers[counter].port, port, MAXLEN);
      counter++;
      if (counter >= number_of_servers){
        return;
      }
    }
    else
      printf ("WARNING: %s/%s: Unknown name/value pair!\n",name, value);
  }
  fclose (fp);
}

int main(int argc, char* argv[]){
    /*------------------- Daemonize -------------------*/
    FILE *fp= NULL;
    FILE *pid_file= NULL;
    FILE *token= NULL;
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
        pid_file = fopen ("/tmp/replica_set_verifier.pid", "w+");
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
    // Change the current working directory to root.
    chdir("/");
    // Close stdin. stdout and stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    // Open a log file in write mode.
    fp = fopen ("/var/log/replica_set_verifier.log", "w+");

    /*------------------- Config Parser -------------------*/    
    struct replica_set replica;

    initiate_replica_set(&replica);
    replica_config (&replica, fp);

    fprintf(fp, "Replica set name: %s Number of servers: %d \n", replica.name, atoi(replica.number_of_servers));

    struct rs_server servers[atoi(replica.number_of_servers)];

    parse_config (servers, atoi(replica.number_of_servers));

    /*------------------- Mongo Connection -------------------*/    
    // Mongo connection
    mongo conn[1];
    int status;
    int timer = 0;
    int TIMEOUT = 5;

    mongo_replica_set_init( conn, replica.name );
    // mongo_destroy( conn );
    // mongo_replica_set_init( conn, "rs" );
    int j;
    for(j = 0; j < atoi(replica.number_of_servers); j++)
    {
        fprintf(fp, "|%s:%d|\n", servers[j].address, atoi(servers[j].port));
        const char *temp_address = servers[j].address;
        mongo_replica_set_add_seed( conn, temp_address, 27017 );
    }

    // Trying to establish the connection
    status = mongo_replica_set_client( conn );
    fprintf(fp, "%d\n", status);
    while(status != MONGO_OK){
      switch ( conn->err ) {
        case MONGO_CONN_NO_SOCKET:    fprintf(fp, "no socket\n" ); break;
        case MONGO_CONN_FAIL:         fprintf(fp, "connection failed\n" ); break;
        case MONGO_CONN_ADDR_FAIL:    fprintf(fp, "error occured while calling getaddrinfo().\n" ); break;
        case MONGO_CONN_BAD_SET_NAME: fprintf(fp, "Given rs name doesn't match this replica set.\n" ); break;
        case MONGO_CONN_NO_PRIMARY:   fprintf(fp, "Can't find primary in replica set.\n" ); break;
        case MONGO_CONN_NOT_MASTER:   fprintf(fp, "not master\n" ); break;
      }
      fflush(fp);
      fprintf(fp,"Checking the connection... status( %d )\n", status);
      sleep(TIMEOUT);  
      status = mongo_replica_set_client( conn );  
    }  
    fflush(fp);
    fprintf(fp, "connection OK!.\n" );
    fprintf(fp, "Setting token.\n" );
    // Open a token file in write mode.
    token = fopen ("/tmp/token.rsv", "w+");
    fprintf(token, "true");
    fflush(token);
    mongo_destroy( conn );
    fprintf(fp, "ending...\n" );
    fclose(fp);
    fclose(token);
    return (0);
}