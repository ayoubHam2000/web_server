/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-amma <mel-amma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/07 18:49:22 by mel-amma          #+#    #+#             */
/*   Updated: 2023/02/11 13:41:23 by mel-amma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "includes.hpp"
#include "static/StatusCode.hpp"
#include "ListClients.hpp"
#include "static/ContentTypes.hpp"
#include "static/StaticConfiguration.hpp"
#include "static/SupportedMethods.hpp"
#include "parsing/configParser/ServerConfigs.hpp"
#include "parsing/configParser/ConfigParser.hpp"
#include "parsing/requestParser/HeaderParser.hpp"
#include "parsing/configParser/Location.hpp"
#include "server.hpp"
#include "_http.hpp"
#include "A_Request.hpp"
#include "GetRequest.hpp"

void closeHosts(std::map<std::string, ServerMap > &servers);
bool CreateHostSockets(std::map<std::string, ServerMap >& servers, SOCKET &maxSocketSoFar, fd_set& reads, fd_set& writes);

#endif