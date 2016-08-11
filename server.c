 #include <stdio.h>  
 #include <sys/types.h>  
 #include <sys/socket.h>  
 #include <netinet/in.h>  
 #include <stdlib.h>  
 #include <string.h>  

#define messageSize 512

int main(int argc,char *argv[])  
{  

	char array[messageSize];

	 int udpSocket,portno, t = 0, i, check, nBytes;  
	 char buffer[messageSize];  
	 struct sockaddr_in serverAddr,clientAddr;  
	 int n; 
	 int registered_clients[100];

	strcpy(array," ");
	//constructing socket
	 udpSocket=socket(AF_INET,SOCK_DGRAM,0);  
	 if(udpSocket<0)  
		  perror("error opening socket");  
	else printf("Socket creation..........\n");

	//constructing server address
	bzero((char *)&serverAddr,sizeof(serverAddr)); 
	serverAddr.sin_family=AF_INET;  
	serverAddr.sin_addr.s_addr=INADDR_ANY;
	serverAddr.sin_port=htons(9000); 
 
	//bind socket
 	if(bind(udpSocket,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) <0)  
	    error("error on binding");  
	else printf("Binding the server socket........\nThe server is waiting for connections\n");
		
	while(1)
	{  //infinite loop to continually wait for connections 
		check = 0;
		socklen_t clilen=sizeof(clientAddr);   

		bzero(buffer,sizeof(buffer));  
		n=recvfrom(udpSocket,buffer, messageSize, 0, (struct sockaddr*)&clientAddr, &clilen);  

		if(n<0)  
		{  
		 	error("error reading from socket");
		 	exit(1);  
		}  
		else  
		{  	
			printf("\n");
			printf("\n");
			printf("+------------------------------------------------------------------+\n");
			printf("A new incoming connecting request from client listening at port:%s\n", buffer); 
			printf("Client registration check in progress......\n");
			sscanf(buffer, "%d", &portno);

			/*check is client had registered with the server*/
			for( i = 0; i < t; i++){
				/*if registered print the registered message*/
				if(registered_clients[i] == portno)
				{
					check = -1;
					printf("Client had been registered\n");
					break;
				}
			}

			/*if not registered, register the client and store port no*/
			if(check != -1)
			{
				registered_clients[t] = portno;
				strcat(array,buffer);
				strcat(array, " ");
				t++;
				printf("client registered successfully!\n");
			}
		} 
		
		bzero(buffer,sizeof(buffer));
		strcpy(buffer, "Clients registered are listening at port:");
		strcat(buffer, array);
		printf("Client registered are:%s\n", array);
		printf("+------------------------------------------------------------------+\n");
		printf("\n");

		nBytes=strlen(buffer)+1;
		n=sendto(udpSocket, buffer, nBytes, 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
		if (n<0)
		{
			perror("could not send array");
		}

	}   
	close(udpSocket);   
  
}
