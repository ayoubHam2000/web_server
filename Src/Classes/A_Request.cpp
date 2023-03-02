/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   A_Request.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 14:53:29 by klaarous          #+#    #+#             */
/*   Updated: 2023/03/02 15:59:00 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "A_Request.hpp"
#include "Client.hpp"
#include "configParser/ConfigParser.hpp"


A_Request::A_Request()
{
	_isErrorOccurs = false;
};

std::string &A_Request::getPath() 
{
	return (_path);
}

std::string &A_Request::getMethod() 
{
	return (_method);
}

std::string &A_Request::getQuery()
{
	return (_query);
}


bool A_Request::isErrorOccured() const
{
	return (_isErrorOccurs);
}


std::string A_Request::getHeaderCgiValue(std::string header)
{
	std::string value;
	std::map<std::string , std::string >::iterator it = _headersForCgi.find(header);
	if (it != _headersForCgi.end())
		value = it ->second;
	return (value);
}


std::pair <std::string , std::string>  A_Request::parseCgiHeader(HeaderParser &parser)
{
	std::pair <std::string , std::string> p;
	std::string keysDelimeters = ":";
	p.first = parser.getNextToken(keysDelimeters);
	if (!p.first.empty())
		p.second= parser.getHeaderValue();
	return (p);
}


void A_Request::setHeadersForCgi(std::string &request)
{
	
	HeaderParser parser(request);
	parser.getHeaderValue();
	
	while (!parser.isDoneParsing())
	{
		std::pair <std::string , std::string> p = parseCgiHeader(parser);
		_headersForCgi[p.first] = p.second;
	}

}

void  A_Request::parsePath(HeaderParser &parser)
{
	HeaderPath headerPath;
	std::string keysDelimeters = ":";
	std::string path = parser.getNextToken(keysDelimeters);
	if (headerPath.parse(path))
	{
		_path = headerPath.getPath();
		_query = headerPath.getParams();
	}
	else
		_isErrorOccurs = true;

	
}

void A_Request::setHeadersRequest(std::string &request)
{
	std::string keysDelimeters = ":";
	std::string valuesDelimeters = ";,=:";
	//std::cout << request << std::endl;
	HeaderParser parser(request);
	_method = parser.getNextToken(keysDelimeters);
	
	parsePath(parser);
	//std::cout << "path = " << _path << " query = " << _query << std::endl;
	_httpVersion =  parser.getNextToken(keysDelimeters);
	while (!parser.isDoneParsing())
	{
		std::string requestHeader = parser.getNextToken(keysDelimeters);
		if (!requestHeader.empty())
		{
			std::vector < std::string >  values = parser.getValuesCurrToken(valuesDelimeters, _isErrorOccurs);
			_headers[requestHeader] = values;
			if (requestHeader == "Content-Type")
			{
				if (values.size() >= 3)
				{
					if (values[0] == "multipart/form-data")
					{
						if (values[1] != "boundary")
							_isErrorOccurs = true;
						else
							_headers["boundary"] = std::vector <std::string> (1,values[2]);
					}
				}
			}
		}
	}
}

void A_Request::parseRequestHeader(std::string &request)
{
	//std::cout << request << std::endl;
	setHeadersRequest(request);
	setHeadersForCgi(request);
}

std::string &A_Request::getHttpVersion()
{
	return (_httpVersion);
}

A_Request::headersType &A_Request::getHeaders()
{
	return (_headers);
}

std::string A_Request::getHeaderValue(const std::string header)
{
	std::string headerValue;
	std::map<std::string , std::vector < std::string > >::iterator it = _headers.find(header);
	if (it !=_headers.end() && !it->second.empty())
		headerValue = it->second[0];
	return (headerValue); 
}

bool A_Request::isRequestWellFormed(Client &client)
{
	bool isWellFormed = false;
	std::string transferEncoding = getHeaderValue("Transfer-Encoding");
	if (!transferEncoding.empty() && transferEncoding != "chunked")
	{
		client.set_response_code(NOT_IMPLEMENTED);
		return (false);
	}
	std::string contentLength = getHeaderValue("Content-Length");
	if (transferEncoding.empty() && contentLength.empty() && _method == "POST")
	{
		client.set_response_code(BAD_REQUEST);
		return (false);
	}
	if (!client.clientInfos._bestLocationMatched->isMethodAllowed(client.clientInfos._requestHandler->getMethod()))
	{
		client.set_response_code(METHOD_NOT_ALLOWED);
		return (false);
	}
	if (_path.length() > MAX_URI_SIZE)
	{
		client.set_response_code(REQUEST_URI_TOO_LONG);
		return (false);
	}
	if (client.clientInfos._bestLocationMatched && !client.clientInfos._bestLocationMatched->getRedirect().empty())
	{
		client.set_response_code(MOVED_PERMANETLY);
		return (false);
	}

	if (client.clientInfos._serverConfigs->size_limit_found() && !contentLength.empty() && ConfigParser::toUnsigendLL(contentLength) > client.clientInfos._serverConfigs->getMaxClientBodySize())
	{
		client.set_response_code(REQUEST_ENTITY_TOO_LARGE);
		return (false);
	}

	isWellFormed = true;
	return (isWellFormed);
}

//aaa/a..

bool A_Request::isValidPath()
{
	std::stringstream stream(_path + "/");
	std::string token;
	while (std::getline(stream, token, '/')){
		if (token == ".." || token == "."){
			return (false);
		}
	}
	return (true);
}



std::string A_Request::getPathRessource(Location &bestLocationMatched)
{
	std::string currPath = bestLocationMatched.getRoot();
	if (currPath[currPath.length() - 1] != '/')
		currPath += "/";
	currPath += _path.substr(bestLocationMatched.getRoute().length(), _path.length());
	return (currPath); 
}


void A_Request::addHeaderToCgi(const std::string headerKey, const std::string headerValue)
{
	_headersForCgi[headerKey] = headerValue;
}


void A_Request::setBodyAsFinished(Client &client)
{
	client.finished_body();
	client.set_response_code(CREATED);
}

bool A_Request::isCgiHeaderHasValue(const std::string &header)
{
	return (_headersForCgi.find(header) != _headersForCgi.end());
}


const std::string A_Request::getCgiHeaderValue(const std::string &header)
{
	return (_headersForCgi.at(header));
}


const std::map<std::string , std::string > &A_Request::getCgiMap() 
{
	return (_headersForCgi);
}

void A_Request::printCgisHeaders() {
	std::cout << "headers for cgis : \n\n";
	for (std::map<std::string , std::string >::iterator iter = _headersForCgi.begin(); iter != _headersForCgi.end(); ++iter){
		std::map<std::string , std::string >::value_type& xs = *iter;
		std::cout << xs.first << "=" << xs.second << std::endl;
	}
}

A_Request::~A_Request() {

}
