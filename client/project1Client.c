//Junaid (Jay) Khan 11346814 CptS455 WSU

#include "../headers/project1.h" //arg definitions, general libraries.
#include "../headers/project1Client.h" //client definitions

//******NOTE: Acknowledging use of the TCP-IP sockets book (Calvert)

int main(int argc, char **argv){
	int sock; //socket descriptor
	struct sockaddr_in serverAddress;
	unsigned short serverPort;
	char *serverIP;
	char buffer[RCVBUFSIZE]; //buffer to store server string above
	int bytesReceived; //bytes read in a single recv() call
   	int	totalBytesReceived; //total bytes we read

	int i;

	//printf does not work without this for me.
	setbuf(stdout, NULL);

	if (argc != 3){	
		fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n",
			   	argv[0]);
		exit(1);
	}

	serverIP = argv[1]; // The server's IP address.
	serverPort = atoi(argv[2]);


	// Create reliable, stream socket using TCP			
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		DieWithError("socket() failed");
	} 

	// Construct server address structure	
	memset(&serverAddress, 0, sizeof(serverAddress)); // zero out the struct
	serverAddress.sin_family = AF_INET; // Internet address family
	serverAddress.sin_addr.s_addr = inet_addr(serverIP); //server's ip
	serverAddress.sin_port = htons(serverPort); //port (to big endian)

	// Establish that connection
	if(connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress))<0){
		DieWithError("connect() failed");
	}

	for(i = 0; i < COMMANDS; i++){


		switch(i){
			case 6: //0
				NoMoreCommands(sock, i);
				break;					

			case 0:
				NullTerminatedCmd(sock, i);
				break;

			case 1:
				GivenLengthCmd(sock, i);
				break;

			case 2:
				BadIntCmd(sock, i);
				break;

			case 3:
				GoodIntCmd(sock, i);
				break;

			case 4:
				ByteAtATimeCmd(sock, i);
				break;

			case 5:
				KbyteAtATimeCmd(sock, i);
				break;

		}

	}
}

void DieWithError(char *errorMessage){
		perror(errorMessage);
		exit(1);
}
