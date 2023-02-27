/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 12:56:24 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/26 13:52:46 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_PARSER_HPP
#define HEADER_PARSER_HPP

#include "includes.hpp"

class HeaderParser
{
	private :
		unsigned int 	_currPos;
		std::string 	_request;
	
	public :
		HeaderParser();
		
		HeaderParser(std::string _request);

		bool	isDoneParsing() const;

		bool isDelimeter(char c, std::string &delimeter);

		std::string getNextToken(std::string &delimeter);;

		std::string getHeaderValue();

		void pErrorParsing(const char *msg) const;

		void skipSpaces();

		std::vector <std::string> getValuesCurrToken(std::string &delimeters, bool &isErrorOccurs);

};



#endif
