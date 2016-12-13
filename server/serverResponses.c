//Junaid (Jay) Khan 11346814 CptS455 WSU
#include "../headers/project1.h"

void NullTerminatedCmd(int sock, char buffer[RCVBUFSIZE], int cmd){
	unsigned short cmdArgLen, sendLen;
	unsigned short *Len16; //16bit string length to send
	char sendBuffer[RCVBUFSIZE];

	/*Note that the terminator for the string sent by nullTerminatedCmd
 	 * is *not* considered part of the string and should not be echoed.
	 */

	cmdArgLen = strlen(buffer); //this length disregards nullbyte
	fwrite(buffer, cmdArgLen, 1, fp);

	Len16 = (unsigned short *) &sendBuffer[0];

	//Length of commandName + colon (1) + space (1) + length of the received string
	*Len16 = htons(strlen(commandNames[cmd]) + 2 + cmdArgLen); //Make sure this is a network byte order number
	//the client will do a call to ntohs on this 16 bit string length

	sprintf(sendBuffer + 2, "%s: %s", commandNames[cmd], buffer);
	sendLen = 2 + strlen(sendBuffer + 2); //The first 2 is the 16 bit string length and sendBuffer + 2 is the string

	// null byte is not included for sendBuffer as only length of string is given
	//send message back to client
	if(send(sock, sendBuffer, sendLen, 0) != sendLen){
		DieWithError("send() failed");
	}
}

//note buffer is rcvbuffer
void GivenLengthCmd(int sock, char buffer[RCVBUFSIZE], int cmd){
	unsigned short cmdArgLen, sendLen;
	unsigned short *Len16, *recvLenPtr; //16bit string length to send
	char sendBuffer[RCVBUFSIZE];

	int i, j;

	recvLenPtr = (unsigned short *) &buffer[0];

	cmdArgLen = ntohs(*recvLenPtr); //sent from client as network byte order, we want host
	//because that tells us the length of the cmd.arg
	
	fwrite(buffer, cmdArgLen + 2, 1, fp);

	Len16 = (unsigned short *) &sendBuffer[0];

	//Length of commandName + colon (1) + space (1) + length of the received string
	*Len16 = htons(strlen(commandNames[cmd]) + 2 + cmdArgLen); //add cmdArgLen after, since already Network byte order

	//printf("*Len16 = %hu", *Len16);

	sprintf(sendBuffer + 2, "%s: ", commandNames[cmd]);

	//we still need to put in the buffer (message we initially received)
	i = strlen(sendBuffer + 2) + 2; //i will start index at '\0' +2 accounts for Len16
	j = 2; //skip the first 2 bytes

	sendLen = i;

	// + 2 accounts for j = 2
	while(j < cmdArgLen + 2){
		sendBuffer[i++] = buffer[j++];	
	}

	sendLen += j; //j is non zero length so include last iteration
	
	//send message back to client
	if(send(sock, sendBuffer, sendLen, 0) != sendLen){
		DieWithError("send() failed");
	}
}

void SomeIntCmd(int sock, char buffer[RCVBUFSIZE], int cmd){
	unsigned short cmdArgLen, sendLen;
	unsigned short *Len16; //16bit string length to send
	char sendBuffer[RCVBUFSIZE];

	int i;
	int *someInt;
	int sendInt;

	char intAsString[128];

	Len16 = (unsigned short *) &sendBuffer[0];	
		
	someInt = (int *) &buffer[0];
	sendInt = ntohl(*someInt);
	sprintf(intAsString, "%d", sendInt);

	//we must get the displayed string length
	cmdArgLen = strlen(intAsString); 

	fwrite(buffer, 4, 1, fp);

	//Length of commandName + colon (1) + space (1) + length of the received string
	*Len16 = htons(strlen(commandNames[cmd]) + 2 + cmdArgLen);

	sprintf(sendBuffer + 2, "%s: %s", commandNames[cmd], intAsString);

	//strlen doesn't include the nullbyte above
	sendLen = 2 + strlen(sendBuffer + 2); // first 2 accounts for the 16 bit string length

	//send message back to client
	if(send(sock, sendBuffer, sendLen, 0) != sendLen){
		DieWithError("send() failed");
	}

}

