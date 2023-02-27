/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 11:42:23 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/26 17:28:12 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIResponse.hpp"
#include "A_Request.hpp"


CGIResponse::CGIResponse()
{
	
}

void CGIResponse::_setBody(std::string body)
{
	_body = body;
}



void CGIResponse::_parseCgiHeader(std::string buffer ,Client &client)
{
	size_t pos = buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
	{
		client.set_response_code(BAD_REQUEST);
		return ;
	}
	_header = buffer.substr(0, pos);
	this->_setBody(buffer.substr(pos + 4, buffer.length()));
	HeaderParser parser(_header);
	std::string keysDelimeters = ":";
	while (!parser.isDoneParsing())
	{
		std::pair <std::string , std::string> p = A_Request::parseCgiHeader(parser);
		_cgiHeaders.insert(p);
	}
}

bool	CGIResponse::_cgiHasHeader(const std::string &header)
{
	return (_cgiHeaders.find(header) != _cgiHeaders.end());
}



std::string CGIResponse::getHeaderResponse(Client &client)
{
	client.fp = fopen(client.cgiHandler.getOutFilePath().c_str(), "rb");
	if (!client.fp)
	{
		client.set_response_code(INTERNAL_SERVER_ERROR);
		client.createResponseFile();
	}
	char buffer[BUFFER_SIZE + 1];
	memset(buffer, 0, BUFFER_SIZE + 1);
	size_t sz = fread(buffer, 1,BUFFER_SIZE, client.fp);
	std::cout << "buffer === "  << buffer << std::endl;
	_parseCgiHeader(std::string(buffer, sz), client);
	StatusCode status = OK;
	if (_cgiHasHeader("Status"))
	{
		std::multimap<std::string, std::string >::iterator it = _cgiHeaders.find("Status");
		status =  static_cast <StatusCode>(stoi(it->second));
	}
	if (_cgiHasHeader("Location"))
		status =  MOVED_PERMANETLY;
	std::cout << "status = " << status << std::endl;
	std::string headerRespone = client.clientInfos._requestHandler->getHttpVersion() + " " +  std::to_string(status) + " ";
	headerRespone += StaticResponseMessages::getMessageResponseCode(status) + "\r\n";
	for (std::multimap<std::string, std::string >::iterator iter = _cgiHeaders.begin(); iter != _cgiHeaders.end(); ++iter){
		std::multimap<std::string, std::string >::value_type& xs = *iter;
		if (xs.first != "Status")
			headerRespone += xs.first + ": " + xs.second + "\r\n";
	}
	long int posCursor = ftell(client.fp);
	fseek(client.fp, SEEK_CUR, SEEK_END);
	size_t fileSize = ftell(client.fp);
	fseek(client.fp, posCursor, SEEK_SET);
	fileSize -= (_header.size() + 5);
	//std::cout << "fileSize = " << fileSize << std::endl;
	headerRespone += "Connection: close\r\nContent-Length: " + std::to_string(fileSize) +" \r\n\r\n";
	headerRespone += _body;
	return (headerRespone);
}