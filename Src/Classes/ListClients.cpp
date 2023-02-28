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

void ListClients::AddClient(Client &client) {
	_clients.push_back(client);
}

bool ListClients::isClientExist(int clientSocket) {
	int clientIdx = getClient(clientSocket);
	return (clientIdx != CLIENT_NOT_FOUND);
}

void ListClients::dropClient(int &clientIdx, fd_set &reads, fd_set &writes) 
{
	Client &client = _clients[clientIdx];
	SOCKET clientSocket = client.socket;
	FD_CLR(clientSocket, &reads);
	FD_CLR(clientSocket, &writes);
	if (ISVALIDSOCKET(clientSocket))
		CLOSESOCKET(clientSocket);
	client.socket = -1;
	if (client.clientInfos._requestHandler)
		delete client.clientInfos._requestHandler;
	if (client.clientInfos._responseHandler)
		delete client.clientInfos._responseHandler;
	client.clientInfos._requestHandler = nullptr;
	client.clientInfos._responseHandler = nullptr;
	if (client.fp)
		fclose(client.fp);
	client.fp = nullptr;
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
