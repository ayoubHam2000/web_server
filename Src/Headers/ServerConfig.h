/***************************************************/
/*     created by TheWebServerTeam 2/17/23         */
/***************************************************/

#ifndef WEB_SERVER_SERVERCONFIG_H
#define WEB_SERVER_SERVERCONFIG_H

#include "LocationConfig.h"

struct ServerConfig
{
private:
	std::string								_host;
	std::string								_service;
	std::string								_server_name;
	unsigned long							_maxClientBodySize;
	std::map<int, std::string> 				_errorPages; //errorStatus => pagePath
	std::map<std::string, LocationConfig>	_locations; //locationPath => Location class

public:

	ServerConfig(): _host(),
					_service("80"),
					_server_name(),
					_maxClientBodySize(ULONG_MAX),
					_errorPages(),
					_locations()
	{
		_errorPages[400] = "../static/error404.html";
		_errorPages[403] = "../static/error403.html";
		_errorPages[404] = "../static/error400.html";
		_errorPages[405] = "../static/error405.html";
		_errorPages[413] = "../static/error413.html";
	}

	ServerConfig(const ServerConfig& other){
		*this = other;
	}

	ServerConfig& operator=(const ServerConfig& other){
		_host = other._host;
		_service = other._service;
		_server_name = other._server_name;
		_maxClientBodySize = other._maxClientBodySize;
		_errorPages = other._errorPages;
		_locations = other._locations;
		return (*this);
	}

	~ServerConfig(){}

public:

/*****************************************************************/
// Get
/*****************************************************************/

std::string getListen(){
	return _host + ":" + _service;
}

const std::string &getHost() const {
	return _host;
}

const std::string &getService() const {
	return _service;
}

const std::string &getServerName() const {
	return _server_name;
}

unsigned long getMaxClientBodySize() const {
	return _maxClientBodySize;
}

const std::map<int, std::string> &getErrorPages() const {
	return _errorPages;
}

const std::map<std::string, LocationConfig> &getLocations() const {
	return _locations;
}

/*****************************************************************/
// Set
/*****************************************************************/

	void	setDefAndCheckResult(){
		if (_server_name.empty())
			_server_name = getListen();
		if (_locations.empty()){
			throw std::runtime_error("No location provided in the server config");
		}
	}

	void setListen(const std::string &value){
		if (!_parseListenIpAddressPort(value))
			throw std::runtime_error("Invalid listen value");
	}

	void setServerName(const std::string &value){
		if (!_parseServerName(value))
			throw std::runtime_error("Invalid server name");
		_server_name = value;
	}

	void setMaxClientBodySize(const std::string &value){
		if (!_parseMaxClientBodySize(value))
			throw std::runtime_error("Invalid MaxClientBodySize");
		_maxClientBodySize = std::stoul(value);
	}

	void addErrorPage(const std::string &value){
		if (!_parseErrorPage(value))
			throw std::runtime_error("Invalid error page");
		std::string::size_type pos = value.find(' ');
		int errorStatus = std::stoi(value.substr(0, pos));
		while (value[pos] == ' ')
			pos++;
		std::string page = FileSystem::removeDotDot(value.substr(pos));
		if (!FileSystem::file_exists(page.c_str()))
			throw std::runtime_error("error page does not exist");
		if (FileSystem::isDirectory(page.c_str()))
			throw std::runtime_error("error page is a directory");
		_errorPages[errorStatus] = page;
	}

	void addLocation(const std::string &locationPath, const LocationConfig &location){
		_locations[locationPath] = location;
	}

public:

	void display(){
		std::cout << "Host: " << _host << std::endl;
		std::cout << "Service: " << _service << std::endl;
		std::cout << "Server_name: " << _server_name << std::endl;
		std::cout << "MaxClientBodySize: " << _maxClientBodySize << std::endl;
		std::cout << "ErrorPages: " << std::endl;
		for (std::map<int, std::string>::iterator iter = _errorPages.begin(); iter != _errorPages.end(); iter++){
			std::cout << "\t" << (*iter).first << "=>" << (*iter).second << std::endl;
		}

		std::cout << "Locations: " << std::endl;
		for (std::map<std::string, LocationConfig>::iterator iter = _locations.begin(); iter != _locations.end(); iter++){
			std::cout << (*iter).first << std::endl;
			(*iter).second.display();
		}
	}

private:

	bool _is_number(const std::string& s)
	{
		//[1-9][0-9]*
		std::string::const_iterator it = s.begin();
		if (it != s.end() && std::isdigit(*it) && (*it != '0' || s.size() == 1)){
			++it;
			while (it != s.end() && std::isdigit(*it))
				++it;
		}
		return (!s.empty() && it == s.end());
	}

	bool	_parseListenIpAddressPort(const std::string &str){
		int				status;
		struct			addrinfo hints;
		struct addrinfo *bind_address;

		std::string::size_type pos = str.find(':');
		if (pos != std::string::npos){
			_host = str.substr(0, pos);
			_service = str.substr(pos + 1);
		} else {
			_host = str;
		}

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		status = getaddrinfo(_host.c_str(), _service.c_str(), &hints, &bind_address);
		if (status != 0) {
			return (false);
		}
		char address_buffer[100];
		char service[100];
		status = getnameinfo(
				bind_address->ai_addr, bind_address->ai_addrlen,
				address_buffer, sizeof(address_buffer),
				service, sizeof(service),
				NI_NUMERICSERV | NI_NUMERICHOST);
		if (!status){
			_host = address_buffer;
			_service = service;
		}else{
			return (false);
		}
		return (true);
	}

	bool _parseServerName(const std::string &str){
		//[-_a-Z0-9.:]*
		const std::string tokens("_-.:");
		for (std::string::const_iterator iter = str.begin(); iter != str.end(); ++iter){
			if (!std::isalnum(*iter) && tokens.find(*iter) == std::string::npos)
				return (false);
		}
		return (true);
	}

	bool _parseMaxClientBodySize(const std::string &str){
		if (_is_number(str) && str.size() <= 10){
			if (std::stoll(str) < 4294967296L)
				return (true);
		}
		return (false);
	}

	bool _parseErrorPage(const std::string& str){
		std::string::const_iterator pos = std::find(str.begin(), str.end(), ' ');
		if (pos == str.cend())
			return (false);
		std::string::size_type index = str.find(' ');
		std::string nb = str.substr(0, index);
		if (nb.size() <= 3){
			int n = std::stoi(nb);
			if (n >= 100 && n < 600)
				return (true);
			else
				return (false);
		}else
			return (false);
	}

};


#endif //WEB_SERVER_SERVERCONFIG_H
