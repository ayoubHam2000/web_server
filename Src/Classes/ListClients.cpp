# include "ListClients.hpp"

ListClients::ListClients() {}

int ListClients::getClient(int clientSocket) const {
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].socket == clientSocket)
			return (i);
	}
	return (-1);
}

void ListClients::AddClient(Client client) {
	_clients.push_back(client);
}

bool ListClients::isClientExist(int clientSocket) {
	int clientIdx = getClient(clientSocket);
	return (clientIdx != CLIENT_NOT_FOUND);
}

void ListClients::dropClient(int &clientIdx, fd_set &reads, fd_set &writes) {
	std::cout << "dropping client " << clientIdx << std::endl;
	SOCKET clientSocket = _clients[clientIdx].socket;
	FD_CLR(clientSocket, &reads);
	FD_CLR(clientSocket, &writes);
	CLOSESOCKET(clientSocket);
	// if (_clients[clientIdx].serverConfigs)
	// 	delete _clients[clientIdx].serverConfigs;
	_clients.erase(_clients.begin() + clientIdx);
	clientIdx--;
}

Client &ListClients::operator[](int i) {
	if (i < 0 || i >= (int)_clients.size())
		throw std::out_of_range("Out Of Bounds");
	return (_clients[i]);
}

const Client &ListClients::operator[](int i) const {
	if (i < 0 || i >= (int)_clients.size())
		throw std::out_of_range("Out Of Bounds");
	return (_clients[i]);
}

unsigned int ListClients::getNumberClient() const {
	return (_clients.size());
}
