/**************************************************/
/*     created by TheWebServerTeam 2/9/23         */
/**************************************************/

#include "WebServer.h"
#include <fstream>
#include <exception>
#include <stack>

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

};

class ServerConfig
{
private:
	std::string						_host;
	std::string						_service;
	unsigned int					_maxClientBodySize;
	std::map<int, std::string> 		_errorPages; //errorStatus => pagePath
	std::map<std::string, Location>	_locations; //location => Location class

public:



};



class Configurations
{
private:
	int _state;
	int	_body;
public:

	enum {
		ERROR, SUCCESS, T_SERVER, T_OPEN_BRACE, T_CLOSE_BRACE, B_SERVER, B_LOCATION, NONE
	};

	Configurations(const std::string &confPath)
	{
		_state = T_SERVER; //check for server header
		_body = NONE;
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

	int isIn(char c, const char *set){
		if (strchr(set, c))
			return (true);
		return (false);
	}

	int isBlank(char c){
		return isIn(c, "\t\v \f");
	}

	char *skipBlanks(char *str){
		while (*str && isBlank(*str))
			str++;
		return (str);
	}

	char *skipUntil(char *str, char *set){
		while (*str && !isIn(*str, set))
			str++;
		return (str);
	}

	int processServerToken(char *line){
		std::cout << "S: " << line << std::endl;
		return (SUCCESS);
	}

	int processLocationToken(char *line){
		std::cout << "L: " << line << std::endl;
		return (SUCCESS);
	}

	int parseLine(char *line){
		//std::cout << line << std::endl;
		line = skipBlanks(line);
		if (*line == 0)
			return (SUCCESS);
		if (_state == T_SERVER){
			if (strncmp(line, "server", 6))
				return (ERROR);
			line += 6;
			if (!isBlank(*line) && *line != 0)
				return (ERROR);
			line = skipBlanks(line);
			if (*line == '{'){
				_state = T_CLOSE_BRACE;
				_body = B_SERVER;
				line = skipBlanks(line);
				if (*line == 0)
					return (SUCCESS);
				else
					return (ERROR);
			}else if (*line == 0){
				_state = T_OPEN_BRACE;
				return (SUCCESS);
			}else
				return (ERROR);
		}
		else if (_state == T_OPEN_BRACE){
			if (*line == '{'){
				_state = B_SERVER;
				line = skipBlanks(line);
				if (*line == 0)
					return (SUCCESS);
				else
					return (ERROR);
			}else
				return (ERROR);
		}else if (_state == T_CLOSE_BRACE){
			if (*line == '}'){
				_state = T_SERVER;
				line = skipBlanks(line);
				if (*line == 0)
					return (SUCCESS);
				else
					return (ERROR);
			}else if (processServerToken(line) == ERROR)
				return (ERROR);
		}
	}

	int parser(std::ifstream &file){
		unsigned int maxFileSize = 1024*1024*1024;
		char *buffer;
		char *p;
		char *line;

		file.seekg(0, file.end);
		unsigned int length = file.tellg();
		if (length > maxFileSize){
			throw std::runtime_error("File is too big");
		}
		file.seekg(0, file.beg);
		buffer = new char[length + 1];
		file.read(buffer, length);
		buffer[length] = 0;

		line = buffer;
		while (true)
		{
			p = strchr(line, '\n');
			if (p){
				*p = 0;
				parseLine(line);
				line = p + 1;
			}else{
				parseLine(line);
				delete buffer;
				if (_state == T_SERVER)
					return (SUCCESS);
				else
					return (ERROR);
			}
		}
	}

};

void start(const std::string &confPath){
	Configurations configurations = Configurations(confPath);
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