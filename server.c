#include<stdio.h>
#include<stdlib.h>
#include <strings.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<signal.h>
#include<string.h>
#include<pthread.h>

#define PORT 17137 //Port that server is going to use
FILE *logfile; //Pointer for logfile

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

static struct client *mainc; //Init the main client(groupchat), also root pointer for all clients.

/*Clientside will have a selector menu that lists
available servers(localhost and one that I'll be hosting)
*/

//Returns a pointer that holds the date as a string
char date_s[21];
char *getdate(){
	time_t T=time(NULL);
	struct tm date = *localtime(&T);

	snprintf(date_s, 20, "%d-%02d-%02d_%02d:%02d:%02d", date.tm_year+1900, date.tm_mon+1, date.tm_yday+1, date.tm_hour, date.tm_min, date.tm_sec);
	return date_s;
}

void quithandler(){
	printf("Server is halting\n");
	fprintf(logfile,"%sServer shuting down\n",getdate());
	//Loop below will close connections with every client
	struct client *c_temp = mainc; //Serverclient pointer will hold the main chatroom(group one)
	while(c_temp != NULL){
		printf("Released address:%d\n",c_temp->c_address);
		fprintf(logfile,"Released address:%d ", c_temp->c_address);
		close(c_temp->c_address);
		c_temp=c_temp->nextc;
	}
	fprintf(logfile, "\n");
	fclose(logfile);
	exit(1);
}

