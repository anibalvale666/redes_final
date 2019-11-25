#include <iostream>
#include <vector>
#include <string>
#include "server.h"
#include "auxiliaryfunctions.h"

using namespace std;

Server::Server()
{
	
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
        int idx_dest_socket = Hash(vector_input_command[2]);
        
        string to_send = "1";
        to_send += ConvertSize(vector_input_command[2].length(), LengthSize);
        to_send += vector_input_command[2];
        to_send += ConvertSize(vector_input_command[3].length(), LengthSize);
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
        int idx_dest_socket_A = Hash(vector_input_command[2]);
    
        int idx_dest_socket_B = Hash(vector_input_command[3]);

        string size_name_A = ConvertSize(vector_input_command[2].length(), LengthSize);
        string size_name_B = ConvertSize(vector_input_command[3].length(), LengthSize);
        
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
