//Junaid (Jay) Khan 11346814 CptS455 WSU

#ifndef PROJECT1_H
#define PROJECT1_H

#include <stdio.h> // printf and fprintf
#include <sys/socket.h> // socket(), connect(), send(), rec()
#include <arpa/inet.h> // sockaddr_in, inet_addr()
#include <stdlib.h> // atoi()
#include <string.h> // memset()
#include <unistd.h> // close()

#define RCVBUFSIZE 1024
#define SENDBUFSIZE 1024

typedef struct {
unsigned cmd;
char *arg;
} command;
#define nullTerminatedCmd (1)
#define givenLengthCmd (2)
#define badIntCmd (3)
#define goodIntCmd (4)
#define byteAtATimeCmd (5)
#define kByteAtATimeCmd (6)
#define noMoreCommands (0)

/* This is the list of commands to be run by the client to demonstrate your program */
static command commands[] = {
{nullTerminatedCmd, "Sent as a null-terminated string"},
{givenLengthCmd, "Sent as an unterminated string"},
{badIntCmd, "20160919"},
{goodIntCmd, "20160919"},
{byteAtATimeCmd, "500000"},
{kByteAtATimeCmd, "500000"},
{noMoreCommands, ""}
};
/*
* These command names are to be prefixed to responses by the server; the array is
* indexed by the #define’d constants above
*/
static char *commandNames[] = {
"No More Commands",
"Null Terminated",
"Given Length",
"Bad Int",
"Good Int",
"Byte At A Time",
"KByte At A Time"
};

void DieWithError(char *errorMessage);

FILE *fp;

#endif
