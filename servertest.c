#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<signal.h>

#define PORT 17137 //Port that server is going to use

static int client_index = 0; //Will store id for the clients

//Double ll to hold client data
struct client{
	int c_index;
	struct sockaddr_in c_address;
	char * c_username;
	struct client *nextc;
	struct client *prevc;
};

int sockfd, addrlen; //Socket file descriptor and address length
struct sockaddr_in address; //*_in there represents inet IPV4

/*Clientside will have a selector menu that lists
  available servers(localhost and one that I'll be hosting)
  */
/*void quithandler(){
//Loop below will close connections with every client
struct client *c_temp = serverclient; //Serverclient pointer will hold the main chatroom(group one)
while(c_temp->nextc == NULL){
close(c_address);
c_temp=c_temp->nextc;
free(c_temp->prevc);
}
*/

int acceptsock(){ //Acceptsock will be called when a new client connects
	int newsock;
	if ((newsock = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
		fprintf(stderr,"\nError while accepting connection\n");
		exit(EXIT_FAILURE);
	}
	return newsock;
}

void mockread(){
	int sock;
	char buff[1024] = {0};
	while(1){
		sleep(1);
		sock=acceptsock();
		read(sock, buff, 1024);
		if(buff[0] != 0){
			system("clear");
			fprintf(stdout,"%s\n", buff);
			fprintf(stdout,"%d", sock);
		}
		buff[0] = 0;
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

