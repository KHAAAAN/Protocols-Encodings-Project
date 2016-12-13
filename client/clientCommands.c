//Junaid (Jay) Khan 11346814 CptS455 WSU

#include "../headers/project1.h"

//this parses the response we get from server and echoes it.
void PrintServerResponse(char recvBuffer[RCVBUFSIZE]){
	char *c;
	int i;
	int bytesToPrint;
	char sizeBuffer[5]; //max value for unsigned short (16 bit) is 65,535
	unsigned short *Len16; //16 bit string length
	unsigned short len;
	
	//get the 16bit length.
	Len16 = (unsigned short *) &recvBuffer[0];
	len = ntohs(*Len16); //this 16-bit number was send as network byte order, so change

	//jump assign past the 16 bit string length above
	c = recvBuffer + 2;

	i = 0;

	while(i < len){
		putchar(*c++);
		i++;
	}
	
	putchar('\n');
}

//case 0. 
void NoMoreCommands(int sock){
	close(sock);
}

/********* 
 * Case 1:
**********/
void NullTerminatedCmd(int sock, int cmdNum){
	unsigned int cmdArgLen, totalLen;
	command cmd = commands[cmdNum];
	int bytesReceived; //bytes read in a single recv() call
	char recvBuffer[RCVBUFSIZE]; //buffer to store server string above
	char sendBuffer[SENDBUFSIZE];

	cmdArgLen = strlen(cmd.arg); 

	// + 1 for null byte
	// another + 1 for cmd.cmd byte at the beginning
	totalLen = cmdArgLen + 1 + 1; 


	sendBuffer[0] = (char) (cmd.cmd + '0');
	strcpy(sendBuffer+1, cmd.arg);

	//the + 1 on the length is for the cmd.cmd byte at the beginning
	if(send(sock, sendBuffer, totalLen, 0) != totalLen){
		DieWithError("send() sent a different number of bytes than expected");	
	}

	if ((bytesReceived = recv(sock, recvBuffer, RCVBUFSIZE, 0)) < 0){
		DieWithError("recv() failed or connection closed prematurely");
	}

	PrintServerResponse(recvBuffer);
}

/********* 
 * Case 2:
**********/
void GivenLengthCmd(int sock, int cmdNum){
	unsigned int cmdArgLen, totalLen;
	command cmd = commands[cmdNum];
	int bytesReceived; //bytes read in a single recv() call
	char recvBuffer[RCVBUFSIZE]; //buffer to store server string above
	char sendBuffer[SENDBUFSIZE];

	int i;
	char *c;

	//sendBuffer[1] and [2] are now reserved for the string's length
	//as a 16 bit number
	unsigned short *len = (unsigned short *) &sendBuffer[1];
	cmdArgLen = strlen(cmd.arg); 

	// + 2 for 16 bit length
	// another + 1 for cmd.cmd byte at the beginning
	// cmdArgLen doesn't include null byte 
	totalLen = 1+ 2 + cmdArgLen; 


	sendBuffer[0] = (char) (cmd.cmd + '0');

	//turn our strings length to 16 bit number
	*len = htons((unsigned short) cmdArgLen);	

	c = cmd.arg;

	//now buffer [0], [1], [2] are taken
	i = 3;
	while(*c != '\0'){
		sendBuffer[i++] = *c++;
	}
	
	c = 0; //reset c.

	//the + 1 on the length is for the cmd.cmd byte at the beginning
	if(send(sock, sendBuffer, totalLen, 0) != totalLen){
		DieWithError("send() sent a different number of bytes than expected");	
	}

	if ((bytesReceived = recv(sock, recvBuffer, RCVBUFSIZE, 0)) < 0){
		DieWithError("recv() failed or connection closed prematurely");
	}

	PrintServerResponse(recvBuffer);
}

void BadIntCmd(int sock, int cmdNum){
	unsigned int totalLen;
	command cmd = commands[cmdNum];
	int bytesReceived; //bytes read in a single recv() call
	char recvBuffer[RCVBUFSIZE]; //buffer to store server string above
	char sendBuffer[SENDBUFSIZE];

	//sendBuffer[1], [2], [3], [4] are now reserved for the badInt
	//as a 16 bit number
	int *badInt = (int *) &sendBuffer[1];

	//here is where you would have applied the htonl	
	// i.e *badInt = htonl(atoi(cmd.arg))
	
	*badInt = atoi(cmd.arg);	
	sendBuffer[0] = (char) (cmd.cmd + '0');

	//cmd.cmd + 4 byte integer (not converted to big endian)
	totalLen = 1 + 4;

	if(send(sock, sendBuffer, totalLen, 0) != totalLen){
		DieWithError("send() sent a different number of bytes than expected");	
	}

	if ((bytesReceived = recv(sock, recvBuffer, RCVBUFSIZE, 0)) < 0){
		DieWithError("recv() failed or connection closed prematurely");
	}

	PrintServerResponse(recvBuffer);
}

