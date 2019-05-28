#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include "header.h"

int main(int argc, char* argv[]){

	char buf[1024], command[14], ip[14], clientPort[10];
	int port, sock, newsocket, clientSock, receiveLen, sendLen, c;
	socklen_t clientlen;
	struct sockaddr_in server, client, serverClient;
	struct sockaddr *serverptr = (struct sockaddr *) &server;
	struct sockaddr *serverClientptr = (struct sockaddr *) &serverClient;
	struct sockaddr *clientptr = (struct sockaddr *) &client;

	struct hostent *rem, *clientRem;
	listptr clientList=NULL;
	
	if(argc<3){
		printf("PLease give port number!\n");
		exit(1);
	}

	port=atoi(argv[2]);

	//Create socket 
	if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
   		perror_exit("socket");

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
		sscanf(buf,"%s %s %s", command, ip, clientPort);
		printf("Received command: <%s>\n",command);

		if( strcmp(command, "LOG_ON")==0) 
			c=1;
		if(strcmp(command, "GET_CLIENTS")==0) 
			c=2;
		if(strcmp(command, "LOG_OFF")==0)
			c=3;

		switch(c){
			case 1:
				printf("Received client ip <%s>\n", ip);
				printf("Received client port <%s>\n", clientPort);
				//LOG_ON
	
				//Insert client in list if he does not already exist
				if(!isInList(clientList,ip,clientPort)){
					insertList(&clientList,ip,clientPort);
					printf("Inserted a new client in my list!\n");
				}
				//Send the USER_ON <IP, port> to all the clients 
				listptr templist=clientList;
				while(templist!=NULL){
					//Create socket
					if((clientSock=socket(AF_INET,SOCK_STREAM,0))<0)
 					   perror_exit("socket");

					//Find server address 
					if((clientRem=gethostbyname(clientList->clientIP))==NULL){
						herror("gethostbyname");
    					exit(1);
					}
	
					serverClient.sin_family=AF_INET;
					memcpy(&serverClient.sin_addr,clientRem->h_addr, clientRem->h_length);
					serverClient.sin_port=htons(atoi(clientList->clientPort));

					//Initiate Connection
					if((connect(clientSock,serverClientptr,sizeof(serverClient)))<0)
				 	   perror_exit("connect");
					//printf("Connecting to %s serverPort %d\n",argv[4],serverPort);
					snprintf(buf, 10 + strlen(ip) + strlen(clientPort) , "USER_ON %s %s" ,ip, clientPort );
   				 	//Send the character
   		 			if(send(clientSock,buf,strlen(buf),0)==-1)
						perror_exit("write");
					close(clientSock);
					templist=templist->next;
				}
				
			case 2: 
				//GET_CLIENTS
				//CLIENT_LIST N <IP1, PORT1> <IP2 PORT2> ... <IPN, PORTN>
				
				break;
	
			case 3: 
				//LOG_OFF
	
				//Find the  client that is logging off
				// Remove the client from our list
				if(deleteClient(&clientList, ip, clientPort)){
				//Send the USER_OFF <IPadress, portNum> to all the clients
					listptr templist=clientList;
					while(templist!=NULL){
						//Create socket
						if((clientSock=socket(AF_INET,SOCK_STREAM,0))<0)
 						   perror_exit("socket");
	
						//Find server address 
						if((clientRem=gethostbyname(clientList->clientIP))==NULL){
							herror("gethostbyname");
    						exit(1);
						}
	
						serverClient.sin_family=AF_INET;
						memcpy(&serverClient.sin_addr,clientRem->h_addr, clientRem->h_length);
						serverClient.sin_port=htons(atoi(clientList->clientPort));
	
						//Initiate Connection
						if((connect(clientSock,serverClientptr,sizeof(serverClient)))<0)
					 	   perror_exit("connect");
						//printf("Connecting to %s serverPort %d\n",argv[4],serverPort);
						snprintf(buf, 10 + strlen(ip) + strlen(clientPort) , "USER_OFF %s %s" ,ip, clientPort );
	   				 	//Send the character
	   		 			if(send(clientSock,buf,strlen(buf),0)==-1)
							perror_exit("write");
						close(clientSock);
						templist=templist->next;
					}
				
				}
				else{	//Client not found
					printf("ERROR_IP_PORT_NOT_FOUND_IN_LIST\n");
					if(send(newsocket,"ERROR_IP_PORT_NOT_FOUND_IN_LIST,strlen("ERROR_IP_PORT_NOT_FOUND_IN_LIST),0)==-1)
						perror_exit("write");		
				}
				break;
			default:
				printf("Client did not send correct information. Doing nothing.\n");
				break;
		}
		print(clientList);
		close(newsocket);
		break;
   }
	destroyList(&clientList);
	close(sock);
	return 0;
}

