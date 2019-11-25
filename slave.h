#ifndef SLAVE_H
#define SLAVE_H

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


class Slave
{
public:
  //Variables globales
  multimap<string, string> NodeDB;

  multimap<string, string> RelationDB;

  int LengthSize;

  Slave()
  {
    LengthSize = 4;
  }

  void PrintServer(){
      cout << "Nodos" << endl;
      cout << "Nombre\tValor" << endl;
      for(multimap<string, string>::iterator it = NodeDB.begin(); it != NodeDB.end(); it++){
          cout << it->first << "\t" << it->second << endl;
      }
      cout << endl;
      cout << "Relaciones" << endl;
      cout << "Nombre 1 \tNombre 2" << endl;
      for(multimap<string, string>::iterator it = RelationDB.begin(); it != RelationDB.end(); it++){
          cout << it->first << "\t" << it->second << endl;
      }
  }

  void Sendmsg(int SocketClient, string mensaje){
      cout << "Log mandando mensaje: " << SocketClient << " " << mensaje << endl;
      int sizetotal = mensaje.length();
      string size = size_to_string(sizetotal, LengthSize);
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
  void ProccessMessage(int SocketClient, string mensaje){//necesito saber
      //ifs segun el tipo de mesnsaje
      if(mensaje[0] == 'I'){//diferentes comandos
          //procesar comando
          cout << mensaje << endl;
          if(mensaje[1] == 'N'){
              int TamNombre = stoi(mensaje.substr(2, LengthSize));
              string Nombre = mensaje.substr(2 + LengthSize, TamNombre);
              int TamValue = stoi(mensaje.substr(2 + LengthSize + TamNombre, LengthSize));
              string Value = mensaje.substr(2 + LengthSize + TamNombre + LengthSize, TamValue);
              multimap<string, string>::iterator it = NodeDB.find(Nombre);
              if(it == NodeDB.end()){//encontre algo
                  string respuesta = "O";
                  NodeDB.insert(pair<string, string>(Nombre, Value));
                  Sendmsg(SocketClient, respuesta);//borrado con exito
              }
              else{
                  Sendmsg(SocketClient, "E");//el nodo no existia
              }
          }
          else if(mensaje[1] == 'R'){
              int TamNombre1 = stoi(mensaje.substr(2, LengthSize));
              string Nombre1 = mensaje.substr(2 + LengthSize, TamNombre1);
              int TamNombre2 = stoi(mensaje.substr(2 + LengthSize + TamNombre1, LengthSize));
              string Nombre2 = mensaje.substr(2 + LengthSize + TamNombre1 + LengthSize, TamNombre2);
              multimap<string, string>::iterator it1 = NodeDB.find(Nombre1);
              //multimap<string, string>::iterator it2 = NodeDB.find(Nombre2);//no debo verificar el segund nodo
              if(it1 == NodeDB.end()){//encontre algo
                  string respuesta = "E";
                  Sendmsg(SocketClient, respuesta);//borrado con exito
              }
              /*else if(it2 == NodeDB.end()){
                  string respuesta = "E";
                  Sendmsg(SocketClient, respuesta);//borrado con exito
              }*/
              else{
                  //busco por rango el nombre 1
                  //dentro de los resultados veo si esta nombre 2, si esta no inserto, si no esta inserto
                  pair<multimap<string,string>::iterator, multimap<string, string>::iterator> range = RelationDB.equal_range(Nombre1);
                  bool ExistRelation = false;
                  for(multimap<string, string>::iterator it = range.first; it != range.second; it++){
                      if(it->second == Nombre2){
                          ExistRelation = true;
                      }
                  }
                  if(!ExistRelation){
                      RelationDB.insert(pair<string, string>(Nombre1, Nombre2));
                      string respuesta = "O";
                      Sendmsg(SocketClient, respuesta);//borrado con exito
                  }
                  else{
                      string respuesta = "E";
                      Sendmsg(SocketClient, respuesta);//borrado con exito
                  }
              }
          }
          //mandar respuesta
          //Sendmsg(SocketClient, mensaje);//por ahora lo mando de regreso
      }
      else if(mensaje[0] == 'U'){
          //procesar comando
          cout << mensaje << endl;
          if(mensaje[1] == 'N'){
              int TamNombre = stoi(mensaje.substr(2, LengthSize));
              string Nombre = mensaje.substr(2 + LengthSize, TamNombre);
              int TamValue = stoi(mensaje.substr(2 + LengthSize + TamNombre, LengthSize));
              string Value = mensaje.substr(2 + LengthSize + TamNombre + LengthSize, TamValue);
              multimap<string, string>::iterator it = NodeDB.find(Nombre);
              if(it != NodeDB.end()){//encontre algo
                  string respuesta = "O";
                  it->second = Value;
                  Sendmsg(SocketClient, respuesta);//borrado con exito
              }
              else{
                  Sendmsg(SocketClient, "E");//el nodo no existia
              }
          }
          else if(mensaje[1] == 'R'){

          }
          //mandar respuesta
      }
      else if(mensaje[0] == 'D'){
          //procesar comando
          cout << mensaje << endl;
          if(mensaje[1] == 'N'){
              int TamNombre = stoi(mensaje.substr(2, LengthSize));
              string Nombre = mensaje.substr(2 + LengthSize, TamNombre);
              multimap<string, string>::iterator it = NodeDB.find(Nombre);
              if(it != NodeDB.end()){//encontre algo
                  string respuesta = "O";
                  NodeDB.erase(it);
                  Sendmsg(SocketClient, respuesta);//borrado con exito
              }
              else{
                  Sendmsg(SocketClient, "E");//el nodo no existia
              }
          }
          else if(mensaje[1] == 'R'){
              int TamNombre1 = stoi(mensaje.substr(2, LengthSize));
              string Nombre1 = mensaje.substr(2 + LengthSize, TamNombre1);
              int TamNombre2 = stoi(mensaje.substr(2 + LengthSize + TamNombre1, LengthSize));
              string Nombre2 = mensaje.substr(2 + LengthSize + TamNombre1 + LengthSize, TamNombre2);
              multimap<string, string>::iterator it1 = RelationDB.find(Nombre1);
              multimap<string, string>::iterator it2 = NodeDB.find(Nombre2);//debo ver que existan los nodso
              if(it1 == NodeDB.end()){//encontre algo
                  string respuesta = "E";
                  Sendmsg(SocketClient, respuesta);//borrado con exito
              }
              else if(it2 == NodeDB.end()){
                  string respuesta = "E";
                  Sendmsg(SocketClient, respuesta);//borrado con exito
              }
              else{
                  //busco por rango el nombre 1
                  //dentro de los resultados veo si esta nombre 2, si esta no inserto, si no esta inserto
                  pair<multimap<string,string>::iterator, multimap<string, string>::iterator> range = RelationDB.equal_range(Nombre1);
                  bool ExistRelation = false;
                  multimap<string, string>::iterator iterase;
                  for(multimap<string, string>::iterator it = range.first; it != range.second; it++){
                      if(it->second == Nombre2){
                          ExistRelation = true;
                          iterase = it;
                      }
                  }
                  if(ExistRelation){
                      RelationDB.erase(iterase);
                      string respuesta = "O";
                      Sendmsg(SocketClient, respuesta);//borrado con exito
                  }
                  else{
                      string respuesta = "E";
                      Sendmsg(SocketClient, respuesta);//borrado con exito
                  }
              }

          }
          //mandar respuesta
      }
      else if(mensaje[0] == 'Q'){
          //procesar comando
          cout << mensaje << endl;
          if(mensaje[1] == 'N'){
              int TamNombre = stoi(mensaje.substr(2, LengthSize));
              string Nombre = mensaje.substr(2 + LengthSize, TamNombre);
              multimap<string, string>::iterator it = NodeDB.find(Nombre);
              if(it != NodeDB.end()){//encontre algo
                  string respuesta = "O";
                  string sizevalue = size_to_string(it->second.length(), LengthSize);
                  respuesta += sizevalue + it->second;
                  Sendmsg(SocketClient, respuesta);//por ahora lo mando de regreso
              }
              else{
                  Sendmsg(SocketClient, "E");//por ahora lo mando de regreso
              }
          }
          else if(mensaje[1] == 'R'){//para devolver los vecinos inmediatos
              int TamNombre = stoi(mensaje.substr(2, LengthSize));
              string Nombre = mensaje.substr(2 + LengthSize, TamNombre);
              multimap<string, string>::iterator it = NodeDB.find(Nombre);
              if(it != NodeDB.end()){//encontre algo
                  string respuesta = "L";
                  pair<multimap<string,string>::iterator, multimap<string, string>::iterator> range = RelationDB.equal_range(Nombre);
                  bool ExistRelation = false;
                  multimap<string, string>::iterator iterase;
                  vector<string> vecinos;
                  for(multimap<string, string>::iterator it = range.first; it != range.second; it++){
                      //tengo que agregarlos al comando
                      vecinos.push_back(it->second);
                  }
                  string numerovecinos = to_string(vecinos.size());
                  string sizenumerovecinos = size_to_string(numerovecinos.size(), LengthSize);
                  respuesta += sizenumerovecinos + numerovecinos;
                  for(int i = 0; i < vecinos.size(); i++){
                      string sizevecino = size_to_string(vecinos[i].length(), LengthSize);
                      respuesta += sizevecino + vecinos[i];
                  }
                  Sendmsg(SocketClient, respuesta);//por ahora lo mando de regreso
              }
              else{
                  Sendmsg(SocketClient, "E");//por ahora lo mando de regreso
              }

          }
          //mandar respuesta
      }
      PrintServer();
  }



};


#endif
