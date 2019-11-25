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

using namespace std;

int LengthSize = 4;

int NumeroServidores = 4;

vector<int> SocketsServers;

void MandarMensaje(int SocketClient, string mensaje)
{
    cout << "Log mandando mensaje: " << SocketClient << " " << mensaje << endl;
    int sizetotal = mensaje.length();
    string size = ConvertSize(sizetotal, LengthSize);
    int n = send(SocketClient, size.c_str(), size.length(), 0);
    if(n < 0){
        perror("Error sending to socket");
    }

    n = send(SocketClient, mensaje.c_str(), mensaje.length(), 0);
    if(n < 0){
        perror("Error sending to socket");
    }
}

string RecibiendoCliente(int SocketClient){//no es para crear un thread, el comando que llamaa a esta funcion si es un thread
    //recibo mensajes hasta hartarme, proceso la consulta y la devuelvo, este si es de enfoque tradicionarl
    int n;
    char * buffersize = new char [LengthSize];

    bool esperando = true;
    while(esperando){
        n = recv(SocketClient, buffersize, LengthSize, 0);
        if (n < 0){
            //perror("Error size from socket");
        }
        if(n>0){
            int size = atoi(buffersize);
            char * buffermensaje = new char [size];

            n = recv(SocketClient, buffermensaje, size, 0);
            if (n < 0){
                perror("Error reading message from socket");
            }

            if(n>0){
                string smensaje (buffermensaje, size);
                cout << "Mensaje recibido del servidor: " << smensaje << endl;
                //ProccessMessage(SocketClient, smensaje);
                return smensaje;
                esperando = false;

            }
            delete [] buffermensaje;
        }
    }
    delete [] buffersize;
    return "";
}

void show_menu()
{
    cout<<"DISTRIBUTED DATABASE"<<endl;
    string input_text;

    while(true)
    {
        cout<<"> "<< endl;
        getline(cin, input_text);
        vector<string> words_input_text = split_into_words(input_text);
        ProccessCommand(words_input_text);
    }
}

int main()
{
    //Start connections
    Server main_server;

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
        SSocketAddr.sin_port = htons(1100 + i);

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
        SocketsServers.push_back(SocketI);
    }

    show_menu();
    return 0;
}