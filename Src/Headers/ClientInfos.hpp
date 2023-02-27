/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientInfos.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 17:51:10 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/25 19:09:00 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_INFOS_HPP
#define CLIENT_INFOS_HPP

#include "includes.hpp"
#include "StatusCode.hpp"
#include "A_Request.hpp"
#include "A_Response.hpp"
#include "configParser/Location.hpp"
#include "configParser/ServerConfigs.hpp"

struct ClientInfos
{
	A_Request 				*_requestHandler;
	ServerConfigs			*_serverConfigs;
	Location				*_bestLocationMatched;
	A_Response				*_responseHandler;
	std::string				_cgiPath;
	std::string				_indexPath;
	std::string				_cgiFilePath;
	std::string				_addr;
	std::string				_port;
	

	ClientInfos();;
	
	void setRequestHandler(A_Request *req);

	void setResponseHandler(A_Response *res);

	void setServerConfigs(ServerConfigs *conf);


	void setBestLocation(Location *location);
	

	void setCgiPath(std::string cgiPath);

	void setIndexPath(std::string indexPath);

	void setCgiFilePath(std::string cgiFilePath);

	~ClientInfos();
};

#endif