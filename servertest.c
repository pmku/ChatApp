#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

#define PORT 17137 //Port that server is going to use

/*Clientside will have a selector menu that lists
available servers(localhost and one that I'll be hosting)
*/

int sockfd, addrlen; //Socket file descriptor and address length
struct sockaddr_in address; //*_in there represents inet IPV4

int acceptsock(){
	int newsock;
	if ((newsock = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
	return newsock;
}

void mockread(){
	int sock = acceptsock();
	char buff[1024] = {0};
	while(1){
		read(sock, buff, 1024);
		fprintf(stdout,"%s\n", buff);
	}
}
int main(int argc, char *argv[]){
	addrlen = sizeof(address); //Put value in addrlen, it was just global with 0 in it
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	/*
	   AF_INET describes connection is over internet
	   I would use AF_LOCAL to use loopback.
	   SOCK_STREAM defines connection type as TCP
	   SOCK_DGRAM is for UDP which I won't use
	   Protocol 0 is for IP
	*/

//Failure check for socket
	if (sockfd < 0){
		fprintf(stderr,"\nSocket creation has failed\n");
		exit(EXIT_FAILURE);
	}

//Setsocketopt in case the address in use
	int opt = 1;
	 if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		fprintf(stderr,"\nSocketopt could not be set\n");
		exit(EXIT_FAILURE);
	    }
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

//Final, binding section
	if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		fprintf(stderr,"\nSocket binding has failed\n");
		exit(EXIT_FAILURE);
	    }

//Put the server socket in listening mode. 5 is the max length of the queue
	if (listen(sockfd, 5) < 0) {
		fprintf(stderr,"\nSocket can't be listened\n");
		exit(EXIT_FAILURE);
	}
/*Server is set up at this point. To process request from clients;
	if ((new_socket = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
Is going to be present in each thread before read (valread = read(new_socket, buffer, 1024);)
*/
	mockread();
	return 0;
	}

