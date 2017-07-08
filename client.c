#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//this program uses a socket implementation from www.LinuxHowtos.org
void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	//loop until user quits 
	while(1){
		//take input from user
		printf("Silent or Betray? (capital B or S): ");
		bzero(buffer,256);
		fgets(buffer,255,stdin);
		//check if input is valid 
		if(buffer[0]!='S'&&buffer[0]!='B'&&buffer[0]!='s'&&buffer[0]!='b'){
			printf("Error in input...\n"); continue;
		}
		//write to server
		n = write(sockfd,buffer,strlen(buffer));
		if (n < 0) 
			error("ERROR writing to socket");
		bzero(buffer,256);
		n = read(sockfd,buffer,255);
		if (n < 0) 
			error("ERROR reading from socket");
		printf("%s\n",buffer);

		//take input for play again
		printf("Play Again?[Y/N]: ");
		bzero(buffer, 256);
		fgets(buffer,255,stdin);
		//continue loop
		if(buffer[0]=='y'||buffer[0]=='Y') continue;
		//else write break to server and the break client
		else{
			bzero(buffer,256);
			write(sockfd,"break",5);
			break;
		}
	}
	close(sockfd);
	return 0;
}
