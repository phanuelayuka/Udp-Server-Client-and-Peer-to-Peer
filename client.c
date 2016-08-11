#include <stdio.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <netdb.h>  
#include <string.h>  
#include <pthread.h>
#include <stdlib.h>  

int socketHandler(int portno);
void connectionHandler(int portno, char *msg);
void* listeningHandler(void* sockfd);
int closecon = 1;

int socketHandler(int portno)
{
	int sockfd;
	struct sockaddr_in addr; 
	 
	sockfd=socket(AF_INET,SOCK_DGRAM,0); 
	if(sockfd<0)  
	 {  
		perror("error opening socket");  
	 }

	if (portno == -1)
 		return sockfd;
	//AF_INET is the address domain of the socket
	addr.sin_family=AF_INET;  
 	addr.sin_addr.s_addr=INADDR_ANY;  //any ip address can be used
 	addr.sin_port=htons(portno);
	

	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		perror("ERROR on binding");
  
	return sockfd;
} 

void connectionHandler(int portno, char *msg)
{
	int sockfd, n;
	struct sockaddr_in serv_addr, client_addr;  
	char buffer[256];  
	bzero((char*)&serv_addr,sizeof(serv_addr));  

	serv_addr.sin_family=AF_INET;  
	serv_addr.sin_port=htons(portno); 
	serv_addr.sin_addr.s_addr= inet_addr("127.0.0.1");   

 
	bzero(buffer, sizeof(buffer));  
	strcpy(buffer, msg);

	sockfd = socketHandler(-1);

	n = sendto(sockfd,buffer,sizeof(buffer), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));  
	if(n<0)  
	{  
		error("error writing to socket\n");  
		return;
	} 
	
	bzero(buffer,sizeof(buffer));

	socklen_t clientlen=sizeof(client_addr);
	n=recvfrom(sockfd,buffer, 256, 0, (struct sockaddr*)&client_addr, &clientlen);   
	if(n<0)
		perror("error reading rom socket\n");
	else 
		printf("Reply: %s\n", buffer);     
	
}

void* listeningHandler(void* sockfd)
{
	int newsockfd, n;
	int listsoc = *(int*)sockfd;
	char buffer[256], reply[256];
	struct sockaddr_in clientAddr;



	while (1){	
		socklen_t clientlen=sizeof(clientAddr);
		n = recvfrom(listsoc,buffer,256, 0, (struct sockaddr*)&clientAddr, &clientlen);
		if (n < 0) 
			perror("ERROR reading from socket\n");

		printf("message received from peeer: %s\n",buffer);
		bzero(buffer,256); 

		printf("Reply the message:");
		scanf("%s", reply);

		strcpy(buffer, reply);
		n = sendto(listsoc,buffer,256, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
		if (n < 0) 
			perror("ERROR writing to socket\n");
	}
	printf("stopped listening\n");
		
}


 int main(int argc,char *argv[])  
 { 
	int portno;
	int socid;
	char msg[256], *str_portno;
	int menu = 0;
	int pno= 9000;
	
	
	pthread_t storelist;
	
	printf("\n");
	printf("Which port do you want your client to listen from:");
	scanf("%d",&portno);
	printf("\n");
	
	socid = socketHandler(portno);
	
	pthread_create(&storelist,NULL,listeningHandler,(void*)&socid);
	
	sprintf(str_portno, "%d", portno);
	
	connectionHandler(pno, str_portno);
	

	while(closecon)
	{
		printf("\n");
		printf("   +------------------------------+\n");
		printf("   |          CHOICES             |\n");
		printf("   |------------------------------|\n");
		printf("   |   1:Request client lists     |\n");
		printf("   |   2:communicate with peer    |\n");
		printf("   |   3:close                    |\n");
		printf("   +------------------------------+\n");
		scanf("%d",&menu);


		switch(menu){
			case 1:
				sscanf(str_portno, "%d", &portno);
				connectionHandler(pno, str_portno);
				printf("\n\n");
				break;

			case 2:
				printf("Enter message to send: ");
				scanf("%s", msg);
				printf("Enter recipient port no:");
				scanf("%d",&portno);
				connectionHandler(portno,msg);
				printf("\n\n");
				menu=0;
				break;

			case 3:
				closecon = 0;
				break;
			break;

			default:
			menu=0;
			printf("no option selected");
			continue;
			
		}
	}
	pthread_join(storelist, (void **)NULL);
	
 }