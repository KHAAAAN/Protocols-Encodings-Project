//Junaid (Jay) Khan 11346814 CptS455 WSU

#ifndef PROJECT1CLIENT_H
#define PROJECT1CLIENT_H

#define COMMANDS (7)


void NoMoreCommands(int sock, int cmdNum);
void NullTerminatedCmd(int sock, int cmdNum);
void GivenLengthCmd(int sock, int cmdNum);
void BadIntCmd(int sock, int cmdNum);
void GoodIntCmd(int sock, int cmdNum);
void ByteAtATime(int sock, int cmdNum);
void KbyteAtATime(int sock, int cmdNum);

void PrintResponse(char sendBuffer[SENDBUFSIZE], char recvBuffer[RCVBUFSIZE]);

#endif
