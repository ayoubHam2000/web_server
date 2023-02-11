#ifndef LIST_CLIENT_HPP
#define LIST_CLIENT_HPP


#include "client.hpp"

class Client;

class ListClients
{
	private:
		std::vector<Client> _clients;
	public:
	ListClients(){};
		int	getClient(SOCKET clientSocket) const
		{
			for (int i = 0; i < _clients.size(); i++)
			{
				if (_clients[i].socket == clientSocket)
					return (i);
			}
			return (-1);
		}


		void AddClient(Client client)
		{
			_clients.push_back(client);
		}

		bool isClientExist(SOCKET clientSocket)
		{
			int clientIdx = getClient(clientSocket);
			return (clientIdx != CLIENT_NOT_FOUND);
		}

		void dropClient(int &clientIdx, fd_set &reads, fd_set &writes)
		{
			SOCKET clientSocket = _clients[clientIdx].socket;
			FD_CLR(clientSocket, &reads);
			FD_CLR(clientSocket, &writes);
			CLOSESOCKET(clientSocket);
			_clients.erase(_clients.begin() + clientIdx);
			clientIdx--;
		}

		Client &operator [](int i)
		{
			if (i < 0 || i >= (int)_clients.size())
				throw std::out_of_range("Out Of Bounds");
			return (_clients[i]);
		}

		const Client &operator [](int i) const
		{
			if (i < 0 || i >= (int)_clients.size())
				throw std::out_of_range("Out Of Bounds");
			return (_clients[i]);
		}
		
		unsigned int getNumberClient() const
		{
			return (_clients.size());
		}
		
};


#endif