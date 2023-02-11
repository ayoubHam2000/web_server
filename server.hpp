/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-amma <mel-amma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/28 15:49:29 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/11 17:29:35 by mel-amma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
#define SERVER_HPP


#include "includes.hpp"
#include "static/ContentTypes.hpp"
#include "ListClients.hpp"

class Server
{
	private :
		ServerConfigs	_serverConfigs;
		SOCKET 			_serverSocket;
		ListClients		_clients;
		
	public :
		Server()
		{
			_serverSocket  = INVALID_FD;
		}

		ServerConfigs	&getServerConfigs()
		{
			return (_serverConfigs);
		}

		SOCKET getSocket() const
		{
			return (_serverSocket);
		}

		void setSocket(SOCKET socket) 
		{
			_serverSocket = socket;
		}


		ListClients &getClients()
		{
			return (_clients);
		}

		void createSocket()
		{
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

		

		void addClientSocket(std::pair<fd_set, fd_set> &ioSocket)
		{
			for (int i = 0; i < _clients.getNumberClient(); i++)
			{
				FD_SET(_clients[i].socket, &ioSocket.first);
				FD_SET(_clients[i].socket, &ioSocket.second);
			}
		}

		Location &getBestMatchedLocation(std::vector <Location> &locations, std::string path)
		{
			int idxBestLocation = 0;
			int maxLenMatched = 1;
			for (int i = locations.size() - 1 ; i > 0; i--)
			{
				if (locations[i].isRouteMatch(path))
				{
					if (locations[i].getRoute().length() >= maxLenMatched)
					{
						maxLenMatched = locations[i].getRoute().length();
						idxBestLocation = i;
					}
				}
			}
			return (locations[idxBestLocation]);
		}
		
		std::string getPathRessource(Location &bestLocationMatched, std::string path)
		{
			std::string currPath = bestLocationMatched.getRoot();
			if (currPath[currPath.length() - 1] != '/')
				currPath += "/";
			currPath += path.substr(bestLocationMatched.getRoute().length(), path.length());
			//std::cout << "currPath1 = " << currPath << std::endl;
			return (currPath); 
		}

		std::string genereteRandomName()
		{
			std::time_t ms = std::time(nullptr);
			std::stringstream ss;
			ss << ms;
			return (ss.str());
		}

		void listDirectoyIntoFile(Client &client, std::string &path)
		{
			
			DIR* dir = opendir(path.c_str());
			if (dir == NULL) {
				client.set_error_code(NOT_FOUND);
				return ;
			}
			std::string fileName = genereteRandomName() + ".html";
			std::string filePath = "/tmp/" +  fileName;
			FILE *listDir = fopen(filePath.c_str(),"wb");
			if (listDir == nullptr)
			{
				std::cout << "failed open file\n\n";
				return ;
			}
			dirent* entry = readdir(dir);
			std::string fileContent = "<html><head><title>Example Page</title></head><body><h1>List Files : </h1><ul>";
			
			while (entry != NULL) {
				std::string url = client.requestHandler->getPath();
				if (url[url.length() - 1] != '/')
					url += "/";
				url += entry->d_name;
				fileContent += "<li><a href=" + url  + ">" + entry->d_name +   "</a></li><br>";
				entry = readdir(dir);
			}
			closedir(dir);
			fileContent += "</body></html>";
			fputs(fileContent.c_str(),listDir );
			fclose(listDir);
			path = filePath;
			client.fp  = fopen(filePath.c_str(),"rb");
		}

		void  tryOpenRessource(std::string &path, Client &client, Location &bestLocation)
		{
			path = getPathRessource(bestLocation ,client.path);
			if (path.find("..") != std::string::npos)
			{
				client.responseCode = BAD_REQUEST;
				return ;			
			}
			struct stat s;
			if( stat(path.c_str(),&s) == 0 )
			{
				if( s.st_mode & S_IFDIR )
				{
					std::vector <std::string> &indexes =  bestLocation.getIndexes();
					for (int i = 0; i < indexes.size();i++)
					{
						std::string fullPath = path + "/" + indexes[i];
						if (fullPath.length() < 100)
						{
							client.fp = fopen(fullPath.c_str(), "rb");
							if (client.fp)
							{
								path = fullPath.c_str();
								break;
							}
						}
					}
					if (bestLocation.getAutoIndex())
					{
						listDirectoyIntoFile(client, path);
					}
				}
				else if( s.st_mode & S_IFREG )
				{
					client.fp = fopen(path.c_str(), "rb");
				}
			}
			if (client.fp == nullptr)
				client.responseCode = NOT_FOUND;
		}

		std::string getExtention(std::string &str)
		{
			std::string extention;
			size_t pos = str.rfind('.');
			if (pos != std::string::npos)
				extention = str.substr(pos, str.length());
			return (extention);
		}
		std::string  getHeaderResponse(Client &client, std::string &path, Location &bestLocation)
		{
			std::string headerRespone = client.requestHandler->getHttpVersion() + " " +  std::to_string(client.responseCode);
			fseek(client.fp, 0L, SEEK_END);
			size_t fileSize = ftell(client.fp);
			rewind(client.fp);
			std::string extention = getExtention(path);
			std::string contentType =  ContentTypes::getContentType(extention);
			if (client.responseCode == BAD_REQUEST)
				headerRespone += "  Bad Request ";
			else if (client.responseCode == NOT_FOUND)
				headerRespone += "  Not Found ";
			else if (client.responseCode == METHOD_NOT_ALLOWED)
			{
				headerRespone += "  Method Not Allowed\r\nAllow: ";
				std::map <std::string , bool > allowedMethod =  bestLocation.getAllowMethods();
				for (auto xs : allowedMethod)
					headerRespone += xs.first + ", ";
			}
			else
				headerRespone += " OK ";
			headerRespone += "\r\nConnection: close\r\nContent-Length: " + std::to_string(fileSize) +  "\r\nContent-Type: " + contentType + "\r\n\r\n";

			return (headerRespone);
		}
		
		void setPathError(Client &client, std::string &path)
		{
			path = client.getRequestConfigs().getErrorPage(client.responseCode);
			client.fp = fopen(path.c_str(), "rb");
		}


		bool sendHeaderResponse(Client &client, fd_set &reads, fd_set &writes, int &clientIdx)
		{
			std::string path = client.path;
			
			ServerConfigs serverConfig = client.getRequestConfigs();
			Location &bestLocationMatched = getBestMatchedLocation(serverConfig.getLocations(), client.path);
			std::cout << "request Path = " << path << " bestLocation : " << bestLocationMatched.getRoute() << " isErrorHappend = " << client.sendError << std::endl;
			if (!client.sendError)
			{
				if (bestLocationMatched.isMethodAllowed(client.requestHandler->getMethod()))
					tryOpenRessource( path, client, bestLocationMatched);
				else
					client.set_error_code(METHOD_NOT_ALLOWED);
			}
			if (client.fp == nullptr)
				setPathError(client, path);
			std::string responseHeader = getHeaderResponse(client, path, bestLocationMatched);
			std::cout << responseHeader << std::endl;	
			if (send(client.socket, responseHeader.c_str(), responseHeader.length(), 0)  == -1 || client.requestHandler->getMethod() == "HEAD")
			{
				fclose(client.fp);
				client.fp = nullptr;
				_clients.dropClient(clientIdx, reads, writes);
				return (false);
			}
			return (true);
		}

		void serve_resource(Client &client) {
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

		void addClient(SOCKET &maxSocketSoFar, fd_set &reads, fd_set &writes)
		{
			Client newClient;
			newClient.socket = accept(_serverSocket, (struct sockaddr *)&(newClient.address), &newClient.address_length);
			 if (!ISVALIDSOCKET(newClient.socket)) {
                fprintf(stderr, "accept() failed. (%d)\n",
                        GETSOCKETERRNO());
				return ;
            }

            printf("New connection from %d.\n",
                  newClient.socket);
			_clients.AddClient(newClient);
			fcntl(newClient.socket, F_SETFL,  O_NONBLOCK);
			FD_SET(newClient.socket, &reads);
			FD_SET(newClient.socket, &writes);
			maxSocketSoFar = std::max(maxSocketSoFar, newClient.socket);
		}

		void send_400(Client &client, fd_set &reads, fd_set &writes, int &clientIdx) {
			const char *c400 = "HTTP/1.1 400 Bad Request\r\n"
				"Connection: close\r\n"
				"Content-Length: 11\r\n\r\nBad Request";
			send(client.socket, c400, strlen(c400), 0);
			_clients.dropClient(clientIdx, reads, writes);
		}
		
		void send_404(Client &client, fd_set &reads, fd_set &writes, int &clientIdx) {
			 const char *c404 = "HTTP/1.1 404 Not Found\r\n"
        	"Connection: close\r\n"
        	"Content-Length: 9\r\n\r\nNot Found";
			send(client.socket, c404, strlen(c404), 0);
			_clients.dropClient(clientIdx, reads, writes);
		}

		void closeHost()
		{
			printf("\nClosing socket...\n");
			CLOSESOCKET(_serverSocket);
			printf("Finished.\n");
		}
};


#endif