int acceptsock(){ //Acceptsock will be called when a new client connects
	int newsock;
	if ((newsock = accept(sockfd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
		fprintf(stderr,"Error while accepting connection\n");
		fprintf(logfile,"%s Error while accepting connection\n", getdate());
		exit(EXIT_FAILURE);
	}
	return newsock;
}

//Listener for client, also the thread function
void *c_listener(void *_client){
	char rmessage[1024];//Received message from the client
	struct client *iterator = mainc;
	char clientlist[200];

	//Variables for MESG section
	int i=0;
	time_t t;
	char target[17];

	send(((struct client *)_client)->c_address,"Connected Successfully", 22, 0);
	while(1){
		sleep(1);
		read(((struct client*)_client)->c_address, rmessage, 1024); //every char is 1 byte anyways(ASCII)
		if(rmessage[0] != 0){ //If string is not empty
			if(strncmp(rmessage,"GONE", 4) == 0){
				//kill thread function
				fprintf(logfile,"%s User %s has left, killing thread with internal id %d\n", getdate(), ((struct client *)_client)->c_username, ((struct client *)_client)->c_index);
				bzero(((struct client *)_client)->c_username, 16); //Can't bother with efficient memory management for 10 points, this is way better than expected as is tbh.
				pthread_exit(0);
			}
			else if(strncmp(rmessage,"MESG", 4) == 0){
			//Send message, but it has a change to be messed

				char *keeper=NULL; //keeps pointer for first encounter of > in the message
				keeper=strchr(rmessage,'>');
				keeper++;
				strcpy(target, keeper);
				printf("target:%s\n", target);//test
				//Find the receiver's address

				struct client *c_itr=mainc;//Reset iterator
				while((strcmp(target, c_itr->c_username) != 0)){
					c_itr=c_itr->nextc;
				}

				srand((unsigned) time(&t));
				//Send the actual message
				if(rand()%2){
					send(c_itr->c_address, rmessage, strlen(rmessage), 0);
				}
				else{
					send(c_itr->c_address,"aqwertdcvklunx", 15, 0);
				}

				bzero(target, 17);
				fprintf(logfile,"%s Sent Message%s", getdate(), rmessage);
				bzero(rmessage, 1024);
			}
			else if(strncmp(rmessage,"MERR", 4) == 0){
				char *keeper=NULL; //keeps pointer for first encounter of > in the message
				keeper=strchr(rmessage,'>');
				keeper++;
				strcpy(target, keeper);
				printf("%s MERR|Message targeted to:%s\n", getdate(), target);
				//Find the receiver's address

				struct client *c_itr=mainc;//Reset iterator
				while((strcmp(target, c_itr->c_username) != 0)){
					c_itr=c_itr->nextc;
				}

				//Send the actual message
				send(c_itr->c_address, rmessage, strlen(rmessage), 0);

				bzero(target, 17);
				fprintf(logfile,"%s ERROR while sending\n%s", getdate(), rmessage);
				bzero(rmessage, 1024);
			}

			//Send a list of clients to client that asks for it
			else if(strncmp(rmessage,"L", 1) == 0){
				bzero(clientlist, 200);
				strcat(clientlist, "List of Active Clients\n");
				while(iterator != NULL){
					strcat(clientlist, iterator->c_username);
					strcat(clientlist, "\n");
					iterator=iterator->nextc;
				}
				send(((struct client *)_client)->c_address, clientlist, sizeof(clientlist), 0);
			}
		}
		bzero(rmessage, 1024);//Empty received/sent messages in any case
	}
}

int main(int argc, char *argv[]){
	signal(SIGINT, quithandler);

	//Init logfile
	system("mkdir logs");
	char filename[30];
	snprintf(filename, 29, "logs/%s.log", getdate());

	logfile = fopen(filename, "w");
	if (logfile == NULL){
		fprintf(stderr, "\nError while creating file %s\n", filename);
	}
	else{
		printf("Logfile %s has been created", filename);
	}

	//Init socket
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
		fprintf(stderr,"Socket creation has failed\n");
		fprintf(logfile,"%s Socket creation has failed\n", getdate());
		exit(EXIT_FAILURE);
	}

	//Setsocketopt in case the address in use
	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, &opt, sizeof(opt))) {
		fprintf(stderr,"Socketopt could not be set\n");
		fprintf(logfile,"%s Socketopt could not be set\n", getdate());
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	//Final, binding section
	if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		fprintf(stderr,"Socket binding has failed\n");
		fprintf(logfile,"%s Socket binding has failed\n", getdate());
		exit(EXIT_FAILURE);
	}

	//Put the server socket in listening mode. 5 is the max length of the queue
	if (listen(sockfd, 10) < 0) {
		fprintf(stderr,"Socket could not be listened\n");
		fprintf(logfile,"%s Socket could not be listened\n", getdate());
		exit(EXIT_FAILURE);
	}
	//Initialise the first "client"(main chatroom) and the linked-list
	mainc=malloc(sizeof(struct client));
	mainc->nextc=NULL;
	strcpy(mainc->c_username, "Chatroom");

	//Connection listener, will stay at the last section
	int newaddr=0;//Holds newuser address
	char newuser[20]={0};//Holds message from new cliet
	char newusername[16]={0};//Holds username for new client
	int i=0; //Iterator for string loop below
	struct client *itr = mainc; //Temporary struct pointer to be passed in thread function
	pthread_t threads[10];
	printf("\nServer has started listening for connections\n");
	struct client *temp;
	while(1){
		sleep(1);
		newaddr=acceptsock();
		read(newaddr,newuser, 20);
		if(strncmp(newuser,"CONN", 4) == 0){
			while(itr->nextc != NULL){
				itr=itr->nextc;
			}
			temp=malloc(sizeof(struct client));//new ll item
			itr->nextc=temp;
			//At the end of the ll
			//Extract the username from CONN signal into the newusername string
			for (i=0; (newuser[i+5] != 0) | (newuser[i+5] > 255u); i++) {
			newusername[i]=newuser[i+5];
			}
			newusername[i+1] = '\0'; //Add null character to the end
			strcpy(temp->c_username,newusername);//Put username in
			temp->c_address=newaddr;//Put the address in the ll to be passed into the thread func.
			temp->c_index=thread_index; //Client index global var started from 1
			temp->nextc=NULL;
			fprintf(logfile,"%s Client connected with username %s, thread_id %d, address %d\n", getdate(), temp->c_username, temp->c_index, temp->c_address);
			printf("Client connected with username %s, thread_id %d, address %d\n",temp->c_username, temp->c_index, temp->c_address);
			pthread_create(&threads[temp->c_index], NULL, c_listener, (void *)temp);
			thread_index++;
		}
		//Reset temporary values for new clients;
		newaddr=0; i=0; bzero(newuser,20); bzero(newusername,16);
	}
	return 0;
}

