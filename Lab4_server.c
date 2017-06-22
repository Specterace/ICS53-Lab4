/*
  Oscar Choy
  ID # 53555169
  3/14/2017
  Lab 4
  
  This is the server-side portion of the Lab 4 socket project. Running on the server-side terminal/machine, 
  the server will open a socket to listen for messages on the port whose number was given by the user in the
  command line. It will then display a message on the output console telling the server-side user that the
  server was initialized and is now listening for messages on that port.
  
  When the server receives a message from the port, it will do the following: decode the message, and respond
  according to the message given. First, the decoding step will take the message, obtain the size of the
  email address sent by the client, and use that size to copy in the address itself. The server will then take
  that address and compare it to the entries in it's internal database (the array of addresses in the code). If
  it finds a match, the server will then look for the owner in the database, and return a message to the client
  stating who the owner is. If there is no match, the server will return a message to the client stating "unknown".
  If at any point the server reads in a message of "+++", it will send a final message to the client stating that
  it will close the connection. The server will then go through with the close of the connection. Note that the 
  server will still be able to listen into the channel to reconnect a socket connection if a client connects or
  reconnects with it.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <unistd.h>


struct webDBase {
	unsigned char webAddress[256];
	unsigned char webName[256];
};

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen, cliActive = 1, areEqual;
     int addrLen, i, foundAddress = 0;
     unsigned char buffer[256];
     unsigned char exitStr[5] = "+++";
     unsigned char webAddr[256];
     unsigned char sentMesg[256];
	 unsigned char addrName[256];
	 struct webDBase dataBase[3]; 
     struct sockaddr_in serv_addr, cli_addr;
     int n;
	 
	 strcpy(dataBase[0].webAddress, "harris@ics.uci.edu");
	 strcpy(dataBase[0].webName, "Ian G. Harris");
	 strcpy(dataBase[1].webAddress, "joe@cnn.com");
	 strcpy(dataBase[1].webName, "Joe Smith");
	 strcpy(dataBase[2].webAddress, "jane@slashdot.org");
	 strcpy(dataBase[2].webName, "Jane Smith");
	 
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");  
     }
	printf("Address server started\n");
    while(1) {
		cliActive = 1;
		listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		bzero(buffer,256);
		bzero(webAddr,256);
		bzero(sentMesg,256);
		if (newsockfd < 0) {
			perror("ERROR on accept");
        }
		while (cliActive == 1) {
			bzero(buffer,256);
			bzero(webAddr,256);
			bzero(sentMesg,256);
			n = read(newsockfd,buffer,255);
			if (n == 0) {
				cliActive = 0;
			} else if (n < 0) {
				perror("ERROR reading from socket");
			} else {
				foundAddress = 0;
				addrLen = (int) buffer[0];
				i = 0;
				while (i < addrLen) {
					webAddr[i] = buffer[(i + 1)];
					i++;
				}
				i = 0;
				printf("\n%s\n",webAddr);
				while((i < 3) && (foundAddress == 0)) {
					if (strcmp(webAddr, dataBase[i].webAddress) == 0) {
						addrLen = strlen(dataBase[i].webName);
						strcpy(addrName, dataBase[i].webName);
						foundAddress = 1;
					} else {
						i++;
					}
				}
				if (foundAddress == 0) {
					strcpy(addrName, "unknown");
					addrLen = 7;
				}
				sentMesg[0] = addrLen;
				i = 0;
				while (i < addrLen) {
					sentMesg[(i + 1)] = addrName[i];
					i++;
				}
				n = write(newsockfd,sentMesg,strlen(sentMesg));
				if (n < 0) perror("ERROR writing to socket");
			}
			
		}
        close(newsockfd);
        sleep(1);
    }
    return 0; 
}