#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "a1_lib.h"
#include "rpc.h"


#define BUFSIZE   1024


void pack_message(char *input, message_t *msg){

	int i = 0;

	char* token = strtok(input, " ");
	strcpy(msg->cmd,token);
	token = strtok(NULL, " ");

	while (token != NULL) {
		strcpy(msg->args[i], token);
    	i++;
		token = strtok(NULL, " ");
    }
}

/**
 * @brief: Connect to the server. Request message from the client. Pack message, send message. Receive message and optionally print the return message.
 * 
 * @return: int 
 */
int main(int argc, char *argv[]) {
  int sockfd;
  int no_exit = 1;
  char user_input[BUFSIZE] = { 0 };
  char server_msg[BUFSIZE] = { 0 };
  int port = atoi(argv[2]);

  if (connect_to_server(argv[1], port, &sockfd) < 0) {
    fprintf(stderr, "oh no\n");
    return -1;
  }

  while ((strcmp(user_input, "exit") != 0) && (strcmp(user_input, "shutdown") != 0)) { // while the client did not enter exit or shutdown.

    memset(server_msg, 0, sizeof(server_msg));

    // read user input from command line
    fgets(user_input, BUFSIZE, stdin);

    user_input[strlen(user_input) - 1] = 0;  //Removes \n character that fgets adds to the string.

    message_t *message_pointer; // pointer initialization of type message_t
		message_pointer = (message_t *) malloc(sizeof(message_t)); // allocate mmemory for the structure that the client will send to the server.
    pack_message(user_input, message_pointer);  // pack the input of the client into a structure.
  
    // send the input to server
    send_message(sockfd, (char *) message_pointer , sizeof(message_t)); 
    // receive a msg from the server
    ssize_t byte_count = recv_message(sockfd, server_msg, sizeof(server_msg)); 
    if (byte_count <= 0) {
      break;

       free(message_pointer); // free memory.

    } // end of while loop

    if (strcmp(server_msg, "Sleep in progress..") != 0 ){  // if the client entered "sleep" command, do not print anything to the client UI.
    printf("%s\n", server_msg);
    }

  }

  return 0; // terminate client normally.
}