void ByteAtATimeCmd(int sock, char buffer[RCVBUFSIZE], int cmd, int *totalBytesReceived){
	unsigned short cmdArgLen, sendLen;
	unsigned short *Len16; //16bit string length to send
	char sendBuffer[RCVBUFSIZE];
	char altBuffer[1];

	int i, recvMsgSize;
	int *someInt;
	int numBytes; //500 000

	int numRcv; //number of recv calls made

	char intAsString[128];

	numRcv = 1; //1 has been made before this is called

	Len16 = (unsigned short *) &sendBuffer[0];	
	someInt = (int *) &buffer[0];
	numBytes = ntohl(*someInt);

	fwrite(buffer, 4, 1, fp);

	i = 5; //we already 5 bytes

	while (i < numBytes){
		if ((recvMsgSize = recv(sock, altBuffer, 1, 0)) < 0){
			DieWithError("recv() failed");
		} else if (recvMsgSize > 0){
			*totalBytesReceived += recvMsgSize; //where recvMsgSize is always 1
			numRcv += 1;

			fwrite(altBuffer, 1, 1, fp);
		}
		i++;
	}		

	//numRcv -= 5; //get rid of the first 5 bytes we sent, the 1 accounts for it
	sprintf(intAsString, "%d", numRcv);

	//Length of commandName + colon (1) + space (1) + length of number of recv calls
	*Len16 = htons(strlen(commandNames[cmd]) + 2 + strlen(intAsString));

	sprintf(sendBuffer + 2, "%s: %s", commandNames[cmd], intAsString);

	sendLen = 2 + strlen(sendBuffer + 2); // first 2 accounts for the 16 bit string length

	//send message back to client
	if(send(sock, sendBuffer, sendLen, 0) != sendLen){
		DieWithError("send() failed");
	}
}

void KbyteAtATimeCmd(int sock, char buffer[RCVBUFSIZE], int cmd, int *totalBytesReceived){
	unsigned short cmdArgLen, sendLen;
	unsigned short *Len16; //16bit string length to send
	char sendBuffer[RCVBUFSIZE];
	char altBuffer[1000];

	int i, recvMsgSize;
	int *someInt;
	int numBytes; //500 000

	int numRcv; //number of recv calls made

	char intAsString[128];

	fwrite(buffer, 4, 1, fp);

	numRcv = 1; //1 has been made before this is called

	Len16 = (unsigned short *) &sendBuffer[0];	
	someInt = (int *) &buffer[0];
	numBytes = ntohl(*someInt);

	//except for the last. (aka we read 5 bytes but that filled in a 1000 byte block)
	i = 1000; 

	while (i < numBytes){
		if ((recvMsgSize = recv(sock, altBuffer, 1000, 0)) < 0){
			DieWithError("recv() failed");
		} else if (recvMsgSize > 0){
			*totalBytesReceived += recvMsgSize; //where recvMsgSize is always 1
			numRcv += 1;

			fwrite(altBuffer, 1000, 1, fp);
		}
		i+= 1000;
	}

	sprintf(intAsString, "%d", numRcv);

	//Length of commandName + colon (1) + space (1) + length of number of recv calls
	*Len16 = htons(strlen(commandNames[cmd]) + 2 + strlen(intAsString));

	sprintf(sendBuffer + 2, "%s: %s", commandNames[cmd], intAsString);

	sendLen = 2 + strlen(sendBuffer + 2); // first 2 accounts for the 16 bit string length

	//send message back to client
	if(send(sock, sendBuffer, sendLen, 0) != sendLen){
		DieWithError("send() failed");
	}
}
