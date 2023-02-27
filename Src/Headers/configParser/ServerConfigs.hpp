/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfigs.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/03 15:17:55 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/24 14:15:35 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIGS_HPP
#define SERVER_CONFIGS_HPP

#include "Location.hpp"
#include "includes.hpp"

class ServerConfigs
{
	private :
		std::string 				_listen;
		std::string					_host;
		std::string					_serv;
		std::string 				_serverName;
		unsigned long  long			_maxClientBodySize;
		std::map<int , std::string> _errorPages;
		std::vector <Location> 		_locations;
		bool						size_limit_set;

	public :
		ServerConfigs();;

		std::vector <std::string> getDefaultAllowedMethods();

		Location getDefaultLocation();
		
		//getters
		std::string &getListen();

		unsigned long long &getMaxClientBodySize();

		std::string getHost();

		std::string getServ();

		std::string &getServerName();

		std::map<int , std::string> &AllgetResponsePages();

		std::string &getResponsePage(int errorCode);

		std::vector <Location> &getLocations();

		//setters

		void setListen(std::string listen);

		void setHostAndPort(std::string host, std::string port);

		void setServerName(std::string serverName);

		void setMaxClientBodySize(unsigned long long maxClientBodySize);

		void setErrorPages(std::map<int , std::string> errorPages);

		void setLocations(std::vector <Location> locations);

		bool isValidConfigs() const;

		void addErrorPage(int errorCode, std::string path);

		Location &getBestMatchedLocation(std::string &path);

		void size_limit_in();

		bool size_limit_found() const;
};


#endif