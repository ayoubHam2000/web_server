#include "configParser/ServerConfigs.hpp"

ServerConfigs::ServerConfigs() {
	_locations = std::vector <Location>();
	_locations.push_back(getDefaultLocation());
	_serv = "8080";
	size_limit_set = false;
}

std::vector<std::string> ServerConfigs::getDefaultAllowedMethods() {
	std::vector <std::string> defaultAllowedMethods;
	defaultAllowedMethods.push_back("GET");
	defaultAllowedMethods.push_back("POST");
	defaultAllowedMethods.push_back("DELETE");
	return (defaultAllowedMethods);
}

Location ServerConfigs::getDefaultLocation() {
	Location defaultLocation = Location();
	defaultLocation.setRoute("/");
	defaultLocation.setAllowMethods(getDefaultAllowedMethods());
	defaultLocation.setRoot("public");
	std::vector <std::string > indexes;
	indexes.push_back("index.html");
	defaultLocation.setIndexes(indexes);
	return (defaultLocation);
}

std::string &ServerConfigs::getListen() {
	return (_listen);
}

unsigned long long &ServerConfigs::getMaxClientBodySize() {
	return (_maxClientBodySize);
}

std::string ServerConfigs::getHost() {
	return (_host);
}

std::string ServerConfigs::getServ() {
	return (_serv);
}

std::string &ServerConfigs::getServerName() {
	return (_serverName);
}

std::map<int, std::string> &ServerConfigs::AllgetResponsePages() {
	return (_errorPages);
}

std::string &ServerConfigs::getResponsePage(int errorCode) {
	// for (auto xs : _errorPages)
	// 	std::cout << xs.first << " " << xs.second << std::endl;
	return (_errorPages[errorCode]);
}

std::vector<Location> &ServerConfigs::getLocations() {
	return (_locations);
}

void ServerConfigs::setListen(std::string listen) {
	_listen = listen;
	size_t i = 0;
	std::string tmpHost, tmpPort;
	while (i < _listen.length() && _listen[i] != ':')
		tmpHost += _listen[i++];
	i++;
	while (i < _listen.length())
		tmpPort += _listen[i++];
	setHostAndPort(tmpHost, tmpPort);
}

void ServerConfigs::setHostAndPort(std::string host, std::string port) {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	struct addrinfo *peer_address;
	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		exit (FAILURE);
	}

	char buffHost[100];
	char buffServ[100];
	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
				buffHost, sizeof(buffHost),
				buffServ, sizeof(buffServ),
				NI_NUMERICHOST | NI_NUMERICSERV);
	_host = buffHost;
	_serv = buffServ;
	freeaddrinfo(peer_address);
}

void ServerConfigs::setServerName(std::string serverName) {
	_serverName = serverName;
}

void ServerConfigs::setMaxClientBodySize(unsigned long long int maxClientBodySize) {
	_maxClientBodySize = maxClientBodySize;
}

void ServerConfigs::setErrorPages(std::map<int, std::string> errorPages) {
	_errorPages = errorPages;
}

void ServerConfigs::setLocations(std::vector<Location> locations) {
	_locations = locations;
}

bool ServerConfigs::isValidConfigs() const {
	if (_host.empty())
		return (false);
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (!(_locations[i].isLocationValid()))
			return (false);
	}
	return (true);
}

void ServerConfigs::addErrorPage(int errorCode, std::string path) {
	// std::ifstream file;
	// file.open(path);
	// if (file)
	// {
	_errorPages[errorCode] = path;
	// 	file.close();
	// }
}

Location &ServerConfigs::getBestMatchedLocation(std::string &path) {
	size_t idxBestLocation = 0;
	size_t maxLenMatched = 1;
	for (size_t i = _locations.size() - 1 ; i > 0; i--)
	{
		if (_locations[i].isRouteMatch(path))
		{
			if (_locations[i].getRoute().length() >= maxLenMatched)
			{
				maxLenMatched = _locations[i].getRoute().length();
				idxBestLocation = i;
			}
		}
	}
	return (_locations[idxBestLocation]);
}

void ServerConfigs::size_limit_in() {
	size_limit_set = true;
}

bool ServerConfigs::size_limit_found() const {
	return size_limit_set;
}
