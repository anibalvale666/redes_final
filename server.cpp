#include <iostream>
#include <vector>
#include <string>
#include "server.h"
#include "auxiliaryfunctions.h"

using namespace std;

Server::Server()
{
    l_size = 4;
}

int Server::hash_function(string node_name)
{
    return int(node_name[0]) % number_slaves;
}


void Server::parse_input(vector<string> words_input_text)
{
    if (words_input_text[0][0] == 'i')
    {
        operation_insert(words_input_text);
    }
    else if (words_input_text[0][0] == 'u')
    {
        cout<<"words_input_text: Update"<<endl;
    }
    else if (words_input_text[0][0] == 'd')
    {
        cout<<"words_input_text: Delete"<<endl;
    }
    else if (words_input_text[0][0] == 'q')
    {
        cout<<"Comando: Query"<<endl;
    }
}

void Server::operation_insert(vector<string> vector_input_command)
{
    //Insert new node with it's content
    if (vector_input_command[1][0] == 'n')
    {
        int idx_dest_socket = hash_function(vector_input_command[2]);

        string to_send = "1";
        to_send += size_to_string(vector_input_command[2].length(), l_size);
        to_send += vector_input_command[2];
        to_send += size_to_string(vector_input_command[3].length(), l_size);
        to_send += vector_input_command[3];

        MandarMensaje(SocketsServers[idx_dest_socket], to_send);

        string message_back = RecibiendoCliente(SocketsServers[idx_dest_socket]);

        if (message_back[0] == 'O')
        {
            cout << "Node insertado." << endl;
        }
        else
        {
            cout << "Nodo ya existe." << endl;
        }
    }

    //Insert new relationship between two nodes
    else if (vector_input_command[1][0] == 'r')
    {
        int idx_dest_socket_A = hash_function(vector_input_command[2]);

        int idx_dest_socket_B = hash_function(vector_input_command[3]);

        string size_name_A = size_to_string(vector_input_command[2].length(), l_size);
        string size_name_B = size_to_string(vector_input_command[3].length(), l_size);

        string to_send = "2";
        to_send += size_name_A;
        to_send += vector_input_command[2];
        to_send += size_name_B;
        to_send += vector_input_command[3];

        MandarMensaje(SocketsServers[idx_dest_socket_A], to_send);

        string message_back = RecibiendoCliente(SocketsServers[idx_dest_socket_A]);

        to_send = "2";
        to_send += size_name_B + vector_input_command[3];
        to_send += size_name_A + vector_input_command[2];

        MandarMensaje(SocketsServers[idx_dest_socket_B], to_send);

        message_back = RecibiendoCliente(SocketsServers[idx_dest_socket_B]);

        if (message_back[0] == 'O')
        {
            cout<<"Relacion insertada."<<endl;
        }
        else
        {
            cout<<"Error al insertar la relacion."<<endl;
        }
    }
}

void Server::send_command(int SocketClient, string mensaje)
{
   cout << "Log mandando mensaje: " << SocketClient << " " << mensaje << endl;
   int sizetotal = mensaje.length();
   string size = size_to_string(sizetotal, LengthSize);
   int n = send(SocketClient, size.c_str(), size.length(), 0);
   if(n < 0){
       perror("Error sending to socket");
   }

   n = send(SocketClient, mensaje.c_str(), mensaje.length(), 0);
   if(n < 0){
       perror("Error sending to socket");
   }
}

string receive_command(int SocketClient){//no es para crear un thread, el comando que llamaa a esta funcion si es un thread
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
