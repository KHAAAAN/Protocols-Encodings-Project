//Junaid (Jay) Khan 11346814 CptS455 WSU

#include "../headers/project1.h"
#include "../headers/project1Server.h"

#define MAXPENDING 5 // Maximum outstanding connection requests

void HandleTCPClient(int clientSock);

int main(int argc, char **argv){
	int serverSock; // socket descriptor for server
	int clientSock; // socket descriptor for client
	struct sockaddr_in serverAddress; // your local address
	struct sockaddr_in clientAddress; // client's address
	unsigned short serverPort; // server's port
	unsigned int clientLen; // Length of client address data structure

	//printf does not work without this for me.
	setbuf(stdout, NULL);

	if (argc != 2){
		fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
		exit(1);
	}

	serverPort = atoi(argv[1]); // first arg is the port we run our server on


	//create socket for incoming connections
	if ((serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		DieWithError("socket() failed");
	}

	//construct local address structure
	memset(&serverAddress, 0, sizeof(serverAddress)); //zero out structure
	serverAddress.sin_family = AF_INET; // internet address family
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
	serverAddress.sin_port = htons(serverPort); //Local port

	// bind to local address
	if (bind(serverSock, (struct sockaddr *) &serverAddress,
				sizeof(serverAddress)) < 0){
		DieWithError("bind() failed");
	}

	// Mark the socket so it will listen for incoming connections
	if (listen(serverSock, MAXPENDING) < 0){
		DieWithError("listen() failed");
	}

	while(1){
		// Set the size of the in-out parameter
		clientLen = sizeof(clientAddress);

		// wait for a client to connect
		if ((clientSock = accept(serverSock, (struct sockaddr *) &clientAddress,
						&clientLen)) < 0){
			DieWithError("accept() failed");
		}

		// client sock is now connected to a client

		//printf("Handling client %s\n", inet_ntoa(clientAddress.sin_addr));
	
		HandleTCPClient(clientSock);
	}
}

void DieWithError(char *errorMessage){
		perror(errorMessage);
		exit(1);
}

void HandleTCPClient(int clientSock){
	char buffer[RCVBUFSIZE + 1]; //buffer for string, +1 for the cmd.cmd	
	int recvMsgSize;
	int cmd;
	int totalBytesReceived;

	fp = fopen("server.log", "w+");	

	totalBytesReceived = 0;
	
	// Receive message from client
	if ((recvMsgSize = recv(clientSock, buffer, RCVBUFSIZE, 0)) < 0){
		DieWithError("recv() failed");
	}

	// Send received string and receive again until end of transmission
	while (recvMsgSize > 0) { // zero indicates end of transmission

		//we'll know we have at least one byte, which is client's command.cmd
		cmd = buffer[0] - '0';		
		fputc(buffer[0], fp);

		//these send() messages back to client socket
		switch(cmd){
			case 1:	
				NullTerminatedCmd(clientSock, buffer + 1, cmd);
				break;
			case 2:	
				GivenLengthCmd(clientSock, buffer + 1, cmd);
				break;

			case 3:
			case 4:
				SomeIntCmd(clientSock, buffer + 1, cmd);
				break;
				
			case 5:
				ByteAtATimeCmd(clientSock, buffer + 1, cmd, &totalBytesReceived);
				break;

			case 6:
				KbyteAtATimeCmd(clientSock, buffer + 1, cmd, &totalBytesReceived);
				break;

			default:
				printf("Unrecognized msg.\n");
				//close(clientSock);
				break;
		}

		//zero out the buffer for next potential read
		memset(buffer, 0, RCVBUFSIZE);

		//add to our total bytes recv, if below produces 0, we dont add a
		totalBytesReceived += recvMsgSize;

		// See if there is more data to receive
		if ((recvMsgSize = recv(clientSock, buffer, RCVBUFSIZE, 0)) < 0){
			DieWithError("recv() failed");
		}

	}

	printf("For this connection %d bytes were read\n", totalBytesReceived);

	fclose(fp);
	close(clientSock); // Close client socket.
}
