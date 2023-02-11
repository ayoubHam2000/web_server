/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 12:56:24 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/10 15:32:35 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "../../server.hpp"
#include "../../webserv.hpp"

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
		ConfigParser()
		{
			_currPos = 0;
		};
		
		ConfigParser(std::string _serverConfig)
		{
			this ->_serverConfig = _serverConfig;
			_currPos = 0;
			processComments();
		}

		void processComments()
		{
			int i = 0;
			while (i < _serverConfig.length())
			{
				if (_serverConfig[i] == '#')
				{
					while (i < _serverConfig.length() && _serverConfig[i] != '\n')
					{
						_serverConfig[i] = ' ';
						i++;
					}
					
				}
				i++;
			}
		}

		std::string getNextToken()
		{
			std::string nextToken = "";
			while (_currPos < _serverConfig.length())
			{
				if (isspace(_serverConfig[_currPos]) || _serverConfig[_currPos] == '{' || _serverConfig[_currPos] == '}')
				{
					//std::cout << "_currPos = " << _currPos << " val = " << _serverConfig[_currPos] <<  std::endl;
					if (nextToken.empty() &&  (_serverConfig[_currPos] == '{' || _serverConfig[_currPos] == '}'))
					{
						//std::cout << "here = " << _currPos << " val = " << _serverConfig[_currPos] <<  std::endl;
						nextToken += _serverConfig[_currPos];
						_currPos++;
						break;
					}
					if (!nextToken.empty())
						break;
				}
				else
					nextToken += _serverConfig[_currPos];
				_currPos++;
			}
			return (nextToken);
		};

		void skipSpaces()
		{
			while (_currPos < _serverConfig.length() && _serverConfig[_currPos] != '\n' && isspace(_serverConfig[_currPos]))
				_currPos++;
		}

		std::vector <std::string> getValuesCurrToken()
		{
			std::vector <std::string> values;
			while (_currPos < _serverConfig.length() && _serverConfig[_currPos] != '\n')
			{
				skipSpaces();
				if (_serverConfig[_currPos] == '\n')
					break;
				std::string nextToken = getNextToken();
				if (nextToken == "{" || nextToken == "}")
				{
					_currPos--;
					break;
				}
				values.push_back(nextToken);
			}
			return (values);
		};
		
		void pErrorParsing(const char *msg) const
		{
			std::cerr << msg << std::endl;
			exit (FAILURE);
		}

		bool isInsideServer(std::stack <std::string> &bracketStack)
		{
			return (bracketStack.size() == 1);
		}

		std::pair <std::string, int > getConfig(std::map <std::string, int > &supportedConfigs, std::string & currConfig) const
		{
			//std::cout << "currConfig = " << currConfig << std::endl;
			std::map<std::string, int>::iterator it = supportedConfigs.find(currConfig);
			if (it == supportedConfigs.end())
			{
				std::string err  = "Config " + currConfig+   " Not Supported!!";
				pErrorParsing(err.c_str());
			}
			return (std::make_pair(currConfig, it ->second));	
		}


		void settingConfigsFactory(ServerConfigs &serverConfigs, bool isInsideServer ,\
									std::string &ConfigName , std::vector <std::string> &values)
		{
			if (!isInsideServer)
			{
				std::vector <Location>  &locations =  serverConfigs.getLocations();
				setLocationConfigs(locations[locations.size() - 1], ConfigName, values);
				return ;
			}
			setServerConfigs(serverConfigs, ConfigName, values);

		}
		
		void setLocationConfigs(Location &location, std::string &ConfigName , \
										std::vector <std::string> &values)
		{
			if (ConfigName == "allow_methods")
			{
				location.setAllowMethods(values);
			}
			else if (ConfigName == "location")
			{
				location.setRoute(values[0]);
			}
			else if (ConfigName == "redirect")
			{
				location.setRedirect(values[0]);
			}
			else if (ConfigName == "autoindex")
			{
				location.setAutoIndex(values[0] == "on");
			}
			else if (ConfigName == "root")
			{
				location.setRoot(values[0]);
			}
			else if (ConfigName == "index")
			{
				location.setIndexes(values);
			}
			else if (ConfigName == "upload_pass")
			{
				location.setUploadPass(values[0]);
			}
			else if (ConfigName == "cgi_pass")
			{
				location.addcgi(values[0], values[1]);
			}
		}

		int toInt(std::string &str)
		{
			int nb;
			try
			{
				nb = std::stoi(str);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				exit (FAILURE);
			}
			return (nb);
		}

		int getErrorCode(std::string &ErrorCode)
		{
			int nb = toInt(ErrorCode);

			if (nb < 400 || nb >= 600)
			{
				std::cerr << "errorCode not Valid" << '\n';
				exit (FAILURE);
			}
			return (nb);
		}

		void setServerConfigs(ServerConfigs &ServerConfigs, std::string &ConfigName , \
										std::vector <std::string> &values)
		{
			if (ConfigName == "listen")
			{
				ServerConfigs.setListen(values[0]);
			}
			else if (ConfigName == "server_name")
			{
				ServerConfigs.setServerName(values[0]);
			}
			else if (ConfigName == "max_client_body_size")
			{
				int MaxBodySize = toInt(values[0]);
				if (MaxBodySize < 0)
				{
					std::cerr << "Max Body size not Valid" << '\n';
					exit (FAILURE);
				}
				ServerConfigs.setMaxClientBodySize(MaxBodySize);
			}
			else if (ConfigName == "error_page")
			{	
				ServerConfigs.addErrorPage(getErrorCode(values[0]), values[1]);
			}
		}

		// void PrintServers(std::map<std::string, ServerMap >  &servers)
		// {
		// 	for (auto xs : servers)
		// 	{
		// 		for (auto a : xs.second)
		// 		{
					
		// 			std::cout << "******************************************\n\n";
		// 			ServerConfigs &serverConfigs = a.second.getServerConfigs();
		// 			std::map<int, std::string > & errorPages = serverConfigs.getErrorPages();
		// 			std::vector <Location> &locations = serverConfigs.getLocations();
		// 			std::cout << "listen = " << serverConfigs.getListen() << std::endl;
		// 			std::cout << "host = " << serverConfigs.getHost() << " serv = " << serverConfigs.getServ() << std::endl;
		// 			std::cout << "server_name = " << serverConfigs.getServerName() << std::endl;
		// 			std::cout << "maxClientBody size  = " << serverConfigs.getMaxClientBodySize() << std::endl;
		// 			for (auto xs : errorPages)
		// 				std::cout << "errorCode = " << xs.first << " path = " << xs.second << std::endl;
		// 			for (int j = 0; j < locations.size(); j++)
		// 			{
		// 				std::cout << "Location : " << locations[j].getRoute() << std::endl;
		// 				std::map <std::string, bool> &methods = locations[j].getAllowMethods();
		// 				std::cout << "allowMethods = ";
		// 				for (auto xs : methods)
		// 					std::cout << xs.first << " ";
		// 				std::cout << "\nredirection  = " << locations[j].getRedirect() << std::endl;
		// 				std::cout << "autoIndex  = " << locations[j].getAutoIndex() << std::endl;
		// 				std::cout << "root  = " << locations[j].getRoot() << std::endl;
		// 				std::cout << "index = ";
		// 				std::vector <std::string > &indexex = locations[j].getIndexes();
		// 				for (int k = 0; k < indexex.size(); k++)
		// 					std::cout << indexex[k] << " ";
		// 				std::cout << "\nupload pass  = " << locations[j].getUploadPass() << std::endl;
		// 				std::vector < std::pair <Language, std::string > > &cgis = locations[j].getCgis();
		// 				std::cout << "cgis = \n";
		// 				for (int k = 0; k < cgis.size(); k++)
		// 					std::cout << cgis[k].first << " " << cgis[k].second << std::endl;
		// 				std::cout << "\n\n";
		// 			}

		// 			std::cout << "******************************************\n\n";	
		// 			}
		// 	}
		// }

		std::map<std::string, ServerMap > parseServerConfig()
		{
			std::stack <std::string> bracketStack;
			std::map<std::string, ServerMap > servers;
			Server server;
			while (_currPos < _serverConfig.length() - 1)
			{
				std::string nextToken = getNextToken();
				if (bracketStack.empty())
				{
					if (nextToken == "server")
					{

						nextToken = getNextToken();
						if (nextToken != OPEN_BRACKET)
							pErrorParsing("Parsing Error");
						bracketStack.push(OPEN_BRACKET);
						server = Server();
					}
					else
						pErrorParsing("Parsing Error");
				}
				else
				{
					if (nextToken == CLOSE_BRACKET)
					{
						if (bracketStack.empty())
							pErrorParsing("Bracket Error");
						bracketStack.pop();
						if (bracketStack.empty())
						{
							//add server if its valid
							ServerConfigs &currServerConfigs = server.getServerConfigs();
							if (!currServerConfigs.isValidConfigs())
								pErrorParsing("Server Configuration Not Valid!");
							if (servers.find(currServerConfigs.getHost()+ currServerConfigs.getServ()) == servers.end())
							{
								ServerMap serverMap;
								serverMap[currServerConfigs.getServerName()] = server;
								servers[currServerConfigs.getHost()+ currServerConfigs.getServ()] = serverMap;
							}
							else
							{
								ServerMap &serverMap = servers[currServerConfigs.getHost()+ currServerConfigs.getServ()];
								if (serverMap.find(currServerConfigs.getServerName()) != serverMap.end())
									pErrorParsing("this Server Name already exist in this listen!!");
								serverMap[currServerConfigs.getServerName()] = server;
								servers[currServerConfigs.getHost()+ currServerConfigs.getServ()] = serverMap;
							}
						}
						continue;
					}
					std::map <std::string, int > supportedConfigs = StaticConfig::SERVER_CONFIGS[bracketStack.size() - 1];
					std::pair <std::string, int > currConfig = getConfig(supportedConfigs, nextToken);
					std::vector <std::string> values = getValuesCurrToken();
					if (values.size() == 0 || (currConfig.second != 0 &&  values.size() != currConfig.second ))
						pErrorParsing("values Config not Valid !!");
					if (nextToken == "location")
					{
						if (getNextToken() != OPEN_BRACKET || bracketStack.size() >= 2)
							pErrorParsing("Parsing Error");
						bracketStack.push(OPEN_BRACKET);
						server.getServerConfigs().getLocations().push_back(Location());
					}
					settingConfigsFactory(server.getServerConfigs(), isInsideServer(bracketStack), \
											nextToken, values);
				}
			}
			if (bracketStack.size())
				pErrorParsing("Bracket Error");
			//PrintServers(servers);
			return (servers);
		}
};

#endif
