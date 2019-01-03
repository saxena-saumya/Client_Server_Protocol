/* Author: Saumya Saxena
CS544: Computer Networks - Protocol Design Implementation
Date: June 10, 2018
Description: The code for the client
*/

/* Header Files */
#include <sys/types.h>
#include <sys/socket.h>  //includes a number of definitions of structures needed for sockets
#include <netinet/in.h>  //contains constants and structures needed for internet domain addresses
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


/* Function: Receive_msg(void *sock)
Description: When client receives a message from server, pthread_create gets executed and puts message on the console */
void *Receive_msg(void *sock)
{
	int sckt = *((int *)sock);
	char msg[500];
	int len;
	while((len = recv(sckt,msg,500,0)) > 0) {
		msg[len] = '\0';
		fputs(msg,stdout);
		memset(msg,'\0',sizeof(msg));
	}
}

int main(int argc, char *argv[])
{
	/* variables */	
	struct sockaddr_in their_addr; //sockaddr structure is used to define a socket address which is used in the bind(), connect()
	int nwsock; //store the client socket
	int sckt; //server socket from which message is received 
	int addrsize; //server socket address size
	int portno; //Port number of server on which client is connected
	pthread_t send,recv; //pthread sender and receiver thread	
	char msg[500]; //msg sent to the other clients
	char senderName[100]; //name of the sender concatinate with the client message
	char res[600]; //Full message with sendername + "message"  ex: Alan:Hello   ---> senderName: Alan
	char ip[INET_ADDRSTRLEN]; //Stores the ip of the server 
	int len;

	if(argc > 3) {
		printf("too many arguments");
		exit(1);
	}

	portno = atoi(argv[2]); //Get portno from the user
	
	strcpy(senderName,argv[1]); //Copy sender name entered by user into senderName

	nwsock = socket(AF_INET,SOCK_STREAM,0); //AF_INET refers to addresses from the internet, IP addresses specifically
	memset(their_addr.sin_zero,'\0',sizeof(their_addr.sin_zero));

	their_addr.sin_family = AF_INET; //Sock_addr socket creation for server
	their_addr.sin_port = htons(portno);
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(nwsock,(struct sockaddr *)&their_addr,sizeof(their_addr)) < 0) //connect to the server(their_addr) using nwsock
	{
		printf("connection not established");
		exit(1);
	}

	inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN); //CLIENT: Return the ip address of the server on which client is connected
	printf("Connecting to %s.\n",ip);
	
	pthread_create(&recv,NULL,Receive_msg,&nwsock); //Pthread_create creates a function call 

	while(fgets(msg,500,stdin) > 0) //Creating message to send to the other clients through server
	{
		strcpy(res,senderName);
		strcat(res,":");
		strcat(res,msg);
		
		len = write(nwsock,res,strlen(res));

		if(len < 0) {
			printf("message not sent");
			exit(1);
		}
		
		memset(msg,'\0',sizeof(msg));
		memset(res,'\0',sizeof(res));
	}
	pthread_join(recv,NULL);
	close(nwsock);
}
