/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/28 15:49:29 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/25 18:19:20 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SERVER_HPP
#define SERVER_HPP


#include "includes.hpp"
#include "static/ContentTypes.hpp"
#include "static/StaticResponseMessages.hpp"
#include "ListClients.hpp"

class Server
{
	private :
		ServerConfigs	_serverConfigs;
		SOCKET 			_serverSocket;
		ListClients		_clients;
		
	public :
		Server();

		ServerConfigs	&getServerConfigs();

		SOCKET getSocket() const;

		void setSocket(SOCKET socket);


		ListClients &getClients();

		void createSocket();


		void addClientSocket(std::pair<fd_set, fd_set> &ioSocket);

		Location &getBestMatchedLocation(std::string path);
		

		std::string genereteRandomName();

		std::string  getHeaderResponse(Client &client);

		bool isRedirection(StatusCode &responseCode) const;

		bool sendHeaderResponse(Client &client, fd_set &reads, fd_set &writes, int &clientIdx);

		void serve_resource(Client &client);

		void addClient(SOCKET &maxSocketSoFar, fd_set &reads, fd_set &writes);

		void closeHost();
};


#endif