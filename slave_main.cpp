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
#include "auxiliaryfunctions.h"
#include "slave.h"

using namespace std;

  int LengthSize = 4;
vector<thread> ThreadsRecibiendo;
Slave slave1;
vector<int> ClientSockets;

void WaitClient(int SocketClient){
    //recibo mensajes hasta hartarme, proceso la consulta y la devuelvo, este si es de enfoque tradicionarl
    int n;
    char * buffersize = new char [LengthSize];

    while(true){
        n = recv(SocketClient, buffersize, LengthSize, 0);
        if (n < 0){
            perror("Error size from socket");
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
                cout << "Mensaje recibido del cliente: " << smensaje << endl;
                slave1.ProccessMessage(SocketClient, smensaje);

            }
            delete [] buffermensaje;
        }

    }
    delete [] buffersize;
}

////debo escuchar clientes, puede haber varios gestores
void Listening(int SocketI){//aun que solo tendre un unico gestor, no se si necesite esto
    cout << "Listening" << endl;
    while(true){
        int in;
        in = accept(SocketI, NULL, NULL);//no conozco al participante activo
        //se supone que in, es un nuevo socket, el socket
        if (in < 0){
            perror("accept failed");
            //close(SocketI);
            exit(EXIT_FAILURE);
        }
        else{
            ClientSockets.push_back(in);
            //thread ThreadRecibiendoCliente;
            //ThreadRecibiendoCliente = thread(RecibiendoCliente, in);
            ThreadsRecibiendo.push_back(thread(WaitClient, in));

            //ThreadsEnviandoCliente.push_back(ThreadEnviando);
            //solo deberia haber un enviando
        }
    }
}


//inicialiazacion del socket con el numero de puerto como parametro
int init(int PuertoServer)
{
  int SocketI = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//creando la interfaz
  if(SocketI == -1) {
      perror("can not create socket");
      exit(EXIT_FAILURE);
  }

  struct sockaddr_in SSocketAddr;

  memset(&SSocketAddr, 0, sizeof(struct sockaddr_in));
  SSocketAddr.sin_family = AF_INET;
  SSocketAddr.sin_port = htons(PuertoServer);
  SSocketAddr.sin_addr.s_addr = INADDR_ANY;

  int status = bind(SocketI, (const struct sockaddr *)&SSocketAddr, sizeof(struct sockaddr_in));
  if(status == -1){
      perror("bind failed");
      close(SocketI);
      exit(EXIT_FAILURE);
  }

  status = listen(SocketI, 10);
  if(status == -1) {
      perror("listen failed");
      close(SocketI);
      exit(EXIT_FAILURE);
  }
  return SocketI;
}


int main(){
    int PuertoServer;//por el hecho de que probare en local host
    cout << "Introducir puerto para este servidor: ";
    cin >> PuertoServer;
    int SocketI = init(PuertoServer);
    cout << "iniciando servidor" << endl;

    thread EsperandoClients(Listening, SocketI);

    for(int i = 0; i < ThreadsRecibiendo.size(); i++){
        ThreadsRecibiendo[i].join();
    }

    EsperandoClients.join();

    return 0;
}
