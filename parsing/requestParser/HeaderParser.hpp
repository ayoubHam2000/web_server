/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 12:56:24 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/09 16:55:47 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_PARSER_HPP
#define HEADER_PARSER_HPP

#include "../../includes.hpp"




class HeaderParser
{
	private :
		unsigned int _currPos;
		std::string _serverConfig;
	
	public :
		HeaderParser()
		{
			_currPos = 0;
		};
		
		HeaderParser(std::string _serverConfig)
		{
			this ->_serverConfig = _serverConfig;
			_currPos = 0;
		}

		bool	isDoneParsing() const
		{
			return (_currPos >= _serverConfig.length());
		}
		

		std::string getNextToken(char delimeter = ':')
		{
			std::string nextToken = "";
			while (_currPos < _serverConfig.length())
			{
				if ((_currPos + 1 < _serverConfig.length() && _serverConfig[_currPos] == '\r' && _serverConfig[_currPos + 1] == '\n')  || isspace(_serverConfig[_currPos]) || _serverConfig[_currPos] == delimeter)
				{
					if (nextToken.empty() &&  (_serverConfig[_currPos] == delimeter))
					{
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

		void pErrorParsing(const char *msg) const
		{
			std::cerr << msg << std::endl;
			exit (FAILURE);
		}

		void skipSpaces()
		{
			while (_currPos < _serverConfig.length() && _serverConfig[_currPos] != '\n' && isspace(_serverConfig[_currPos]))
				_currPos++;
		}

		std::string getValueCurrToken()
		{
			std::string value;
			skipSpaces();
			std::string delimter = getNextToken();
			if (delimter != ":")
				pErrorParsing("parsing request Error");
			
			while (_currPos < _serverConfig.length() && _serverConfig[_currPos] != '\n')
			{
				skipSpaces();
				if (_serverConfig[_currPos] == '\n')
					break;
				value += _serverConfig[_currPos++];
			}
			return (value);
		};

		std::vector <std::string> getValuesCurrToken()
		{
			std::vector <std::string> values;
			skipSpaces();
			std::string delimter = getNextToken();
			if (delimter == ":")
			{
				while (_currPos < _serverConfig.length() && _serverConfig[_currPos] != '\n')
				{
					skipSpaces();
					if (_serverConfig[_currPos] == '\n')
						break;
					std::string nextToken = getNextToken(',');
					if (nextToken != ",")
						values.push_back(nextToken);
				}
			}
			return (values);
		};
};

#endif
