#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "header.h"

int main(int argc, char* argv[]){

	char buf[1024];
	int port, sock, newsocket, receiveLen, sendLen;
	socklen_t clientlen;
	struct sockaddr_in server, client;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct sockaddr *clientptr = (struct sockaddr *) &client;

	struct hostent *rem;
	
	if(argc<3){
		printf("PLease give port number!\n");
		exit(1);
	}

	port=atoi(argv[2]);

	//Create socket 
	if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
   		perror_exit("socket");
//	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family=AF_INET; //internet domain
	server.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
	server.sin_port=htons(port); //given port

	//Bind socket to addr
	if((bind(sock,serverptr,sizeof(server)))<0)
    	perror_exit("bind");

	//Listen for connections
	if(listen(sock,5)<0)
  	  perror_exit("listen");
	printf("Listening for connections to port %d\n",port);

	while(1){
   		clientlen=sizeof(client);

 		//Accept Connection
    	if((newsocket=accept(sock, clientptr, &clientlen))<0)
			perror_exit("accept");
   
    	//Find client's name
    	if((rem=gethostbyaddr((char*)&client.sin_addr.s_addr,sizeof(client.sin_addr.s_addr),client.sin_family))==NULL){
			herror("gethostbyaddr");
			exit(1);
    	}
    	printf("Accepted connection from %s\n", rem->h_name);

		//recieve message
		receiveLen = recv(newsocket, buf, 1024 ,0);
    	if (receiveLen == -1){
        	printf("Error: receive has been  %d\n", newsocket);
        	close(newsocket);
        	exit(1);
    	}
		//sscanf(buf,"%s %s", message);

		printf("Received message: <%s>\n",buf);
		close(newsocket);
		break;
   }

	close(sock);
	return 0;
}

