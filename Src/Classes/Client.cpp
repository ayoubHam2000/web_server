/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 12:38:10 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/28 16:50:02 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include <iostream>
#include <filesystem>

Client::Client()
{
	received = 0;
	address_length = sizeof(addresStorage);
	socket = 0;
	fp = nullptr;
	responseCode = OK;
	sendError = isHeaderSend = false;
	requestHeaderDone = false;
	body_done = false;
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
	body_done = false;
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
	clientInfos._addr = buffAddress;
	clientInfos._port = buffPort;
}
void Client::set_response_code(StatusCode responseCode)
{
	this->responseCode = responseCode;
	sendError = true;
}


void Client::factoryRequestHandlerSetter()
{
	A_Request *requestHandler;
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

	clientInfos.setRequestHandler(requestHandler);
	
}

void Client::factoryResponseHandlerSetter()
{
	A_Response *responseHandler;
	if (isForCgi)
		responseHandler = new CGIResponse();
	else
		responseHandler = new NResponse();
	clientInfos.setResponseHandler(responseHandler);
}


void Client::set_request_configs(ServerConfigs	*serverConfigs_)
{
	clientInfos.setServerConfigs(serverConfigs_);
};

void Client::finished_body()
{
	body_done = true;
};

void Client::setBestLocationMatched()
{
	if (clientInfos._serverConfigs)
		clientInfos._bestLocationMatched = &(clientInfos._serverConfigs->getBestMatchedLocation(path));
}

bool Client::body_is_done()
{
	return body_done;
}



void Client::setServerConfigs( ServerMap& servers)
{
	A_Request::headersType headers = clientInfos._requestHandler->getHeaders();
	clientInfos._serverConfigs = &((servers.begin())->second.getServerConfigs());
	ServerMap::iterator it = servers.find("Host");
	if (it == servers.end())
		return ;
	std::string host = headers.at("Host")[0];
	for (ServerMap::iterator iter = servers.begin(); iter != servers.end(); ++iter){
		ServerMap::value_type& serv = *iter;
		if (host == serv.first)
		{
			clientInfos._serverConfigs = &(serv.second.getServerConfigs());
			break ;
		}
	}


}

void Client::createResponseFile()
{
	std::string message = StaticResponseMessages::getMessageResponseCode(responseCode);
	std::string fileName = FileSystem::generateRandomString(10) + ".html";
	std::string filePath = "/tmp/" +  fileName;
	FILE *responseFile = fopen(filePath.c_str(),"wb");
	std::string fileContent = "<html><head><title> " + std::to_string(responseCode) + " " + message + "</title></head><body><h1>" + message + "</h1></body></html>";
	fputs(fileContent.c_str(),responseFile );
	fclose(responseFile);
	path = filePath;
	fp  = fopen(filePath.c_str(),"rb");
	if (!fp)
	{
		responseCode = INTERNAL_SERVER_ERROR;
		createResponseFile();
	}
	//std::cout << "file created : " << path << std::endl;
}


void Client::setPathResponse()
{
	if (fp)
		fclose(fp);
	std::string errorPath = clientInfos._serverConfigs->getResponsePage(responseCode);
	//std::cout << "response Code = " << responseCode << " errorPath = "<< errorPath << std::endl;
	path = errorPath;
	fp = fopen(path.c_str(), "rb");
	if (!fp)
		createResponseFile();
}

void Client::setPathRessource()
{
	Location &bestLocation = *clientInfos._bestLocationMatched;
	path = clientInfos._requestHandler->getPathRessource(bestLocation);
}


void Client::tryOpenRessource()
{
	if (!sendError)
	{
		Location &bestLocation = *clientInfos._bestLocationMatched;

		if (FileSystem::isDirectory(path.c_str()))
		{
			if (clientInfos._indexPath != path)
				fp = fopen(clientInfos._indexPath.c_str(), "rb");	
			if (fp)
				path = clientInfos._indexPath;
			else if ((bestLocation.getAutoIndex()))
				listDirectoryIntoFile(path);
			else
				set_response_code(FORBIDDEN);
		}	
		else
			fp = fopen(path.c_str(), "rb");
	}
	if (fp == nullptr && !sendError)
		set_response_code(NOT_FOUND);
}

void Client::listDirectoryIntoFile(std::string &path)
{
	DIR* dir = opendir(path.c_str());
	if (dir == NULL) {
		set_response_code(NOT_FOUND);
		return ;
	}
	std::string fileName = FileSystem::generateRandomString(10) + ".html";
	std::string filePath = "/tmp/" +  fileName;
	FILE *listDir = fopen(filePath.c_str(),"wb");
	if (listDir == nullptr)
	{
		set_response_code(INTERNAL_SERVER_ERROR);
		return ;
	}
	dirent* entry = readdir(dir);
	std::string fileContent = "<html><head><title>Example Page</title></head><body><h1>List Files : </h1><ul>";
	
	while (entry != NULL) {
		std::string url = clientInfos._requestHandler->getPath();
		if (url[url.length() - 1] != '/')
			url += "/";
		url += entry->d_name;
		fileContent += "<li><a href='" + url  + "'>" + entry->d_name +   "</a></li><br>";
		entry = readdir(dir);
	}
	closedir(dir);
	fileContent += "</body></html>";
	fputs(fileContent.c_str(),listDir );
	fclose(listDir);
	path = filePath;
	fp  = fopen(filePath.c_str(),"rb");
}

void Client::setIndexPath()
{
	Location &bestLocation = *clientInfos._bestLocationMatched;
	if (FileSystem::isDirectory(path.c_str()))
	{
		std::vector <std::string> &indexes =  bestLocation.getIndexes();
		for (size_t i = 0; i < indexes.size();i++)
		{
			std::string fullPath = path + "/" + indexes[i];
			if (FileSystem::file_exists(fullPath.c_str()))
			{
				clientInfos._indexPath = fullPath;
				break;
			}
		}
	}
}

void Client::setIsRequestForCgi()
{
	clientInfos._indexPath = path;
	setIndexPath();
	std::string extention = FileSystem::getExtention(clientInfos._indexPath, true);
	std::string pathCgi = clientInfos._bestLocationMatched->getPathCgi(extention);
	if (!pathCgi.empty())
	{
		isForCgi = true;
		clientInfos._cgiPath = pathCgi;
		std::cout << "is For Cgis*******\n\n";
	}
	
}

Client::~Client()
{

}