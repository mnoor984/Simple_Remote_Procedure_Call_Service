#include <stdio.h>
#include <string.h>
#include <stdint.h>
# include <unistd.h>

#include "a1_lib.h"
#include "rpc.h"


#define BUFSIZE   1024

char return_message[50];    // The message that will be sent to the client.
char unpacked_command[50];  // The command sent by the client.

/**
 * @brief: Addition on integers.
 * 
 * @param a: int.
 * @param b: int.
 * @return:  int.
 */
int addInts(int a, int b) {
  return a + b;
}

/**
 * @brief: Multiplication of two integers.
 * 
 * @param a: int.
 * @param b: int.
 * @return: int.
 */
int multipleInts(int a, int b) {
  return a * b;
}

/**
 * @brief: Puts the backend process to sleep for a specified time.
 * 
 * @param a: Time in int for which the backend process will sleep for. 
 */
void process_sleep(int a) {
  sleep(a); // put the current process to sleep for "a" seconds.
}

/**
 * @brief: Division of numbers of type float.
 * 
 * @param a: Numerator of type float.
 * @param b: Denominator of type float.
 * @return: float. 
 */
float divideFloats(float a, float b) {
  return a / b;
}

/**
 * @brief: Calculates factorial of an int.
 *         Recursive method.
 * @param n: int.
 * @return: uint64_t.
 */
uint64_t factorial(int n) {
  if ((n == 1) || (n == 0)) return 1;
  return n * factorial(n-1);
}

/**
 * @brief: Unpacks the message sent by the client, then determines what command the
 * the client sent, if the command is appropriate, the function corresponding to the 
 * command is executed. An appropriate message is then put into the "return_message" array.
 * 
 * If there is something wrong (for example, the command sent by the client does not exist),
 * an appropriate message is put into the "return_message" array.
 * 
 * @param msg: Pointer to the structure sent by the client.
 */
void unpack_message(message_t *msg) {

  memset(return_message, 0, sizeof(return_message));  // All slots in the array are set to 0.

	 char *command = msg->cmd;    // command now points to the command sent by the client.
	printf("Command is: %s\n", command);  // print the command send by the user to see what it is.


  /**
   * @brief: if the command sent by the user does not equal to any command available in the backend, then 
   *         store an appropriate error message in the return message array.
   * 
   */
  if ((strcmp(command, "add") != 0) || (strcmp(command, "multiply") != 0) ||
     (strcmp(command, "divide") != 0) || (strcmp(command, "factorial") != 0) || 
     (strcmp(command, "sleep") != 0) || (strcmp(command, "exit") != 0)) {

      sprintf(return_message, "Error: Command \"%s\" not found.", command);
    } 

  if (strcmp(command, "exit") == 0) {
      strcpy(unpacked_command, "exit");
      strcpy(return_message, "Bye!");
  }


  if (strcmp(command, "shutdown") == 0) {
      strcpy(unpacked_command, "shutdown");
      strcpy(return_message, "Shutdown in progres..");
  }

  if (strcmp(command, "add") == 0) {
		int num1 = atoi(msg->args[0]);
		int num2 = atoi(msg->args[1]);
		int ans = addInts(num1, num2);
    sprintf(return_message, "%d", ans);
	}

	if (strcmp(command, "multiply") == 0) {
		int num1 = atoi(msg->args[0]);
		int num2 = atoi(msg->args[1]);
		int ans = multipleInts(num1, num2);
    sprintf(return_message, "%d", ans);
	}

  if (strcmp(command, "divide") == 0) {
	  float num1 = atof(msg->args[0]);
	  float num2 = atof(msg->args[1]);
    if (num2 == 0.0) {  // if the denominator is == 0, put an appropriate error into the return message array.
      strcpy(return_message, "Error: Division by zero.");
      return;
    }
	  float ans = divideFloats(num1, num2);
    sprintf(return_message, "%.2f", ans);
	}

  if (strcmp(command, "factorial") == 0) {
	  int num1 = atoi(msg->args[0]);
    if ((num1 > 20) || (num1 < 0)) {  // if the number is out of range, put an appropriate error into the return message array.
      strcpy(return_message, "x is guaranteed to be in the range [0,20]");
      return;
    }
	  uint64_t ans = factorial(num1);
    sprintf(return_message, "%lu", ans);
	}
  if (strcmp(command, "sleep") == 0) {
    int num1 = atoi(msg->args[0]);
    process_sleep(num1);
    strcpy(return_message,"Sleep in progress..");
  } 

}

/**
 * @brief: Receives the message sent by the client. Converts the pointer to the structure sent by the client to a pointer of type "message_t", 
 *         "unpack_message" is then called. Once the "unpack_message" returns, an appropriate message is sent to the 
 *         client. Appropriate memory is then freed.
 * 
 * @param clientfd: The file descriptor of the client connection.
 */
void serve_client(int clientfd){
    
    while ((strcmp(unpacked_command, "exit") != 0) && (strcmp(unpacked_command, "shutdown") != 0)) { // While the command sent by the client is not exit or shutdown.
    
    message_t *message_pointer; // pointer initialization of type message_t.
		message_pointer = (message_t *) malloc(sizeof(message_t));  // allocate memory for the structure that will be sent by the client.
    
    ssize_t byte_count = recv_message(clientfd, (char *) message_pointer, sizeof(message_t)); // receive message from client.
    if (byte_count <= 0) {
      break;
    }
    (message_t *) message_pointer;    //  change pointer from type char to type message_t.
    
    unpack_message(message_pointer); // unpack the message sent by the client and do appropriate things.
    
    send_message(clientfd, return_message, strlen(return_message));  //send appropriate message back to the client.
    
    free(message_pointer);  // free memory

  } //end of while loop

}

/**
 * @brief: Create server. Accept connections. Calls the "serve_client" method to serve clients. Responsible for forking,
 *         child process termination and parent process termination.
 * 
 * @return int.
 */
int main(int argc, char *argv[]) {
  int sockfd, clientfd;
  int pid, status;
  int port = atoi(argv[2]);


  printf("Server is listening on %s:%d\n", argv[1], port);      

  if (create_server(argv[1], port, &sockfd) < 0) { 
    fprintf(stderr, "oh no\n");
    return -1;
  }

  while (1) { // continuous while loop
   
  if (accept_connection(sockfd, &clientfd) < 0) {
    fprintf(stderr, "oh no\n");
    return -1;
  }

    waitpid(-1, &status, WNOHANG);  // check status of child processes

   
   if (WEXITSTATUS(status) == 1) { // if a child process did exit(1), in other words, the child process requested a shutdown.
     
     pid = fork();  // fork initialization.

     if (pid == 0) {  // if child process.
     serve_client(clientfd); // serve the client that initiated a connection after a shutdown was requested by another client. This is the last client that will
     exit(0);                // served before the backend (parent process) will be terminated.
     }
     else {
       waitpid(pid, &status, 0);  // wait for the last client to terminate.
     }
     while (waitpid(-1, &status, 0) > 0); // wait for all processes to terminate.

     printf("Terminating Backend\n");
     break;                       // break and do return 0;
   }

  pid = fork(); // fork initialization.

  if (pid == 0) { // if child process.
      serve_client(clientfd); // serve the child process.
      if (strcmp(unpacked_command, "shutdown") == 0) { // if the child process requested a shutdown.
          exit(1);  // terminate child process.
      } 
      exit(0);  //terminate child process normally.
  }

  } //end of while loop.
    
   return 0; // last line of code run by the backend parent process.

} //end of main method.

