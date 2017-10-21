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
#include <sys/sendfile.h>
#include <vector>
#include <iostream>     
#include <fstream> 
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
using namespace std;

void wait(int);
struct stones
{
	string ip;
	string port; 
};


class steppingStones
{
	int ssSize ;
	vector<stones> steps;
public:
	void setSteps(int size , vector<stones> s)
	{
		ssSize = size; 
		steps = s;
	}
	vector<stones> getSteps()
	{
		return steps; 
	}

};

vector<stones> parseFile(string file, int& ss)
{
	vector<stones> parsedFile; 
	ifstream inFile; 
	inFile.open(file);
	
	if(!inFile)
	{
		cout<<"File opened Fail!"<< endl; 
		exit(1); 
	}
	string inputData; 
	int flag = 0;
	while(getline(inFile, inputData))
	{
		if(flag == 0)
		{
			ss = stoi(inputData);
			flag = 1;
		}
		else
		{
			stones temp; 
			int location = inputData.find_first_of(" ");
			temp.ip = inputData.substr(0, location);
			temp.port = inputData.substr(location + 1 ,inputData.size());	
			cout<<temp.ip<<" "<<temp.port<<endl;
			parsedFile.push_back(temp);
		}
	}
	inFile.close( );
	return parsedFile; 
}
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
void signalHandler( int signum ) 
{
   cleanUp();
   exit(signum);  
}
// URLsize-NumbeofIps-url-ips..
string createPacket(vector<stones> temp, string wgetURL, int ss )
{
 	string packetTo = to_string(wgetURL.size());
 	string tempStones = "-";
 	tempStones.append(to_string(ss) + "-");
 	tempStones.append(wgetURL+"-");
 	for(stones t: temp)
 	{
 		tempStones.append(t.ip);
 		tempStones.append(" ");
 		tempStones.append(t.port);
 		tempStones.append(" ");

 	}
 	packetTo.append(tempStones);
 	

 	cout<<packetTo<<endl;
 	return packetTo;
}
int sendAll(int sd, string packet)
{
    const char* packetTosend = packet.c_str();
	int len = packet.size();
	int total = 0;        
    int bytesleft = len;
    int n;
    cout<<"Size of packet "<<len<<endl;
    cout<<"my packet " <<packet<<endl;
    while(total < len) {
        n = send(sd, packetTosend+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
    cout<<"Size sent "<<total<<endl;
    len = total; // return number actually sent here

    return n==-1?-1:0;
}
int sendFile(struct stones firstDest , string packet)
{
        struct addrinfo hints , *res;
        //char ipstr[INET6_ADDRSTRLEN];
        int socketfd ;
        struct sockaddr_in serv_addr;
       // struct stat stat_buf;
       //  off_t offset = 0;
        // char buffer[1024] = {0};
        //struct sockaddr_storage incoming_info;
        // socklen_t addr_size;
        // int new_fd;
        char name[128];
        gethostname(name,sizeof(name));
        memset(&hints, 0, sizeof hints); 
        hints.ai_family = AF_INET;     
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_socktype = SOCK_STREAM;
        if((getaddrinfo(name,"8080" , &hints, &res)) == -1 )
        {         
            printf("Oh dear, something went wrong with status()! %s\n", strerror(errno));
            return 1;
        }	
       if((socketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
       {
            printf("Oh dear, something went wrong with socket()! %s\n", strerror(errno));
            return 1;
       }
         globalSocket = socketfd;  
         serv_addr.sin_family = AF_INET;     
         serv_addr.sin_port = htons(stoi(firstDest.port));
         cout<<"Insisde: "<<firstDest.ip.c_str()<<endl;
         if(inet_pton(AF_INET, firstDest.ip.c_str(), &serv_addr.sin_addr)<=0) 
         { 
          printf("\nInvalid address/ Address not supported \n");
          return -1;
         }
        if (connect(socketfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
            close(socketfd);
            perror("client: connect");
            return 1;
        }
        cout<<"connected!"<<endl;	
        // int fd = open(fileName.c_str(), O_RDONLY);
        // fstat (fd, &stat_buf);
        // offset = 0;
        // remain_data = stat_buf.st_size;
      
        // cout<<wget<<endl;
        // //sends file
        // while (((bytes_sent = sendfile(socketfd, fd, &offset, stat_buf.st_size)) > 0) && (remain_data > 0))
        // {
        //         //(stdout, "1. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", bytes_sent, offset, remain_data);
        //         remain_data -= bytes_sent;
        //         //fprintf(stdout, "2. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", bytes_sent, offset, remain_data);
        // }


       	if(sendAll(socketfd,packet) == 0)
       	{
      	wait(socketfd);

       	}

        
        return 0;	
}
void wait(int socket)
{
  cout<<"waiting for return"<<endl;
  int valread;
  char buffer[1024];
    if((valread = recv(socket,buffer,1024,0) == 0))
    {
        cout<<"shit wasnt sent"<<endl;
    }
    else
    {
        cout<<"this is what we got: "<<endl;
        cout<<buffer<<endl;
    }
}

int getRand(int size)
{
	return rand() % size;
}


int main (int argc, char *argv[]) 
{ 
	string wgetURL;
	steppingStones Stones; 
	 
    char name[128];
    gethostname(name,sizeof(name));
    signal(SIGINT, signalHandler); 
    int ss;
	switch(argc)
	{
		case 3:
			{
			wgetURL = argv[1];
			
			string fileName = argv[2]; 
			std::vector<stones> temp = parseFile(fileName , ss);
			//int randomNumber = getRand(ss);
			sendFile(temp.at(0), createPacket(temp,wgetURL,ss) );

			break;
		   }  
		case 2: 
			{
			std::vector<stones> temp = parseFile("chaingang.txt" , ss);
			wgetURL = argv[1];
			string packet = createPacket(temp,wgetURL,ss);
			//give sendifile the random address and the packet. 
			sendFile(temp.at(0), packet);
			break; 
		    }
		default: 
		
			cout<<"Please provide a URL" << endl;
			exit(1);
			break;
	}





















}