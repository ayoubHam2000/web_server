/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/28 15:06:20 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/28 14:42:42 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "includes.hpp"
#include "StatusCode.hpp"
#include "A_Request.hpp"
#include "GetRequest.hpp"
#include "PostRequest.hpp"
#include "DeleteRequest.hpp"
#include "A_Response.hpp"
#include "NResponse.hpp"
#include "CGIResponse.hpp"
#include "CGI.hpp"
#include "configParser/Location.hpp"
#include "configParser/ServerConfigs.hpp"
#include "ClientInfos.hpp"

class A_Request;
class PostRequest;
class GetRequest;
class DeleteRequest;
class A_Response;
class NResponse;
class CGIResponse;
class CgiHandler;
class ServerConfigs;
class Server;


#define ServerMap std::map<std::string, Server >

class Client
{
	public :
		socklen_t 				address_length;
		struct sockaddr_storage addresStorage;
		SOCKET 					socket;
		char 					request[MAX_REQUEST_SIZE + 1];
		std::string 			path;
		FILE 					*fp;
		unsigned long long 		received;
		StatusCode 				responseCode;
		bool					sendError;
		//A_Request   			*requestHandler;
		//A_Response				*responseHandler;
		bool					requestHeaderDone;
		//ServerConfigs			*serverConfigs;// reset if we reset request?
		bool 					body_done;
		//Location				*bestLocationMatched;
		bool					isHeaderSend;
		bool					isForCgi;
		ClientInfos				clientInfos;
		CGI						cgiHandler;


		Client();
		
		Client(SOCKET socket);
		
		bool isRequestHeaderDone() const;

		ServerConfigs &getserverConfigs()
		{
			return (*(clientInfos._serverConfigs));
		}

		const char *get_address(); //return address client as string
		void set_response_code(StatusCode responseCode);

		void factoryRequestHandlerSetter();
		void factoryResponseHandlerSetter();
		void set_request_configs(ServerConfigs	*serverConfigs_);
		void finished_body();
		bool body_is_done();
		void setBestLocationMatched();
		void setServerConfigs( ServerMap& Servers);
		void setPathRessource();

		void tryOpenRessource();

		void setPathResponse();

		void listDirectoryIntoFile(std::string &path);
		void setClientInfo();

		void setIsRequestForCgi();

		void setIndexPath();

		void createResponseFile();


		~Client();

};

#endif