void GoodIntCmd(int sock, int cmdNum){
	unsigned int totalLen;
	command cmd = commands[cmdNum];
	int bytesReceived; //bytes read in a single recv() call
	char recvBuffer[RCVBUFSIZE]; //buffer to store server string above
	char sendBuffer[SENDBUFSIZE];

	//sendBuffer[1], [2], [3], [4] are now reserved for the goodInt
	//as a 16 bit number
	int *goodInt = (int *) &sendBuffer[1];
	
	*goodInt = htonl(atoi(cmd.arg));	
	sendBuffer[0] = (char) (cmd.cmd + '0');

	//cmd.cmd + 4 byte integer (not converted to big endian)
	totalLen = 1 + 4;

	if(send(sock, sendBuffer, totalLen, 0) != totalLen){
		DieWithError("send() sent a different number of bytes than expected");	
	}

	if ((bytesReceived = recv(sock, recvBuffer, RCVBUFSIZE, 0)) < 0){
		DieWithError("recv() failed or connection closed prematurely");
	}

	PrintServerResponse(recvBuffer);
}


void ByteAtATimeCmd(int sock, int cmdNum){
	unsigned int totalLen, numBytes;
	command cmd = commands[cmdNum];
	int bytesReceived; //bytes read in a single recv() call
	char recvBuffer[RCVBUFSIZE]; //buffer to store server string above
	char sendBuffer[SENDBUFSIZE];
	int cmdArgNL, cmdArg;
	int *ptr;
	int i;
	char altBuffer[1];

	//first 5 bytes is regarded as 1 byte, rest are 0, 1 alternating bytes
	numBytes = 1 + 499995;	

	sendBuffer[0] = (char) (cmd.cmd + '0');

	cmdArg = atoi(cmd.arg); // - 5 because 5 we have already given 5 out 500000 bytes.
	cmdArgNL = htonl(cmdArg);

	ptr = (int *) &sendBuffer[1];
	*ptr = cmdArgNL;

	//should be in the form of 5(500000(NL)) now
	totalLen = 1 + 4; //cmd.cmd and a 4 byte integer
	
	//send the first one
	if(send(sock, sendBuffer, totalLen, 0) != totalLen){
		DieWithError("send() sent a different number of bytes than expected");	
	}

	i = 0;
	altBuffer[0] = 1;
	while(i < cmdArg){	
		altBuffer[0] = (altBuffer[0] == 0) ? 1 : 0;

		if(send(sock, altBuffer, 1, 0) != 1){

			DieWithError("send() sent a different number of bytes than expected");	
		}
		i++;
	}

	if ((bytesReceived = recv(sock, recvBuffer, RCVBUFSIZE, 0)) < 0){
		DieWithError("recv() failed or connection closed prematurely");
	}

	PrintServerResponse(recvBuffer);
}

void KbyteAtATimeCmd(int sock, int cmdNum){

	unsigned int totalLen, numBytes;
	command cmd = commands[cmdNum];
	int bytesReceived; //bytes read in a single recv() call
	char recvBuffer[RCVBUFSIZE]; //buffer to store server string above
	char sendBuffer[SENDBUFSIZE];
	int cmdArgNL, cmdArg;
	int *ptr;
	int i;
	char altBuffer[1000];

	//first 5 bytes is regarded as 1 byte, rest are 0, 1 alternating bytes
	numBytes = 1 + 499995;	

	sendBuffer[0] = (char) (cmd.cmd + '0');

	cmdArg = atoi(cmd.arg); // - 1 because 5 bytes is with a 1000 bytes block
	cmdArgNL = htonl(cmdArg);

	ptr = (int *) &sendBuffer[1];
	*ptr = cmdArgNL;

	//should be in the form of 5(500000(NL)) now
	totalLen = 1 + 4; //cmd.cmd and a 4 byte integer
	
	//send the first one
	if(send(sock, sendBuffer, totalLen, 0) != totalLen){
		DieWithError("send() sent a different number of bytes than expected");	
	}

	i = 0;
	altBuffer[0] = 1;
	while(i < cmdArg){	
		if(altBuffer[0] == 0){
			memset(altBuffer, 1, 1000);
		} else{
			memset(altBuffer, 0, 1000);
		}

		if(send(sock, altBuffer, 1000, 0) != 1000){

			DieWithError("send() sent a different number of bytes than expected");	
		}
		i+= 1000;
	}

	if ((bytesReceived = recv(sock, recvBuffer, RCVBUFSIZE, 0)) < 0){
		DieWithError("recv() failed or connection closed prematurely");
	}

	PrintServerResponse(recvBuffer);

}
