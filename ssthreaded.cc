#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <cstdint>
#include <stdint.h>
#include <string>
#include <iostream>
#include <csignal>
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>
#define TRUE   1
#define FALSE  0
using namespace std;
int globalSocket;
//cs.colostate.edu/~keagan21/hobbitLong.txt 
//uml doc to explain code 
string ip;
//information from incomping connection. 
struct data
{
	int length;
	string url;
	string file; 
};

void printIp(struct addrinfo *test)
{
	   struct addrinfo *p;
	   char host[256];
        for(p = test; p != NULL; p = p->ai_next)
       {
        getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
        ip = string(host);
        cout<<"Listening on: "<<host<<endl;
       }
}
void cleanUp()
{
	cout<<"Cleaning up sockets"<<endl;
	shutdown(globalSocket,SHUT_RDWR);
	close(globalSocket);
}
void signalHandler( int signum )
 {
   cout << "Interrupt signal (" << signum << ") received.\n";
   cleanUp();
   exit(signum);  
}
//serilize into packet structure 
//packet-protocol:: (packet/filesize)--wgeturl-file
int handleIncomingData(string data)
{
	cout<<"Incoming data:: "<<endl;
	cout<<data<<endl;
	return 0;
}
void *connectionHandler(void *socketDec)
{
	//open up connection 
	char buffer[1024];
	int valread;
	int sd = *(int*)socketDec;
	cout<<"we are reading"<<sd<<endl;
    memset(buffer ,0 , 1024); 
    if((valread = recv(sd,buffer,sizeof(buffer),0)) == -1)
    {
        cout<<"read failed"<<endl;
    }

    cout<<buffer<<endl;
    	
    
	return 0;
}

int createConnection(char * port)
{
	    struct addrinfo hints, *res;
        //char ipstr[INET6_ADDRSTRLEN];
        int opt = TRUE;
        int socketfd, b , clientSocketNumber;
      
        struct sockaddr_storage;
        struct sockaddr_in address;
        //fd_set readfds;
        socklen_t addr_size;
   
        char name[128];
        gethostname(name,sizeof(name));
        memset(&hints, 0, sizeof hints); // make sure the struct is empty
        hints.ai_family = AF_INET;     // don't care IPv4 or IPv6 AF_INET ANY_INADDR
        hints.ai_socktype = SOCK_STREAM;
        if((getaddrinfo(name,port, &hints, &res)) == -1 )
        {         
            printf("Oh dear, something went wrong with status()! %s\n", strerror(errno));
            return 1;
        }	
        printIp(res);
       if((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
       {
            printf("Oh dear, something went wrong with socket()! %s\n", strerror(errno));
            return 1;
       }
        globalSocket = socketfd;
        if( setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
        {
        perror("setsockopt");
        exit(EXIT_FAILURE);
        }
        if((b = bind(socketfd, res->ai_addr, res->ai_addrlen)) == -1)
        {
            printf("Oh dear, something went wrong with bind()! %s\n", strerror(errno));
            return 1;
        }
        if(listen(socketfd,3) != 0)
        {
            printf("Oh dear, something went wrong with listen()! %s\n", strerror(errno));
            return 1;
        }
        
	     pthread_t thread_id;
         while((clientSocketNumber=accept(socketfd, (struct sockaddr *) &address, &addr_size)) )
         {
            cout<<"Creating thread for connection"<<endl;
            if( pthread_create( &thread_id , NULL ,  connectionHandler ,  (void *) &clientSocketNumber) < 0)
            {
            perror("could not create thread");
            return 1;
            }
         }
        cout<<"Waiting for connection "<<endl;
        return 0;
}
void wget(string url)
{
    int loc = url.find_last_of(".");
    string fileEnding = url.substr(loc,sizeof(url));  
    string command = "wget " +url +" -O foundFile" +fileEnding;
    system(command.c_str()); 

}


int main(int argc, char *argv[])
{
	
    if(argc == 3 && strcmp(argv[2] , "-p"))
    {
    createConnection(argv[2]);

    }
    else
    {   
        cout<<"Please provide a port number"
        <<"Example  ./a.out -p 8080"<<endl;
        return 0;
    }















}
