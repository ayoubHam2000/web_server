/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HostsManagement.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/07 18:44:42 by mel-amma          #+#    #+#             */
/*   Updated: 2023/02/09 12:13:29 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void closeHosts(std::map<std::string, ServerMap > &servers)
{
	for (std::map<std::string, ServerMap >::iterator iter = servers.begin(); iter != servers.end(); ++iter){
		std::map<std::string, ServerMap >::value_type& xs = *iter;
		for (ServerMap::iterator iter2 = xs.second.begin(); iter2 != xs.second.end(); ++iter2){
			ServerMap::value_type& ser = *iter2;
			if (!ISVALIDSOCKET(ser.second.getSocket()))
				ser.second.closeHost();
		}
	}
}


bool CreateHostSockets(std::map<std::string, ServerMap >& servers, SOCKET &maxSocketSoFar, fd_set& reads, fd_set& writes)
{
	for (std::map<std::string, ServerMap >::iterator iter = servers.begin(); iter != servers.end(); ++iter){
		std::map<std::string, ServerMap >::value_type& xs = *iter;
		try
		{
			ServerMap::iterator it = xs.second.begin();
			it->second.createSocket();
			SOCKET serverSocket = it->second.getSocket();
			FD_SET(serverSocket, &reads);
			FD_SET(serverSocket, &writes);
			maxSocketSoFar = std::max(maxSocketSoFar, serverSocket);
			it++;
			for (; it != xs.second.end(); it++)
				it->second.setSocket(serverSocket);
		}
		catch (const std::exception &e)
		{
			closeHosts(servers);
			return (false);
		}
	}
	return (true);
}