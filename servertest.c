#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<signal.h>
#include<string.h>
#include<pthread.h>

#define PORT 17137 //Port that server is going to use

int thread_index = 1; //Will store id for the clients, also thread id

//ll to hold client data
struct client{
	int c_index;
	int c_address;
	char c_username[16];
	struct client *nextc;
};

int sockfd, addrlen; //Socket file descriptor and address length
struct sockaddr_in address; //*_in there represents inet IPV4

struct client *mainc; //Init the main client(groupchat), also root pointer for all clients.

//I WILL ADD THE SENDER RECEIVER INFO!!!!
/*Clientside will have a selector menu that lists
available servers(localhost and one that I'll be hosting)
*/

/*void quithandler(){
//Loop below will close connections with every client
struct client *c_temp = serverclient; //Serverclient pointer will hold the main chatroom(group one)
struct client *c_temp2 = NULL;
while(c_temp != NULL){
close(c_temp->c_address);
ctemp2 = c_temp; //Second temporary pointer that holds memory space to be freed;
c_temp=c_temp->nextc;
free(c_temp2);
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
//Listener for client, also the thread function
void *c_listener(void *_client){
	char *rmessage = NULL;
	while(1){
		sleep(1);

		read(((struct client*)_client)->c_address, rmessage, strlen(rmessage)); //every char is 1 byte anyways(ASCII)
		if(rmessage != NULL){ //If string is not empty
			if(strncmp(rmessage,"GONE", 4)){
				//kill thread function
			}
			else if(strncmp(rmessage,"MESG", 4)){
				//Send message function
				//Hanifi seninkiler buraya
					}
			else if(strncmp(rmessage,"MERR", 4)){
				//Resend the message in case of an error
					}
		}
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
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, &opt, sizeof(opt))) {
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
	if (listen(sockfd, 10) < 0) {
		fprintf(stderr,"\nSocket can't be listened\n");
		exit(EXIT_FAILURE);
	}
	//Initialise the first "client"(main chatroom) and the linked-list
	mainc=malloc(sizeof(struct client));
	mainc->nextc=NULL;
	strcpy(mainc->c_username, "main");

	//Connection listener, will stay at the last section
	int newaddr=0;
	char newuser[20];
	char newusername[16];
	int i=0; //Iterator for string loop below
	struct client *temp = mainc; //Temporary struct pointer to be passed in thread function
	pthread_t threads[10];
	while(1){
		sleep(1);
		newaddr=acceptsock();
		read(newaddr,newuser, 20);
		if(strncmp(newuser,"CONN", 4)){
			while(temp->nextc != NULL) temp=temp->nextc;
			//At the end of the ll
			//Extract the username from CONN signal into the newusername string
			for (i=6; newuser[i] != '\0'; i++) {
			newusername[i-6]=newuser[i];
			}
			strcpy(temp->c_username,newusername);//Put username in
			temp->c_address=newaddr;//Put the address in the ll to be passed into the thread func.
			temp->c_index=thread_index; //Client index global var started from 1
			temp->nextc=NULL;
			pthread_create(&threads[temp->c_index], NULL, c_listener, (void *)temp);
			thread_index++;
			pthread_join(temp->c_index, NULL);
		}
	}
	return 0;
}

