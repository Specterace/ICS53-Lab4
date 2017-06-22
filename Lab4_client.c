/*
  Oscar Choy
  ID # 53555169
  3/14/2017
  Lab 4
  
  This is the client-side portion of the Lab 4 socket project. Running on the client-side terminal/machine,
  it will first confirm the opening of the connection on the port specified by the user on the command line,
  printing out an error message if the connection was not successful. It will then prompt the user to type 
  in the email address who's owner's name the user wants the server-side to look up and return. If the 
  user sends an address stored in the server's database, the user will see the server's reply (which will 
  be the name of the owner of the entered email address) on the output console. If the address is not in 
  the database, the user will see a message stating such an absence. If at any prompt, the user types in 
  and sends the string "+++" to the server, the server will close the connection, and the client will 
  get a message stating the closing of the connection, after which the client will exit.
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

int main(int argc, char *argv[])
{
    int sockfd, portno, n, i, c, isActive = 1, areEqual = 0, addrLen = 0;
    
    struct sockaddr_in serv_addr;
    struct hostent *server;

    unsigned char buffer[256];
	unsigned char exitStr[5] = "+++\n";
	unsigned char webName[256];
	unsigned char sentMesg[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
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
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");
	while (isActive == 1) {
		printf("> ");
		bzero(buffer,256);
		bzero(sentMesg,256);
		bzero(webName, 256);
		fgets(buffer,255,stdin);
		if (strcmp(buffer, "\n") != 0) {
			areEqual = strcmp(buffer, exitStr);
			if (areEqual == 0) {
				isActive = 0;           
			} else {
				addrLen = (strlen(buffer)) - 1;
				sentMesg[0] = addrLen;
				i = 0;
				while (i < addrLen) {
					sentMesg[(i + 1)] = buffer[i];
					i++;
				}
				n = write(sockfd, sentMesg, strlen(sentMesg));
				if (n < 0) 
					perror("ERROR writing to socket");
				bzero(buffer,256);
				n = read(sockfd,buffer,255);
				i = 0;
				addrLen = (int) buffer[0];
				while (i < addrLen) {
					webName[i] = buffer[(i + 1)];
					i++;
				}
				if (n < 0) 
					perror("ERROR reading from socket");
				printf("\n%s\n\n",webName);
			}
		}
	}
    return 0;
}
