/**************************************************/
/*     created by TheWebServerTeam 2/9/23         */
/**************************************************/

#include "WebServer.h"
#include <fstream>
#include <exception>
#include <stack>
#include <sstream>
#include <algorithm>
#include "ChunckContentHandler.hpp"
#include "FileSystem.hpp"

class Location
{
private:
	std::string							_location;
	std::map<std::string, bool> 		_allowedMethods; //Method => isAllowed
	std::string							_redirection;
	bool								_autoIndex;
	std::string							_rootFolder;
	std::vector<std::string>			_indexFiles;
	std::string							_uploadFolder;
	std::map<std::string, std::string>	_cgi; //extension => cgi_path

public:

	Location() :
		_location(),
		_allowedMethods(),
		_redirection(),
		_autoIndex(false),
		_rootFolder(),
		_indexFiles(),
		_uploadFolder(),
		_cgi()
	{
		_allowedMethods["GET"] = false;
		_allowedMethods["DELETE"] = false;
		_allowedMethods["POST"] = false;
	}

	~Location(){}

	Location(const Location& other){
		*this = other;
	}

	Location& operator=(const Location& other){
		_location = other._location;
		_allowedMethods = other._allowedMethods;
		_redirection = other._redirection;
		_autoIndex = other._autoIndex;
		_rootFolder = other._rootFolder;
		_indexFiles = other._indexFiles;
		_uploadFolder = other._uploadFolder;
		_cgi = other._cgi;
		return (*this);
	}

public:
	void setLocation(const std::string &value) {
		_location = value;
	}

	void setAllowedMethods(const std::string &value) {
		std::istringstream iss(value);
		std::string method;
		while (std::getline(iss, method, ' ')){
			std::transform(method.begin(), method.end(), method.begin(), toupper);
			_allowedMethods[method] = true;
			if (_allowedMethods.size() != 3)
				throw std::runtime_error("Method Not Supported");
		}
	}

	void setRedirection(const std::string &value) {
		_redirection = value;
	}

	void setAutoIndex(const std::string &value) {
		std::string autoIndex = value;
		std::transform(autoIndex.begin(), autoIndex.end(), autoIndex.begin(), tolower);
		if (autoIndex != "on" && autoIndex != "of")
			throw std::runtime_error("Not valid autoIndex");
		_autoIndex = false;
		if (autoIndex == "on")
			_autoIndex = true;
	}

	void setRootFolder(const std::string &value) {
		_rootFolder = value;
	}

	void setIndexFiles(const std::string &value) {
		std::istringstream iss(value);
		std::string file;
		while (std::getline(iss, file, ' ')){
			_indexFiles.push_back(file);
		}
	}

	void setUploadFolder(const std::string &value) {
		_uploadFolder = value;
	}

	void addCgi(const std::string &value) {
		std::string::size_type pos = value.find(" ");
		std::string extension = value.substr(0, pos);
		std::string cgi = value.substr(pos + 1);
		_cgi[extension] = cgi;
	}

public:

	void display(){
		std::cout << "Location: " << _location << std::endl;
		std::cout << "AllowedMethods GET: " << _allowedMethods["GET"];
		std::cout << ", POST: " << _allowedMethods["POST"];
		std::cout << ", DELETE: " << _allowedMethods["DELETE"] << std::endl;
		std::cout << "Redirection: " << _redirection << std::endl;
		std::cout << "AutoIndex: " << _autoIndex << std::endl;
		std::cout << "RootFolder: " << _rootFolder << std::endl;
		std::cout << "IndexFiles: ";
		for (std::vector<std::string>::iterator iter = _indexFiles.begin(); iter != _indexFiles.end(); iter++){
			std::cout << *iter << ", ";
		}
		std::cout << std::endl;
		std::cout << "UploadFolder: " << _uploadFolder << std::endl;
		std::cout << "CGI: " << std::endl;
		for (std::map<std::string, std::string>::iterator iter = _cgi.begin(); iter != _cgi.end(); iter++){
			std::cout << "\t" << (*iter).first << "=>" << (*iter).second << std::endl;
		}
	}

};

struct ServerConfig
{
public:
	std::string						_host;
	std::string						_service;
	std::string						_server_name;
	unsigned long					_maxClientBodySize;
	std::map<int, std::string> 		_errorPages; //errorStatus => pagePath
	std::map<std::string, Location>	_locations; //locationPath => Location class

public:

