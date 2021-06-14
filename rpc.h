#include <stdio.h>

#ifndef RPC_H_
#define RPC_H_

#define CMD_LENGTH  256
#define ARGS_LENGTH 256

/**
 * @brief The struct that will be sent from the client to the server.
 *        It will store the command and arguments entered by the client.
 * 
 */
typedef struct message_t {
    char cmd[10]; // Stores command entered by client.
    char args[2][10]; // stored arguments entered by the client.
} message_t;

#endif



