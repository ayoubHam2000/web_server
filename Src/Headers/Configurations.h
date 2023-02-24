/***************************************************/
/*     created by TheWebServerTeam 2/17/23         */
/***************************************************/

#ifndef WEB_SERVER_CONFIGURATIONS_H
#define WEB_SERVER_CONFIGURATIONS_H

#include "SyntaxTree.h"
#include "ServerConfig.h"

class Configurations
{
private:
	enum {
		ERROR, SUCCESS, BLANK, START
	};

private:
	SyntaxTree _syntaxTree;
	SyntaxTree *_syntaxTreePtr;
	int _state;
	char *_buffer;
	char *_bufferOrigin;
	unsigned int _nbLine;
	unsigned int _counter;
	unsigned int _indexBeginLine;

private:
	Configurations(const Configurations&);
	Configurations& operator=(const Configurations&);

public:

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
				//std::cout << "Success" << std::endl;
			}else{
				throw std::runtime_error(std::string("ParseError ") + confPath);
			}
		}else{
			throw std::runtime_error(std::string("Can't open ") + confPath);
		}
	}

	~Configurations(){};

public:

	void	getConfigurations(std::vector<ServerConfig> &res){
		std::vector<SyntaxTree*> list = _syntaxTree.list;
		for (std::vector<SyntaxTree*>::iterator iter = list.begin(); iter != list.end(); ++iter){
			ServerConfig serverConf;
			fillServerConf(serverConf, *iter);
			res.push_back(serverConf);
		}
	}

	//TODO: ToRemove
	void display(){
		std::vector<ServerConfig> conf;
		getConfigurations(conf);
		for (std::vector<ServerConfig>::iterator iter = conf.begin(); iter != conf.end(); ++iter){
			iter->display();
		}
	}

private:

/*****************************************************************/
// Utils
/*****************************************************************/

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

int _parseToken(const std::string &token){
	_syntaxTreePtr = _syntaxTreePtr->add(token);
	return (SUCCESS);
}


/*****************************************************************/
// Rules
/*****************************************************************/
/*****************************************************************/
/*
START -> _BLANK.START  | "server".Server | EOF
Server -> _BLANK.Server | "{".ServerToken
ServerToken -> _BLANK.ServerToken | _END_BRACE.START | _ServerListToken._Token.ServerToken | "location"._Token.Location
Location -> _BLANK.Location | "{".LocationToken
LocationToken -> _BLANK.LocationToken | _END_BRACE.ServerToken | _LocationListToken._Token.LocationToken
_ServerListToken -> "listen"|"server_name"|"max_client_body_size"|"error_page"
_LocationListToken -> "allow_methods"|"redirect"|"auto_index"|"root"|"index"|"upload_pass"|"cgi_pass"
_Token -> [\t|\v|\r|\f]+.[^\n{}]*
_END_BRACE -> "}"
_BLANK -> \n|\t|\v|\r|\f
*/
/*****************************************************************/

int start(){
	if (_blank() == SUCCESS){
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
	if (_blank() == SUCCESS){
		return (server());
	} else if (!strncmp(_buffer, "{", 1)){
		_advance(1);
		return (serverToken());
	}
	return (ERROR);
}

int serverToken(){
	if (_blank() == SUCCESS){
		return (serverToken());
	} else if (_endBrace() == SUCCESS){
		_syntaxTreePtr = _syntaxTreePtr->getParent();
		return (start());
	} else if (_serverListToken() == SUCCESS && _token() == SUCCESS){
		_syntaxTreePtr = _syntaxTreePtr->getParent()->getParent();
		return (serverToken());
	} else if (!strncmp(_buffer, "location", 8)){
		_syntaxTreePtr = _syntaxTreePtr->add("location");
		_advance(8);
		if (_token() == SUCCESS){
			_syntaxTreePtr = _syntaxTreePtr->getParent();
			_syntaxTreePtr = _syntaxTreePtr->add("locationBody");
			return (location());
		}
	}
	return (ERROR);
}

int location(){
	if (_blank() == SUCCESS){
		return (location());
	} else if (!strncmp(_buffer, "{", 1)){
		_advance(1);
		return (locationToken());
	}
	return (ERROR);
}

int locationToken(){
	if (_blank() == SUCCESS){
		return (locationToken());
	} else if (_endBrace() == SUCCESS){
		_syntaxTreePtr = _syntaxTreePtr->getParent()->getParent();
		return (serverToken());
	} else if (_locationListToken() == SUCCESS && _token() == SUCCESS){
		_syntaxTreePtr = _syntaxTreePtr->getParent()->getParent();
		return (locationToken());
	}
	return (ERROR);
}

//-----------------------------------------------

int _blank(){
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

int _endBrace(){
	if (*_buffer == '}'){
		_advance(1);
		return (SUCCESS);
	}
	return (ERROR);
}

int _token(){
	char *p;
	char *end;

	if (*_buffer != '\n' && _blank() == SUCCESS){
		while (*_buffer != '\n' && _blank() == SUCCESS)
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
			return (_parseToken(std::string(p, end + 1)));
		}
	}
	return (ERROR);
}

int _locationListToken(){
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

int _serverListToken(){
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

//-----------------------------------------------

int parseFile(char *buffer){
	int s = start();
	if (s != SUCCESS)
		handleError();
	return (s);
}

void handleError(){
	int lineIndex = _counter - _indexBeginLine;
	std::cout << "Error In " << _nbLine + 1 << ":" << lineIndex <<std::endl;
	char *p = _bufferOrigin + _indexBeginLine + 1;
	while (*p && *p != '\n')
		p++;
	std::cout << std::string(_bufferOrigin + _indexBeginLine + 1, p) << std::endl;
	if (lineIndex > 0)
		lineIndex = lineIndex - 1;
	std::cout << std::string(lineIndex, ' ') << "^" << std::endl;
}

int parser(std::ifstream &file){
	unsigned int	maxFileSize = 1024*1024*1024;
	int				status;

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

/*****************************************************************/
// Fill
/*****************************************************************/

void fillLocation(LocationConfig& location, SyntaxTree *tree){
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
			LocationConfig location;
			fillLocation(location, (*iter));
			serverConfig.addLocation(value, location);
		}
	}
}

};//END CLASS


#endif //WEB_SERVER_CONFIGURATIONS_H
