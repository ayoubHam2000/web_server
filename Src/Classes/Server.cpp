# include "Server.hpp"

Server::Server() {
	_serverSocket  = INVALID_FD;
}

ServerConfigs &Server::getServerConfigs() {
	return (_serverConfigs);
}

SOCKET Server::getSocket() const {
	return (_serverSocket);
}

void Server::setSocket(int socket) {
	_serverSocket = socket;
}

ListClients &Server::getClients() {
	return (_clients);
}

void Server::createSocket() {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *bind_address;
	getaddrinfo(_serverConfigs.getHost().c_str(), _serverConfigs.getServ().c_str(), \
			&hints, &bind_address);

	printf("Creating socket...\n");
	_serverSocket = socket(bind_address->ai_family,
						   bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(_serverSocket)) {
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		exit(1);
	}
	fcntl(_serverSocket, F_SETFL,  O_NONBLOCK);
	printf("Binding socket to local address...\n");
	if (bind(_serverSocket,
			 bind_address->ai_addr, bind_address->ai_addrlen)) {
		fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
		exit(1);
	}
	freeaddrinfo(bind_address);

	printf("Listening...\n");
	if (listen(_serverSocket, 10000) < 0) {
		fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
		exit(1);
	}
}

void Server::addClientSocket(std::pair<fd_set, fd_set> &ioSocket) {
	for (int i = 0; i < _clients.getNumberClient(); i++)
	{
		FD_SET(_clients[i].socket, &ioSocket.first);
		FD_SET(_clients[i].socket, &ioSocket.second);
	}
}

Location &Server::getBestMatchedLocation(std::string path) {
	return (_serverConfigs.getBestMatchedLocation(path));
}

std::string Server::genereteRandomName() {
	std::time_t ms = std::time(nullptr);
	std::stringstream ss;
	ss << ms;
	return (ss.str());
}

std::string Server::getHeaderResponse(Client &client) {
	std::string headerRespone = client.clientInfos._requestHandler->getHttpVersion() + " " +  std::to_string(client.responseCode);
	fseek(client.fp, 0L, SEEK_END);
	size_t fileSize = ftell(client.fp);
	rewind(client.fp);
	std::string extention = FileSystem::getExtention(client.path);
	std::string contentType =  ContentTypes::getContentType(extention);
	headerRespone += StaticResponseMessages::getMessageResponseCode(client.responseCode);
	if (client.responseCode == MOVED_PERMANETLY)
		headerRespone += "\r\nLocation: " + client.clientInfos._bestLocationMatched->getRedirect();
	headerRespone += "\r\nConnection: close\r\nContent-Length: " + std::to_string(fileSize) +  "\r\nContent-Type: " + contentType + "; charset=utf-8 \r\n\r\n";
	return (headerRespone);
}

bool Server::isRedirection(StatusCode &responseCode) const {
	return (responseCode >= 300 && responseCode < 400);
}

bool Server::sendHeaderResponse(Client &client, fd_set &reads, fd_set &writes, int &clientIdx) {
	if (client.sendError)
		client.setPathResponse();
	std::string responseHeader = getHeaderResponse(client);
	std::cout << responseHeader << std::endl;
	if (send(client.socket, responseHeader.c_str(), responseHeader.length(), 0)  == -1 ||\
			 		client.clientInfos._requestHandler->getMethod() == "HEAD")// || isRedirection(client.responseCode))
	{
		fclose(client.fp);
		client.fp = nullptr;
		_clients.dropClient(clientIdx, reads, writes);
		return (false);
	}
	client.isHeaderSend = true;
	return (true);
}

void Server::serve_resource(Client &client) {
	if (client.fp == nullptr)
		return ;
	char buffer[BUFFER_SIZE + 1];
	memset(buffer, 0, BUFFER_SIZE + 1);
	int r = fread(buffer, 1, BUFFER_SIZE, client.fp);
	if (r <= 0)
	{
		fclose(client.fp);
		client.fp = nullptr;
		return ;
	}
	if (send(client.socket, buffer, r, 0) == -1)
	{
		fclose(client.fp);
		client.fp = nullptr;
	}
}

void Server::addClient(int &maxSocketSoFar, fd_set &reads, fd_set &writes) {
	Client newClient;
	newClient.socket = accept(_serverSocket, (struct sockaddr *)&(newClient.addresStorage), &newClient.address_length);
	if (!ISVALIDSOCKET(newClient.socket)) {
		fprintf(stderr, "accept() failed. (%d)\n",
				GETSOCKETERRNO());
		return ;
	}
	newClient.setClientInfo();
	printf("New connection from %d.\n",
		   newClient.socket);
	_clients.AddClient(newClient);
	fcntl(newClient.socket, F_SETFL,  O_NONBLOCK);
	FD_SET(newClient.socket, &reads);
	FD_SET(newClient.socket, &writes);
	maxSocketSoFar = std::max(maxSocketSoFar, newClient.socket);
}

void Server::closeHost() {
	printf("\nClosing socket...\n");
	CLOSESOCKET(_serverSocket);
	printf("Finished.\n");
}