	ServerConfig(): _host(),
					_service("80"),
					_server_name(),
					_maxClientBodySize(42949672),
					_errorPages(),
					_locations()
	{
		_errorPages[404] = "";
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

	void setListen(const std::string &value){
		if (!_parseListenIpAddressPort(value))
			throw std::runtime_error("Invalid listen value");
		std::string::size_type pos = value.find(':');
		_host = value.substr(0, pos);
		_service = value.substr(pos + 1);
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
		std::string page = value.substr(pos);
		_errorPages[errorStatus] = page;
	}

	void addLocation(const std::string &locationPath, const Location &location){
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
		for (std::map<std::string, Location>::iterator iter = _locations.begin(); iter != _locations.end(); iter++){
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
		std::istringstream iss(str);
		std::string b;

		try{
			int counter = 0;
			int i = 0;
			char c = '.';
			while (std::getline(iss, b,c)){
				if (_is_number(b) && b.size() <= 5){
					int nb = std::stoi(b);
					if (i <= 3 && nb >= 0 && nb <= 255){
						counter++;
					} else if (i <= 4 &&  nb >= 0 && nb <= 65535){
						counter++;
					}
				}
				if (i == 2)
					c = ':';
				i++;
			}
			return (counter == 5);
		}catch (const std::exception &e){
			return (false);
		}
	}

	bool _parseServerName(const std::string &str){
		//[_a-Z0-9]*
		for (std::string::const_iterator iter = str.begin(); iter != str.end(); ++iter){
			if (!std::isalnum(*iter) && *iter != '_')
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
		while (index < str.size() && str[index] == ' ')
			index++;
		if (index != str.size())
			return (true);
		return (false);
	}

};

class SyntaxTree{
public:
	std::string token;
	SyntaxTree* parent;
	std::vector<SyntaxTree*> list;

public:

	SyntaxTree(const std::string &token = "", SyntaxTree *parent = NULL): token(token), parent(parent), list(){}

	SyntaxTree *add(const std::string &t){
		SyntaxTree *newItem = new SyntaxTree(t, this);
		list.push_back(newItem);
		return (newItem);
	}

	SyntaxTree *getParent(){
		return (parent);
	}

	std::string &getToken(){
		return (token);
	}

	void display(int level = 0){
		std::cout << token << std::endl;
		for (std::vector<SyntaxTree*>::iterator iter = list.begin(); iter != list.end(); ++iter){
			int i = level;
			while (i-- >= 0)
				std::cout << "\t";
			(*iter)->display(level + 1);
		}
	}

	static void test(){
		SyntaxTree a = SyntaxTree("server");
		SyntaxTree *listen = a.add("listen");
		listen->add("localhost:3031");

		SyntaxTree *server_name = a.add("server_name");
		server_name->add("server_2");

		SyntaxTree *max_client_body_size = a.add("max_client_body_size");
		max_client_body_size->add("42949672");

		SyntaxTree *location = a.add("location");
		location->add("/planet");
		SyntaxTree *redirect = location->add("redirect");
		redirect->add("https://www.facebook.com/");

		a.display();
	}

};

class Configurations
{
private:
	SyntaxTree _syntaxTree;
	SyntaxTree *_syntaxTreePtr;
	int _state;
	char *_buffer;
	char *_bufferOrigin;
	unsigned int _nbLine;
	unsigned int _counter;
	unsigned int _indexBeginLine;
public:

	enum {
		ERROR, SUCCESS, BLANK, START
	};

	Configurations(const std::string &confPath)
	{
		_buffer = NULL;
		_bufferOrigin = NULL;
		_nbLine = 0;
		_counter = 0;
		_indexBeginLine = 0;
		_state = START;
		_syntaxTreePtr = &_syntaxTree;
		std::ifstream confFile(confPath);
		if (confFile.is_open()){
			if (parser(confFile) == SUCCESS){
				std::cout << "Success" << std::endl;
			}else{
				throw std::runtime_error(std::string("ParseError ") + confPath);
			}
		}else{
			throw std::runtime_error(std::string("Can't open ") + confPath);
		}
	}

	void handleError(int errorType){
		int lineIndex = _counter - _indexBeginLine;
		std::cout << "Error In " << _nbLine + 1 << ":" << lineIndex <<std::endl;
		char *p = _bufferOrigin + _indexBeginLine + 1;
		while (*p && *p != '\n')
			p++;
		std::cout << std::string(_bufferOrigin + _indexBeginLine + 1, p) << std::endl;
		std::cout << std::string(lineIndex - 1, ' ') << "^" << std::endl;
	}


	int parser(std::ifstream &file){
		unsigned int maxFileSize = 1024*1024*1024;
		int status;

		file.seekg(0, file.end);
		unsigned int length = file.tellg();
		if (length > maxFileSize){
			throw std::runtime_error("File is too big");
		}
		file.seekg(0, file.beg);
		_buffer = new char[length + 1];
		_bufferOrigin = _buffer;
		file.read(_buffer, length);
		_buffer[length] = 0;
		status = parseFile(_buffer);
		return (status);
	}

	int _isIn(char c, const char *set){
		if (strchr(set, c))
			return (true);
		return (false);
	}

	int _advance(int i){
		if (*_buffer == 0 && i > 0){
			throw std::runtime_error("Advance used when *_buffer = 0");
		}
		while (i--){
			_counter++;
			if (*_buffer == '\n'){
				_indexBeginLine = _buffer - _bufferOrigin;
				_nbLine++;
			}
			_buffer++;
		}
		return (0);
	}

	int blank(){
		const char *p = " \n\t\v\r\f\0";
		int i = 0;
		while (p[i]){
			if (*_buffer == p[i]){
				_advance(1);
				return (SUCCESS);
			}
			i++;
		}
		return (ERROR);
	}

	int endBrace(){
		if (*_buffer == '}'){
			_advance(1);
			return (SUCCESS);
		}
		return (ERROR);
	}



	int parseToken(const std::string &token){
		std::cout << '[' << token << ']' << std::endl;
		_syntaxTreePtr = _syntaxTreePtr->add(token);
		return (SUCCESS);
	}

	int token(){
		char *p;
		char *end;

		if (*_buffer != '\n' && blank() == SUCCESS){
			while (*_buffer != '\n' && blank() == SUCCESS)
				;
			if (_isIn(*_buffer, "\n{}"))
				return (ERROR);
			p = _buffer;
			end = _buffer;
			while (!_isIn(*_buffer, "\n{}") && *_buffer){
				if (!_isIn(*_buffer, " \t\v\r\f"))
					end = _buffer;
				_advance(1);
			}
			if (_isIn(*_buffer, "\n{}")){
				return (parseToken(std::string(p, end + 1)));
			}
		}
		return (ERROR);
	}

	int locationListToken(){
		const char *arr[] = {"allow_methods\0",
					   "redirect\0",
					   "auto_index\0",
					   "root\0",
					   "index\0",
					   "upload_pass\0",
					   "cgi_pass\0",
					   NULL};
		const char **p = arr;
		while (*p){
			if (!strncmp(_buffer, *p, strlen(*p))){
				_syntaxTreePtr = _syntaxTreePtr->add(*p);
				_advance(strlen(*p));
				return (SUCCESS);
			}
			p++;
		}
		return (ERROR);
	}

	int serverListToken(){
		const char *arr[] = {"listen\0","server_name\0","max_client_body_size\0","error_page\0", NULL};
		const char **p = arr;
		while (*p){
			if (!strncmp(_buffer, *p, strlen(*p))){
				_syntaxTreePtr = _syntaxTreePtr->add(*p);
				_advance(strlen(*p));
				return (SUCCESS);
			}
			p++;
		}
		return (ERROR);
	}

	int start(){
		if (blank() == SUCCESS){
			return (start());
		}
		else if (!strncmp(_buffer, "server", 6)){
			_syntaxTreePtr = _syntaxTreePtr->add("server");
			_advance(6);
			return (server());
		}
		else if (*_buffer == 0)
			return (SUCCESS);
		return (ERROR);
	}

	int server(){
		if (blank() == SUCCESS){
			return (server());
		} else if (!strncmp(_buffer, "{", 1)){
			_advance(1);
			return (serverToken());
		}
		return (ERROR);
	}

	int serverToken(){
		if (blank() == SUCCESS){
			return (serverToken());
		} else if (endBrace() == SUCCESS){
			_syntaxTreePtr = _syntaxTreePtr->getParent();
			return (start());
		} else if (serverListToken() == SUCCESS && token() == SUCCESS){
			_syntaxTreePtr = _syntaxTreePtr->getParent()->getParent();
			return (serverToken());
		} else if (!strncmp(_buffer, "location", 8)){
			_syntaxTreePtr = _syntaxTreePtr->add("location");
			_advance(8);
			if (token() == SUCCESS){
				_syntaxTreePtr = _syntaxTreePtr->getParent();
				_syntaxTreePtr = _syntaxTreePtr->add("locationBody");
				return (location());
			}
		}
		return (ERROR);
	}

	int location(){
		if (blank() == SUCCESS){
			return (location());
		} else if (!strncmp(_buffer, "{", 1)){
			_advance(1);
			return (locationToken());
		}
		return (ERROR);
	}

	int locationToken(){
		if (blank() == SUCCESS){
			return (locationToken());
		} else if (endBrace() == SUCCESS){
			_syntaxTreePtr = _syntaxTreePtr->getParent()->getParent();
			return (serverToken());
		} else if (locationListToken() == SUCCESS && token() == SUCCESS){
			_syntaxTreePtr = _syntaxTreePtr->getParent()->getParent();
			return (locationToken());
		}
		return (ERROR);
	}


	void fillLocation(Location& location, SyntaxTree *tree){
		location.setLocation(tree->list[0]->token);
		std::vector<SyntaxTree*> list = tree->list[1]->list;
		for (std::vector<SyntaxTree*>::iterator iter = list.begin(); iter != list.end(); ++iter){
			if ((*iter)->token == "allow_methods"){
				std::string &value = (*iter)->list[0]->token;
				location.setAllowedMethods(value);
			} else if ((*iter)->token == "redirect"){
				std::string &value = (*iter)->list[0]->token;
				location.setRedirection(value);
			} else if ((*iter)->token == "auto_index"){
				std::string &value = (*iter)->list[0]->token;
				location.setAutoIndex(value);
			} else if ((*iter)->token == "root"){
				std::string &value = (*iter)->list[0]->token;
				location.setRootFolder(value);
			} else if ((*iter)->token == "index"){
				std::string &value = (*iter)->list[0]->token;
				location.setIndexFiles(value);
			} else if ((*iter)->token == "upload_pass"){
				std::string &value = (*iter)->list[0]->token;
				location.setUploadFolder(value);
			} else if ((*iter)->token == "cgi_pass"){
				std::string &value = (*iter)->list[0]->token;
				location.addCgi(value);
			}
		}
	}

	void fillServerConf(ServerConfig& serverConfig, SyntaxTree *tree){
		std::vector<SyntaxTree*> list = tree->list;
		for (std::vector<SyntaxTree*>::iterator iter = list.begin(); iter != list.end(); ++iter){
			if ((*iter)->token == "listen"){
				std::string &value = (*iter)->list[0]->token;
				serverConfig.setListen(value);
			} else if ((*iter)->token == "server_name"){
				std::string &value = (*iter)->list[0]->token;
				serverConfig.setServerName(value);
			} else if ((*iter)->token == "max_client_body_size"){
				std::string &value = (*iter)->list[0]->token;
				serverConfig.setMaxClientBodySize(value);
			} else if ((*iter)->token == "error_page"){
				std::string &value = (*iter)->list[0]->token;
				serverConfig.addErrorPage(value);
			} else if ((*iter)->token == "location"){
				std::string &value = (*iter)->list[0]->token;
				Location location;
				fillLocation(location, (*iter));
				serverConfig.addLocation(value, location);
			}
		}
	}

	std::vector<ServerConfig> getConfigurations(){
		std::vector<ServerConfig> res;

		std::vector<SyntaxTree*> list = _syntaxTree.list;
		for (std::vector<SyntaxTree*>::iterator iter = list.begin(); iter != list.end(); ++iter){
			ServerConfig serverConf;
			fillServerConf(serverConf, *list.begin());
			res.push_back(serverConf);
		}
		return (res);
	}

	int parseFile(char *buffer){
		int s = start();
		if (s != SUCCESS)
			handleError(s);
		else{
			//_syntaxTree.display();
		}
		return (s);
	}



};


void start(const std::string &confPath){
	/*Configurations configurations = Configurations(confPath);
	std::vector<ServerConfig> conf = configurations.getConfigurations();
	for (std::vector<ServerConfig>::iterator iter = conf.begin(); iter != conf.end(); ++iter){
		//iter->display();
	}*/

	ChunkContentHandler::testFunction(confPath);
	/*try{
		FileSystem f("../testFolder");
		f.list_directory();
	}catch (std::exception& e){
		std::cout << e.what() << std::endl;
	}*/


}

int	main(int ac, char **av)
{
	if (ac != 2){
		std::cout << "Path to the configuration file is missing." << std::endl;
		return (1);
	}
	try {
		start(av[1]);
	} catch(const std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return (0);
}