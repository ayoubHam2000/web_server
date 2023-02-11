/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-amma <mel-amma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/28 15:06:20 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/11 17:09:17 by mel-amma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "includes.hpp"
#include "static/StatusCode.hpp"
#include "GetRequest.hpp"
#include "PostRequest.hpp"
// #include "server.hpp"

class A_Request; 
class ServerConfigs;
class Server;
#define ServerMap std::map<std::string, Server >

class Client
{
	public :
		socklen_t address_length;
		struct sockaddr_storage address;
		char address_buffer[128];
		SOCKET socket;
		char request[MAX_REQUEST_SIZE + 1];
		char *path;
		FILE *fp;
		int 		received;
		int 		responseCode;
		bool		sendError;
		A_Request   *requestHandler;
		bool		requestHeaderDone;
		ServerConfigs	*requestConfigs;// reset if we reset request?


		Client();
		
		Client(SOCKET socket);
		
		bool isRequestHeaderDone() const;

		ServerConfigs &getRequestConfigs()
		{
			return (*requestConfigs);
		}

		const char *get_address(); //return address client as string
		void set_error_code(int errorCode);

		void factoryRequestHandlerSetter();
void set_request_configs(ServerConfigs	*requestConfigs_);
};

#endif