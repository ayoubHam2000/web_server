/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/28 16:34:04 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/27 12:12:03 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

std::map<std::string, ServerMap > servers;

void handler(int sig){
	if (sig == SIGINT){
		printf("Closing Server.\n");
		closeHosts(servers);
		exit(0);
	}
}

std::string readFile(std::string file)
{
	std::ifstream myfile (file);

	std::string fileContents;
	
	std::string serverConfigs = "";
	if ( myfile.is_open() ) 
	{
		std::string sa;
        while (getline(myfile, sa))
		
            fileContents += sa + "\n";
        myfile.close();
	}
	return (fileContents);
};

int main(int ac , char **av)
{
	std::srand(std::time(nullptr));
	if (ac != 2)
	{
		std::cerr << "number argument Not valid !" << std::endl;
		return (0);
	}
	signal(SIGINT, handler);
	std::ifstream myfile (av[1]);
	ConfigParser parser = ConfigParser(readFile(av[1]));
	
	//map<host+ip , <map <server name,server>>>
	SOCKET maxSocketSoFar = -1;
	fd_set reads , writes, readyReads, readyWrites;

	FD_ZERO(&reads), FD_ZERO(&writes), FD_ZERO(&readyWrites), FD_ZERO(&readyReads);
	servers = parser.parseServerConfig();
	if(!CreateHostSockets(servers, maxSocketSoFar, reads, writes))
		return (0);

	while (1)
	{
		try
		{
			readyReads = reads;
			readyWrites = writes;
			if (select(maxSocketSoFar + 1, &readyReads, &readyWrites, 0, 0) < 0) {
				fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
				break;
			}

			for (std::map<std::string, ServerMap >::iterator iter = servers.begin(); iter != servers.end(); ++iter){
				std::map<std::string, ServerMap >::value_type& xs = *iter;
				// first server with that ip + port of different host_names
				ServerMap::iterator::value_type &ser = *(xs.second.begin());
				Server &server = ser.second;//getting server from the map
				ListClients &clients = server.getClients();
				ft::Http http(reads, writes, clients, server);
				if (FD_ISSET(server.getSocket(), &readyReads))
					server.addClient(maxSocketSoFar, reads, writes);
				for (size_t i = 0; i < clients.getNumberClient(); i++)
				{
					if (FD_ISSET(clients[i].socket, &readyReads))
						http.getRequest(i,xs.second);
					if (i < clients.getNumberClient() && FD_ISSET(clients[i].socket, &readyWrites) && clients[i].body_is_done())
					{
						if (clients[i].isForCgi && clients[i].cgiHandler.getStatus() != CGI::DONE)
							clients[i].cgiHandler.read();
						if (clients[i].isForCgi == false || clients[i].cgiHandler.getStatus() == CGI::DONE)
							http.sendResponse(i);
					}
				}
			}
		}
		catch (std::exception &e)
		{
			std::cout << "error  " <<  e.what() << std::endl;
			continue;
		}
	}
	closeHosts(servers);
    return 0;
}
