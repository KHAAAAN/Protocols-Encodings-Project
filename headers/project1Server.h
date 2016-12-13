//Junaid (Jay) Khan 11346814 CptS455 WSU

#ifndef PROJECT1SERVER_H
#define PROJECT1SERVER_H

//**NOTE: int sock is the client socket.

void NullTerminatedCmd(int sock, char buffer[RCVBUFSIZE], int cmd);
void GivenLengthCmd(int sock, char buffer[RCVBUFSIZE], int cmd);
void SomeIntCmd(int sock, char buffer[RCVBUFSIZE], int cmd);
void ByteAtATimeCmd(int sock, char buffer[RCVBUFSIZE], int cmd, int *totalBytesReceived);
void KbyteAtATimeCmd(int sock, char buffer[RCVBUFSIZE], int cmd, int *totalBytesReceived);

#endif
