/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 12:38:10 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/21 16:13:07 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "server.hpp"
#include <iostream>
#include <filesystem>


Client::Client()
{
	received = 0;
	address_length = sizeof(addresStorage);
	socket = -1;
	fp = nullptr;
	responseCode = OK;
	sendError = isHeaderSend = false;
	requestHeaderDone = false;
	requestHandler = nullptr;
	body_done = false;
	bestLocationMatched = nullptr;
	isForCgi = false;
}


Client::Client(SOCKET socket)
{
	received = 0;
	address_length = sizeof(addresStorage);
	fp = nullptr;
	this->socket = socket;
	responseCode = OK;
	sendError = isHeaderSend = false;
	requestHeaderDone = false;
	requestHandler = nullptr;
	body_done = false;
	bestLocationMatched= nullptr;
	isForCgi = false;
}


bool Client::isRequestHeaderDone() const
{
	return (requestHeaderDone);
}

void Client::setClientInfo() //setting ip address and port for client
{

	char buffAddress[100];
	char buffPort[100];
	getnameinfo((struct sockaddr*)&addresStorage,
			address_length,
			buffAddress, sizeof(buffAddress),
			buffPort, sizeof(buffPort),
			NI_NUMERICHOST | NI_NUMERICSERV);
	addr = buffAddress;
	port = buffPort;
}
void Client::set_response_code(StatusCode responseCode)
{
	this->responseCode = responseCode;
	sendError = true;
}


void Client::factoryRequestHandlerSetter()
{
	if (strncmp("GET /", request, 5) == 0)
		requestHandler = new GetRequest();
	else if (strncmp("POST /", request, 5) == 0)
		requestHandler = new PostRequest();
	else if (strncmp("DELETE /", request, 5) == 0)
		requestHandler = new DeleteRequest();
	else
	{
		requestHandler = new GetRequest();
		set_response_code(METHOD_NOT_ALLOWED);
		finished_body();
	}
	
}

void Client::set_request_configs(ServerConfigs	*serverConfigs_)
{
	serverConfigs = serverConfigs_;
};

void Client::finished_body()
{
	body_done = true;
};

void Client::setBestLocationMatched()
{
	if (serverConfigs)
		bestLocationMatched = &(serverConfigs->getBestMatchedLocation(path));
}

bool Client::body_is_done()
{
	return body_done;
}


void Client::setServerConfigs( ServerMap& servers)
{
	A_Request::headersType headers = requestHandler->getHeaders();
	serverConfigs = &((servers.begin())->second.getServerConfigs());
	auto it = servers.find("Host");
	if (it == servers.end())
		return ;
	std::string host = headers.at("Host")[0];
	for (auto &serv : servers)
	{
		if (host == serv.first)
		{
			serverConfigs = &(serv.second.getServerConfigs());
			break ;
		}
	}

}

void Client::setPathResponse()
{
	if (fp)
		fclose(fp);
	std::string errorPath = serverConfigs->getResponsePage(responseCode);
	path = errorPath;
	fp = fopen(path.c_str(), "rb");
}

void Client::setPathRessource()
{
	Location &bestLocation = *bestLocationMatched;
	path = requestHandler->getPathRessource(bestLocation);
}

void Client::tryOpenRessource()
{
	if (!sendError)
	{
		Location &bestLocation = *bestLocationMatched;
		struct stat s;
		if( stat(path.c_str(),&s) == 0 )
		{
			if( s.st_mode & S_IFDIR )
			{
				std::vector <std::string> &indexes =  bestLocation.getIndexes();
				for (int i = 0; i < indexes.size();i++)
				{
					std::string fullPath = path + "/" + indexes[i];
					if (fullPath.length() < 100)
					{
						fp = fopen(fullPath.c_str(), "rb");
						if (fp)
						{
							path = fullPath;
							break;
						}
					}
				}
				if (!fp)
				{
					if ((bestLocation.getAutoIndex()) && requestHandler ->getMethod() != "DELETE")
						listDirectoryIntoFile(path);
					else
						set_response_code(FORBIDDEN);
				}	
			}
			else if( s.st_mode & S_IFREG )
				fp = fopen(path.c_str(), "rb");
		}
		if (fp == nullptr && !sendError)
			set_response_code(NOT_FOUND);
	}
}

void Client::listDirectoryIntoFile(std::string &path)
{
	DIR* dir = opendir(path.c_str());
	if (dir == NULL) {
		set_response_code(NOT_FOUND);
		return ;
	}
	std::string fileName = FileSystem::generateRandomString(20) + ".html";
	std::string filePath = "/tmp/" +  fileName;
	FILE *listDir = fopen(filePath.c_str(),"wb");
	if (listDir == nullptr)
	{
		std::cout << "failed open file\n\n";
		return ;
	}
	dirent* entry = readdir(dir);
	std::string fileContent = "<html><head><title>Example Page</title></head><body><h1>List Files : </h1><ul>";
	
	while (entry != NULL) {
		std::string url = requestHandler->getPath();
		//std::cout << "url = " << url  << " entryNam = " << entry->d_name << std::endl;
		if (url[url.length() - 1] != '/')
			url += "/";
		url += entry->d_name;
		//std::cout << "final URL = " << url << std::endl;
		fileContent += "<li><a href=" + url  + ">" + entry->d_name +   "</a></li><br>";
		entry = readdir(dir);
	}
	closedir(dir);
	fileContent += "</body></html>";
	fputs(fileContent.c_str(),listDir );
	fclose(listDir);
	//path = filePath;
	fp  = fopen(filePath.c_str(),"rb");
}

bool Client::isRequestForCgi()
{
	std::string extention = FileSystem::getExtention(path, true);
	std::string pathCgi = bestLocationMatched->getPathCgi(extention);
	if (!pathCgi.empty())
	{
		isForCgi = true;
		cgiPath = pathCgi;
	}
	return (!pathCgi.empty());
	
}

void Client::setupHeadersForCgi(std::string &cgiPath)
{
	requestHandler->addHeaderToCgi("script_name", cgiPath);
	requestHandler->addHeaderToCgi("server_protocol", requestHandler->getHttpVersion());
	requestHandler->addHeaderToCgi("server_port", serverConfigs->getServ());
	requestHandler->addHeaderToCgi("request_method", requestHandler->getMethod());
	requestHandler->addHeaderToCgi("path_info", path);
	std::string fullPath = FileSystem::get_current_dir();
	if (cgiPath[0] != '/')
		fullPath += '/';
	fullPath += path;
	requestHandler->addHeaderToCgi("path_translated", fullPath);
	requestHandler->addHeaderToCgi("query_string", requestHandler->getQuery());
	requestHandler->addHeaderToCgi("remote_addr", addr);
	requestHandler->addHeaderToCgi("remote_port", port);
}

