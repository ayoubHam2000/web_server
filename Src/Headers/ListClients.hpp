#ifndef LIST_CLIENT_HPP
#define LIST_CLIENT_HPP


#include "Client.hpp"

class Client;

class ListClients
{
	private:
		std::vector<Client> _clients;
	public:
	ListClients();

	int	getClient(SOCKET clientSocket) const;


	void AddClient(Client &client);

	bool isClientExist(SOCKET clientSocket);

	void dropClient(int &clientIdx, fd_set &reads, fd_set &writes);

	Client &operator [](int i);

	const Client &operator [](int i) const;

	unsigned int getNumberClient() const;
		
};


#endif