/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 12:56:24 by klaarous          #+#    #+#             */
/*   Updated: 2023/03/02 15:46:33 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "Server.hpp"
#include "webserv.hpp"

#define OPEN_BRACKET "{"
#define CLOSE_BRACKET "}"
#define COMMENTAIRE "#"


#define ServerMap std::map<std::string, Server >


class ConfigParser
{
	private :
		unsigned int _currPos;
		std::string _serverConfig;
	
	public :
		ConfigParser();
		
		ConfigParser(std::string _serverConfig);

		void processComments();

		std::string getNextToken();

		void skipSpaces();

		std::vector <std::string> getValuesCurrToken();

		void pErrorParsing(const char *msg) const;

		bool isInsideServer(std::stack <std::string> &bracketStack);

		std::pair <std::string, int > getConfig(std::map <std::string, int > &supportedConfigs, std::string & currConfig) const;

		void settingConfigsFactory(ServerConfigs &serverConfigs, bool isInsideServer ,\
									std::string &ConfigName , std::vector <std::string> &values);

	   void setLocationConfigs(Location &location, std::string &ConfigName , \
										std::vector <std::string> &values);

		static unsigned long long toUnsigendLL(std::string &str);

		unsigned long long getErrorCode(std::string &ErrorCode);

		void setServerConfigs(ServerConfigs &ServerConfigs, std::string &ConfigName , \
										std::vector <std::string> &values);

		std::map<std::string, ServerMap > parseServerConfig();
};

#endif