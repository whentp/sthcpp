#include "server.h"
#include <iostream>

namespace bible{

using namespace std;

BibleSocket::BibleSocket(){

}

void BibleSocket::CreateSocket(u_short port){
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        exit("Could not create socket");
    }
    cout << "Socket created" << endl;
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        exit("bind failed. Error");
    }
    cout << "bind done" << endl;
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //end of string marker
		client_message[read_size] = '\0';
		
		//Send the message back to client
        write(sock , client_message , strlen(client_message));
		
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 



} // end namespace bible