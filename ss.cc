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
#define TRUE   1
#define FALSE  0
using namespace std;
//dont know if i wanna keep this arround. 
int globalSocket;
void printIp(struct addrinfo *test)
{
	   struct addrinfo *p;
	   char host[256];
        for(p = test; p != NULL; p = p->ai_next)
       {
        getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
        cout<<"Listening on: "<<host<<endl;
       }
}
void cleanUp()
{
	cout<<"Cleaning up sockets"<<endl;
	shutdown(globalSocket,SHUT_RDWR);
	close(globalSocket);
}
void signalHandler( int signum ) {
   cout << "Interrupt signal (" << signum << ") received.\n";
   cleanUp();
   exit(signum);  
}

//opens connection to listen for file. 
int openConnection()
{
        struct addrinfo hints, *res;
        //char ipstr[INET6_ADDRSTRLEN];
        int opt = TRUE;
        int socketfd, clientSocket[30],clientSocketNumber, b , maxFd, sd,activity, valread, maxClients=30;
        
        struct sockaddr_storage;
        struct sockaddr_in address;
        fd_set readfds;
        socklen_t addr_size;
        char buffer[1024] = {0};
        for(int i = 0 ; i <30; i++)
        {
            clientSocket[i] = 0;
        }


        char name[128];
        gethostname(name,sizeof(name));
        memset(&hints, 0, sizeof hints); // make sure the struct is empty
        hints.ai_family = AF_INET;     // don't care IPv4 or IPv6 AF_INET ANY_INADDR
        hints.ai_socktype = SOCK_STREAM;
        if((getaddrinfo(name,"8080" , &hints, &res)) == -1 )
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
        cout<<"Waiting for connection "<<endl;
        //intial connect
        
        while(TRUE)
        {
           
           FD_ZERO(&readfds);
           FD_SET(socketfd, &readfds);
           maxFd = socketfd;
           for(int i = 0 ; i < maxClients; i++)
           {
                
                sd = clientSocket[i];
                if(sd>0)
                {
                    FD_SET(sd , &readfds);
                }
                if(sd > maxFd)
                {
                    maxFd = sd;
                }
           }
           
           activity = select( maxFd+1  , &readfds , NULL , NULL , NULL);
           if ((activity < 0) && (errno!=EINTR)) 
           {
            printf("select error");
            }
           if(FD_ISSET(socketfd,&readfds))
           {
                if((clientSocketNumber=accept(socketfd, (struct sockaddr *) &address, &addr_size)) < 0)
                {
                    printf("Oh dear, something went wrong with accept()! %s\n", strerror(errno));
                    return 1;
                }
                cout<<"got connected: "<<clientSocketNumber<<endl;
                
                for(int i = 0 ; i < 30; i++)
                {
                    if(clientSocket[i] == 0)
                    {
                        clientSocket[i] = clientSocketNumber;
                        break; 
                    }
                }

                for(int i = 0 ; i < 30; i++)
                {
                    sd = clientSocket[i];
                    if(FD_ISSET(sd,&readfds))
                    {
                        cout<<"we are reading"<<sd<<endl;
                        if((valread = recv(sd,buffer,1024,0) == 0))
                        {
                            cout<<"shit wasnt sent"<<endl;
                        }
                        else
                        {
                            cout<<"this is what we got: "<<endl;
                            cout<<buffer<<endl;
                        }
                    }
                }
           }


        }
        return 0;	
}
void wget(string url)
{
    int loc = url.find_last_of(".");
    string fileEnding = url.substr(loc,sizeof(url));  
    string command = "wget " +url +" -O foundFile" +fileEnding;
    system(command.c_str()); 

}
int main()
{
	signal(SIGINT, signalHandler);  
	openConnection();
    //wget("http://speed.hetzner.de/100MB.bin");















}
