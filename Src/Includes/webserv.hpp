/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/07 18:49:22 by mel-amma          #+#    #+#             */
/*   Updated: 2023/02/25 17:39:09 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP


#include "includes.hpp"
#include "StatusCode.hpp"
#include "ListClients.hpp"
#include "static/ContentTypes.hpp"
#include "static/StaticConfig.hpp"
#include "static/SupportedMethods.hpp"
#include "static/StaticConfig.hpp"
#include "configParser/ServerConfigs.hpp"
#include "configParser/ConfigParser.hpp"
#include "requestParser/HeaderParser.hpp"
#include "configParser/Location.hpp"
#include "Server.hpp"
#include "Http.hpp"
#include "A_Request.hpp"
#include "GetRequest.hpp"

void closeHosts(std::map<std::string, ServerMap > &servers);
bool CreateHostSockets(std::map<std::string, ServerMap >& servers, SOCKET &maxSocketSoFar, fd_set& reads, fd_set& writes);

#endif