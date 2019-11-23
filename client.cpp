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

string ConvertSize(int n, int tam){
    string size = to_string(n);
    if(size.length() < tam){
        size = string(tam - size.length(), '0') + size;
    }
    return size;
}

int NumeroServidores = 4;

vector<int> SocketsServers;


int Hash(string name){
    //modulo 4 al caracter de la primera letra del termino
    return int(name[0]) % NumeroServidores;
}

void ConenctarConServidores(){
    //me conencto y los almaceno en SocketsServers
    for(int i = 0; i < NumeroServidores; i++){
        int SocketI = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//creando la interfaz
        if(SocketI == -1) {
            perror("can not create socket");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in SSocketAddr;

        memset(&SSocketAddr, 0, sizeof(struct sockaddr_in));
        SSocketAddr.sin_family = AF_INET;
        SSocketAddr.sin_port = htons(1100);
        //SSocketAddr.sin_addr.s_addr = INADDR_ANY;

        //int In = inet_pton(AF_INET, "192.168.1.9", &SSocketAddr.sin_addr);
        int In = inet_pton(AF_INET, "127.0.0.1", &SSocketAddr.sin_addr);

        if (In < 0) {
            perror("first parameter is not a valid address family");
            close(SocketI);
            exit(EXIT_FAILURE);
        }
        else if (In == 0) {
            perror("char string (second parameter does not contain valid ipaddress");
            close(SocketI);
            exit(EXIT_FAILURE);
        }

        int status = connect(SocketI, (const struct sockaddr *)&SSocketAddr, sizeof(struct sockaddr_in));
        if(status == -1){
            perror("connect failed");
            close(SocketI);
            exit(EXIT_FAILURE);
        }
        SocketsServers.push_back(SocketI);
    }

}

vector<string> SepararComando(string Mensaje){
    //buscar, agrgar, etc
    vector<string> res;
    int ianterior = 0;
    for(int i = 0; i < Mensaje.length(); i++){
        i = Mensaje.find(" ", i);
        if(i != -1){
            //cout << Mensaje.substr(ianterior, i - ianterior) << endl;
            res.push_back(Mensaje.substr(ianterior, i - ianterior));
            ianterior = i+1;
        }
        else{
            //cout << Mensaje.substr(ianterior) << endl;
            res.push_back(Mensaje.substr(ianterior));
            i = Mensaje.length();
        }
    }
    //se entiende que el ultimo sera un @
    return res;
}

void MandarMensaje(int SocketClient, string mensaje){
    cout << "Log mandando mensaje: " << SocketClient << " " << mensaje << endl;
    int sizetotal = mensaje.length();
    string size = ConvertSize(sizetotal, LengthSize);
    int n = send(SocketClient, size.c_str(), size.length(), 0);
    if(n < 0){
        perror("Error sending to socket");
    }
    //
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

// acceder a varios servidores
// debo tener una recepcoin que no sea un threas, pero que si recepcion hasta que encuentra un mensaje, sera un bussy wait

void InsertCommand(vector<string> CommandWords){
    //enviar, y esperar respuesta
    if(CommandWords[1] == "N"){//insertar nodo
        //determinar el socket
        int SocketServer = Hash(CommandWords[2]);
        //int SocketServer = 0;//para probar el servidor
        //MandarMensaje
        string Command = "IN";
        string sizename = ConvertSize(CommandWords[2].length(), LengthSize);
        Command += sizename + CommandWords[2];
        string sizevalue = ConvertSize(CommandWords[3].length(), LengthSize);
        Command += sizevalue + CommandWords[3];
        //string Size
        cout << "Mandando mensaje" << endl;
        MandarMensaje(SocketsServers[SocketServer], Command);
        //RecibirRespuesta
        cout << "RecibiendoRespueta" << endl;
        string respuesta = RecibiendoCliente(SocketsServers[SocketServer]);
        if(respuesta[0] == 'O'){
            cout << "Node insertado." << endl;
        }
        else{
            cout << "Nodo ya existe." << endl;
        }
        //respuesta unica ambas funciones
        //esta funcion tambien debe retornar algun resultado, al menos en estring o algo, dependiendo del tiepo de comando,
        //en este caso solo retornar el bool correcto o no , o un string con un comando
    }
    else if(CommandWords[1] == "R"){
        //int SocketServer1 = 0;//para probar el servidor
        int SocketServer1 = Hash(CommandWords[2]);
        //int SocketServer2 = 0;//para probar el servidor
        int SocketServer2 = Hash(CommandWords[3]);
        //las relaciones podrian mandarse en dos servidores distitnos si el hash los deriva
        //MandarMensaje
        string Command = "IR";
        string sizename1 = ConvertSize(CommandWords[2].length(), LengthSize);
        Command += sizename1 + CommandWords[2];
        string sizename2 = ConvertSize(CommandWords[3].length(), LengthSize);
        Command += sizename2 + CommandWords[3];
        //string Size
        cout << "Mandando mensaje" << endl;
        MandarMensaje(SocketsServers[SocketServer1], Command);
        //RecibirRespuesta
        cout << "RecibiendoRespueta" << endl;
        string respuesta = RecibiendoCliente(SocketsServers[SocketServer1]);
        //if(SocketServer1 != SocketServer2){
            Command = "IR";
            Command += sizename2 + CommandWords[3];
            Command += sizename1 + CommandWords[2];
            //string Size
            cout << "Mandando mensaje segunda insercion" << endl;
            MandarMensaje(SocketsServers[SocketServer2], Command);
            //RecibirRespuesta
            cout << "RecibiendoRespueta" << endl;
            respuesta = RecibiendoCliente(SocketsServers[SocketServer2]);
        //}
        if(respuesta[0] == 'O'){
            cout << "Relacion insertada." << endl;
        }
        else{
            cout << "Error al insertar la relacion." << endl;
        }
    }
}

void DeleteCommand(vector<string> CommandWords){
    if(CommandWords[1] == "N"){//insertar nodo
        //determinar el socket
        int SocketServer = Hash(CommandWords[2]);
        //int SocketServer = 0;//para probar el servidor
        //MandarMensaje
        string Command = "DN";
        string sizename = ConvertSize(CommandWords[2].length(), LengthSize);
        Command += sizename + CommandWords[2];
        //string Size
        cout << "Mandando mensaje" << endl;
        MandarMensaje(SocketsServers[SocketServer], Command);
        //RecibirRespuesta
        cout << "RecibiendoRespueta" << endl;
        string respuesta = RecibiendoCliente(SocketsServers[SocketServer]);
        if(respuesta[0] == 'O'){
            cout << "Node borrado." << endl;
        }
        else{
            cout << "Nodo no existe." << endl;
        }
        //respuesta unica ambas funciones
        //deberia procesar la respuesta, insertado con exito o cosas asi
        //esta funcion tambien debe retornar algun resultado, al menos en estring o algo, dependiendo del tiepo de comando,
        //en este caso solo retornar el bool correcto o no , o un string con un comando
    }
    else if(CommandWords[1] == "R"){
        //int SocketServer1 = 0;//para probar el servidor
        //int SocketServer2 = 0;//para probar el servidor
        int SocketServer1 = Hash(CommandWords[2]);//para probar el servidor
        int SocketServer2 = Hash(CommandWords[3]);//para probar el servidor
        //las relaciones podrian mandarse en dos servidores distitnos si el hash los deriva
        //MandarMensaje
        string Command = "DR";
        string sizename1 = ConvertSize(CommandWords[2].length(), LengthSize);
        Command += sizename1 + CommandWords[2];
        string sizename2 = ConvertSize(CommandWords[3].length(), LengthSize);
        Command += sizename2 + CommandWords[3];
        //string Size
        cout << "Mandando mensaje" << endl;
        MandarMensaje(SocketsServers[SocketServer1], Command);
        //RecibirRespuesta
        cout << "RecibiendoRespueta" << endl;
        string respuesta = RecibiendoCliente(SocketsServers[SocketServer1]);
        //if(SocketServer1 != SocketServer2){
            Command = "DR";
            Command += sizename2 + CommandWords[3];
            Command += sizename1 + CommandWords[2];
            //string Size
            cout << "Mandando mensaje" << endl;
            MandarMensaje(SocketsServers[SocketServer2], Command);
            //RecibirRespuesta
            cout << "RecibiendoRespueta" << endl;
            respuesta = RecibiendoCliente(SocketsServers[SocketServer2]);
        //}
        if(respuesta[0] == 'O'){
            cout << "Relacion borrada." << endl;
        }
        else{
            cout << "Error al borrar la relacion." << endl;
        }

    }
}

void UpdateCommand(vector<string> CommandWords){
    if(CommandWords[1] == "N"){//insertar nodo
        //determinar el socket
        int SocketServer = Hash(CommandWords[2]);
        //int SocketServer = 0;//para probar el servidor
        //MandarMensaje
        string Command = "UN";
        string sizename = ConvertSize(CommandWords[2].length(), LengthSize);
        Command += sizename + CommandWords[2];
        string sizevalue = ConvertSize(CommandWords[3].length(), LengthSize);
        Command += sizevalue + CommandWords[3];
        //string Size
        cout << "Mandando mensaje" << endl;
        MandarMensaje(SocketsServers[SocketServer], Command);
        //RecibirRespuesta
        cout << "RecibiendoRespueta" << endl;
        string respuesta = RecibiendoCliente(SocketsServers[SocketServer]);
        //respuesta unica ambas funciones
        //esta funcion tambien debe retornar algun resultado, al menos en estring o algo, dependiendo del tiepo de comando,
        //en este caso solo retornar el bool correcto o no , o un string con un comando
    }
    else if(CommandWords[1] == "R"){

    }
}

void ReadCommand(vector<string> CommandWords){
    if(CommandWords[1] == "N"){//insertar nodo
        //determinar el socket
        int SocketServer = Hash(CommandWords[2]);
        //int SocketServer = 0;//para probar el servidor
        //MandarMensaje
        string Command = "QN";
        string sizename = ConvertSize(CommandWords[2].length(), LengthSize);
        Command += sizename + CommandWords[2];
        //string Size
        cout << "Mandando mensaje" << endl;
        MandarMensaje(SocketsServers[SocketServer], Command);
        //RecibirRespuesta
        cout << "RecibiendoRespueta" << endl;
        string respuesta = RecibiendoCliente(SocketsServers[SocketServer]);

        if(respuesta[0] == 'O'){
            int TamValue = stoi(respuesta.substr(1, LengthSize));
            string Value = respuesta.substr(1 + LengthSize, TamValue);
            cout << "Node: " << CommandWords[2] << ", Value: " << Value << endl;
        }
        else{
            cout << "Nodo no existe." << endl;
        }
        //respuesta unica ambas funciones
        //deberia procesar la respuesta, insertado con exito o cosas asi
        //esta funcion tambien debe retornar algun resultado, al menos en estring o algo, dependiendo del tiepo de comando,
        //en este caso solo retornar el bool correcto o no , o un string con un comando
    }
    else if(CommandWords[1] == "R"){
        queue<pair<vector<string>, string>> Cola;
        vector<string> path;
        path.push_back(CommandWords[2]);
        int Deep = stoi(CommandWords[3]);
        Cola.push(pair<vector<string>, string>(path, CommandWords[2]));
        vector<string> visitados;
        for(int i = 0; i < Deep; i++){
            int TamCola = Cola.size();
            for(int j = 0; j < TamCola; j++){
                pair<vector<string>, string> p = Cola.front();
                visitados.push_back(p.second);
                Cola.pop();
                //consultar los vecinos de esta cosa, con una consulta de relacion simple
                //int SocketServer = 0;//has para p.first
                int SocketServer = Hash(p.second);//has para p.first
                string Command = "QR";
                string sizename = ConvertSize(p.second.length(), LengthSize);
                Command += sizename + p.second;
                //string Size
                cout << "Mandando mensaje" << endl;
                MandarMensaje(SocketsServers[SocketServer], Command);
                //RecibirRespuesta
                cout << "RecibiendoRespueta" << endl;
                string respuesta = RecibiendoCliente(SocketsServers[SocketServer]);
                //ahora analizo la respuesta
                if(respuesta[0] == 'L'){
                    //cuantos son?
                    int TamNumeroRegistros = stoi(respuesta.substr(1, LengthSize));
                    int NumeroRegistros = stoi(respuesta.substr(1+LengthSize, TamNumeroRegistros));
                    int offset = 1 + LengthSize + TamNumeroRegistros;
                    for(int k = 0; k < NumeroRegistros; k++){
                        int TamVecino = stoi(respuesta.substr(offset, LengthSize));
                        string Vecino = respuesta.substr(offset + LengthSize, TamVecino);
                        offset += LengthSize + TamVecino;
                        if(find(visitados.begin(), visitados.end(), Vecino) == visitados.end()){

                            pair<vector<string>, string> q;
                            vector<string> newpath = p.first;
                            newpath.push_back(Vecino);
                            q.first = newpath;
                            q.second = Vecino;
                            Cola.push(q);
                        }
                    }
                }
            }
        }
        //cola tiene todos los vecinos
        while(!Cola.empty()){
            pair<vector<string>, string> p = Cola.front();
            Cola.pop();
            vector<string> camino = p.first;
            for(int l = 0; l < camino.size()-1; l++){
                cout << camino[l] << "-";
            }
            cout << camino[camino.size()-1] << endl;
        }
    }
}

void ProccessCommand(vector<string> comando){//necesito saber
    //ifs segun el tipo de mesnsaje
    if(comando[0] == "I"){//diferentes comandos
        //verificar que tengan el numero de parametros adecuado
        InsertCommand(comando);//no necesito aun el socket
        //procesar comando, ses decir traduzco el comando para mandarlo al servidor
        //mandar mensaje
    }
    else if(comando[0] == "U"){
        //procesar comando
        UpdateCommand(comando);
        //mandar respuesta
    }
    else if(comando[0] == "D"){
        //procesar comando
        DeleteCommand(comando);
        //mandar respuesta
    }
    else if(comando[0] == "Q"){
        //procesar comando
        ReadCommand(comando);
        //mandar respuesta
    }
}
//el procesamiento de query se hace un otra funcion
    //esta funcion se encarga de mandar derivar el procesamiento en los CRUD
    //por ejemplo si es crear una relacion esta se debe guardar tanto en el servidor para el primer nodo y para el del segundo, entoces mandre el C a ambos servidores
        //cada CRUD es una funcion, que se la mando al servidore que necesito
        //estas funciones ademas esperando cada una respuesta, y recien devuelven algo
        //respecto a esos servidores me comporto como cliente, no como servidor
//que recibe el hash


void MenuLoop(){
    string query;
    while(true){
        cout << "GDB: " << endl;
        getline(cin, query);
        if(query == "exit"){
            break;
        }
        //Deteminar query
        vector<string> CommandWords = SepararComando(query);
        //procesar query
        ProccessCommand(CommandWords);
        //debe devolver un string con el estado del resultado,
    }
}


int main(){
    //conectarme con los servidores;
    ConenctarConServidores();
    //iniciar el mainloop
    MenuLoop();
    return 0;
}
