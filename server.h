#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Server
{
public:
	int number_slaves;
	int l_size;
	vector<int> SocketsServers;

	Server();
	void send_command(int SocketClient, string mensaje);
	void receive_command(int SocketClient);
	void send_keep_alive();
	int hash_function(string node_name);
	void parse_input(vector<string> words_input_text);
	void operation_insert(vector<string> vector_input_command);
	void operation_delete(vector<string> vector_input_command);
	void operation_update(vector<string> vector_input_command);
	void operation_query(vector<string> vector_input_command);
};

#endif
