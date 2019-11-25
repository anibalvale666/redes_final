#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <algorithm>
#include <time.h>
#include <map>
#include <iterator>
#include <queue>

#include "auxiliaryfunctions.h"
#include "server.h"

using namespace std;

Server main_server;

int LengthSize = 4;

int NumeroServidores = 4;



void show_menu()
{
   cout<<"DISTRIBUTED DATABASE"<<endl;
   string input_text;

   while(true)
   {
       cout<<"> "<< endl;
       getline(cin, input_text);
       vector<string> words_input_text = split_into_words(input_text);
       main_server.parse_input(words_input_text);
   }
}

int main()
{
   //Start connections
   for(int i = 0; i < main_server.number_slaves; i++)
   {
       int SocketI = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//creando la interfaz
       if(SocketI == -1)
       {
           perror("can not create socket");
           exit(EXIT_FAILURE);
       }

       struct sockaddr_in SSocketAddr;

       memset(&SSocketAddr, 0, sizeof(struct sockaddr_in));
       SSocketAddr.sin_family = AF_INET;
       SSocketAddr.sin_port = htons(1100);

       int In = inet_pton(AF_INET, "127.0.0.1", &SSocketAddr.sin_addr);

       if (In < 0)
       {
           perror("first parameter is not a valid address family");
           close(SocketI);
           exit(EXIT_FAILURE);
       }
       else if (In == 0)
       {
           perror("char string (second parameter does not contain valid ipaddress");
           close(SocketI);
           exit(EXIT_FAILURE);
       }

       int status = connect(SocketI, (const struct sockaddr *)&SSocketAddr, sizeof(struct sockaddr_in));

       if(status == -1)
       {
           perror("connect failed");
           close(SocketI);
           exit(EXIT_FAILURE);
       }
       main_server.SocketsServers.push_back(SocketI);
   }

   show_menu();
   return 0;
}
