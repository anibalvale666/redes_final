

class Server
{
private:
	int number_slaves;
public:
	Server();
	void send_command();
	void receive_command();
	void send_keep_alive();
	int hash_function(string node_name);
	void parse_input(vector<string> words_input_text);
	void operation_insert(vector<string> vector_input_command);
	void operation_delete(vector<string> vector_input_command);
	void operation_update(vector<string> vector_input_command);
	void operation_query(vector<string> vector_input_command);
}