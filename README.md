1. conexion con slaves, los sockets se almacenan en un vector de ints despues de que se ha establecido la conexion 
2. separa las palabras del query en un vector de strings
3. analiza el vector de palabras para identificar el tipo de operacion (insert, update, delete, ¿query?)
	3.1  insert 
		- se puede insertar nodo [I N nombre contenido] -> [insert node A julito]
			-se hashea la primera letra del nombre modulo numero de servidores
			-se envia comando IN seguido de la longitud del nombre + nombre + longitud del contenido + contenido
			-se manda el mensaje y se espera una respuesta, dependiendo de la respuesta se toma alguna decision
		- se puede insertar relaciones [I R nombre_nodo_A nombre_nodo_B]
			-se hashea la primera letra del primer nombre modulo numero de servidores
			-se hashea la primera letra del segundo nombre modulo numero de servidores
			-se manda el mensaje y se espera una respuesta, dependiendo de la respuesta se toma alguna decision
			(dado que no es grafo direccionado, se repiten los tres pasos pero a la inversa)
el socket (direccion) se obtiene en las funciones, en base al mensaje/contenido

Protocolo para "Insertar Nodo": 
	Código: 1
	Tamaño del nombre del nodo
	Nombre del nodo
	Tamaño del contenido del nodo
	Contenido del nodo


class AuxiliaryFunctions
{
	void MandarMensaje();
	string ConvertSize();
}

class Server 
{
	Communication.MandarMensaje();
	void operation_insert();
	
}

class Slave 
{
	Communication.MandarMensaje();
	... 
}
