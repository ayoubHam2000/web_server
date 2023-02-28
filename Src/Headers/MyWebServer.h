/***************************************************/
/*     created by TheWebServerTeam 2/17/23         */
/***************************************************/

#ifndef WEB_SERVER_MYWEBSERVER_H
#define WEB_SERVER_MYWEBSERVER_H

#include "Libraries.h"
#include "Configurations.h"
#include "Server.h"

class MyWebServer {
public:
	typedef std::vector<ServerConfig>							listConfType;
	typedef std::map<socketType, std::vector<ServerConfig*> >	mapServerType;
	typedef std::map<socketType, Client*> 						mapClientType;
public:
	static bool webServerIsRunning;
private:
	const static unsigned int max_listen = 100000;
private:
	listConfType	_configs;
	mapServerType	_servers;
	mapClientType	_clients;
private:
	MyWebServer(const MyWebServer &other);
	MyWebServer& operator=(const MyWebServer &other);
public:
	MyWebServer(){}
	~MyWebServer(){}

public:
/*****************************************************************/
// Core Functions
/*****************************************************************/

	void acceptNewClientInto(socketType serverSocket){
		Client* newClient = new Client(serverSocket, _servers[serverSocket]);
		if (newClient->getErrorFlag() == Client::NOT_ACCEPTED){
			std::cerr << "Client Not Accepted" << std::endl;
			delete newClient;
		} else {
			_clients[newClient->getSocket()] = newClient;
		}
	}

	void dropClient(Client *client){
		close(client->getSocket());
		_clients[client->getSocket()] = NULL;
		delete client;
	}

	void addExpectedReadersAndWriters(int &maxSocket, fd_set &reads, fd_set &writes){
		for (mapServerType::iterator iter = _servers.begin(); iter != _servers.end(); ++iter){
			if (maxSocket < iter->first)
				maxSocket = iter->first;
			FD_SET(iter->first, &reads);
		}
		for (mapClientType::iterator iter = _clients.begin(); iter != _clients.end(); ++iter){
			if (maxSocket < iter->first)
				maxSocket = iter->first;
			if (iter->second->getPhase() == Client::WAITING_REQ)
				FD_SET(iter->first, &reads);
			else if (iter->second->getPhase() == Client::SENDING_RES)
				FD_SET(iter->first, &writes);
		}
	}


	void readFromClient(Client* client){
		char chunk[read_chunk_size];
		size_type nbRead = read(client->getSocket(), chunk, read_chunk_size);
		if (nbRead < 1)
			throw std::runtime_error(TRACK_WHERE + "ERROR read");
		client->handleChunk(chunk, nbRead);
	}

	void writeToClient(Client* client){
		int status = client->sendResponse();
		if (status == Client::DONE || status == Client::FAILED){
			dropClient(client);
		}
	}


/*****************************************************************/
// Core
/*****************************************************************/

	void serve(fd_set &reads, fd_set &writes){
		for (mapServerType::iterator iter = _servers.begin(); iter != _servers.end(); ++iter){
			if (FD_ISSET(iter->first, &reads)){
				acceptNewClientInto(iter->first);
			}
		}
		for (mapClientType::iterator iter = _clients.begin(); iter != _clients.end(); ++iter){
			Client*	client = _clients[iter->first];
			try{
				if (FD_ISSET(client->getSocket(), &reads)){
					readFromClient(client);
				}else if (FD_ISSET(iter->first, &writes)){
					writeToClient(client);
				}
			} catch (std::exception &e){
				std::cerr << "Error: " << e.what() << std::endl;
				dropClient(client);
			}
		}

		//remove clients
		mapClientType::iterator iter = _clients.begin();
		while (iter != _clients.end()){
			if (iter->second == NULL){
				_clients.erase(iter++);
			} else {
				++iter;
			}
		}
	}

	void core(){
		while (webServerIsRunning){
			int maxSocket = 0;
			fd_set reads;
			fd_set writes;
			FD_ZERO(&reads);
			FD_ZERO(&writes);
			addExpectedReadersAndWriters(maxSocket, reads, writes);
			if (select(maxSocket + 1, &reads, &writes, 0, 0) < 0) {
				if (errno != EINTR)
					throw std::runtime_error(std::to_string(errno) + " : select failed.");
			}
			if (webServerIsRunning)
				serve(reads, writes);
		}
	}

/*****************************************************************/
// SetUp Functions
/*****************************************************************/

	void startWebServer(){
		MyWebServer::webServerIsRunning = true;
		std::cout << "Web Server Is running" << std::endl;
		core();
		stopWebServer();
	}

	void stopWebServer(){
		std::set<int> sockets;

		for (mapClientType::iterator iter = _clients.begin(); iter != _clients.end(); ++iter){
			close(iter->first);
			delete ((iter->second));
		}
		for (mapServerType::iterator iter = _servers.begin(); iter != _servers.end(); ++iter){
			close(iter->first);
		}
		std::cout << "WebServer is stopped" << std::endl;
	}


	socketType	createSocket(const std::string &host, const std::string &service){
		int				status;
		struct			addrinfo hints;
		struct addrinfo *bind_address;
		socketType 		socket_listen;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		status = getaddrinfo(host.c_str(), service.c_str(), &hints, &bind_address);
		if (status != 0) {
			throw std::runtime_error(std::string(std::string ("getaddrinfo failed: ") + gai_strerror(status)));
		}

		socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
		if (socket_listen == -1) {
			throw std::runtime_error(std::to_string(errno) + ": Failed to create socket");
		}

		status = bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen);
		if (status == -1) {
			throw std::runtime_error(std::to_string(errno) + ": Failed to create bind");
		}

		status = listen(socket_listen, max_listen);
		if (status == -1) {
			throw std::runtime_error(std::to_string(errno) + ": listen failed");
		}

		freeaddrinfo(bind_address);
		std::cout << "Listen On " << host << ":" << service << std::endl;
		return socket_listen;
	}

	void setServers(){
		std::map<std::string, int> 	mapListenSocket;
		socketType 					socket;

		for (listConfType::iterator iter = _configs.begin(); iter != _configs.end(); ++iter){
			std::string listen = iter->getListen();
			if (mapListenSocket.find(listen) == mapListenSocket.end()){
				socket = createSocket(iter->getHost(), iter->getService());
				_servers.insert(std::make_pair(socket, std::vector<ServerConfig*>()));
				mapListenSocket[listen] = socket;
			} else {
				socket = mapListenSocket[listen];
			}
			_servers[socket].push_back(&(*iter));
		}
	}

	void setConfigurations(const std::string &confPath){
		Configurations configurations(confPath);
		configurations.getConfigurations(_configs);
		configurations.checkConf(_configs);
		//configurations.display();
	}

};



#endif //WEB_SERVER_MYWEBSERVER_H
