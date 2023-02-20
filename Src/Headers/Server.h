/***************************************************/
/*     created by TheWebServerTeam 2/17/23         */
/***************************************************/

#ifndef WEB_SERVER_SERVER_H
#define WEB_SERVER_SERVER_H

#include <ServerConfig.h>
#include "Client.h"

class Server {
private:
	const int 				_socket;
	const ServerConfig		&_serverConfig;
	std::map<int, Client> 	_clients;
private:
	Server& operator=(const Server& other);
public:

	Server(const int socket, const ServerConfig &serverConfig): _socket(socket), _serverConfig(serverConfig){}
	Server(const Server& other): _socket(other._socket), _serverConfig(other._serverConfig){}
	~Server(){};
public:

	const int &getSocket() const {
		return _socket;
	}

	const ServerConfig &getServerConfig() const {
		return _serverConfig;
	}

	const std::map<int, Client> &getClients() const {
		return _clients;
	}

public:

	void stopServer(){
		for (std::map<int, Client>::iterator iter = _clients.begin(); iter != _clients.end(); ++iter){
			close(iter->second.getSocket());
		}
	}

	void	addClientToRead(int &maxSocket, fd_set &read){
		for (std::map<int, Client>::iterator iter = _clients.begin(); iter != _clients.end(); ++iter){
			int socket = iter->second.getSocket();
			if (maxSocket < socket)
				maxSocket = socket;
			FD_SET(socket, &read);
		}
	}

};


#endif //WEB_SERVER_SERVER_H
