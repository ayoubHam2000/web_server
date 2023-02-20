/***************************************************/
/*     created by TheWebServerTeam 2/18/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_URL_H
#define DUMMY_CLIENT_URL_H

#include "Libraries.h"

class HeaderPath {
private:
	std::string	_path;
	std::string _params;
public:
	HeaderPath(){}

	HeaderPath(const HeaderPath& other) :
			_path(other._path),
			_params(other._params)
	{}

	HeaderPath& operator=(const HeaderPath& other) {
		_path = other._path;
		_params = other._params;
		return *this;
	}

	~HeaderPath(){}
public:

	const std::string &getPath() const {
		return _path;
	}

	const std::string &getParams() const {
		return _params;
	}

public:

	bool validatePath(const char *path){
		if (*path != '/')
			return (false);
		while (*path){
			if (*path == '%'){
				if (!isHexChar(*(path + 1)) || !isHexChar(*(path + 2)))
					return (false);
				path += 3;
			}else{
				path++;
			}
		}
		return (true);
	}

	std::string decodePath(const char *path){
		std::stringstream decodedPath;

		while (*path){
			if (*path == '%'){
				char hex[3];
				hex[0] = *(path + 1);
				hex[1] = *(path + 2);
				hex[2] = 0;
				decodedPath << char(std::stoi(hex, NULL, 16));
				path += 3;
			} else {
				decodedPath << *path;
				path++;
			}
		}
		return (decodedPath.str());
	}

	bool parse(const std::string &path){
		if (!validatePath(path.c_str()))
			return (false);
		std::string::size_type pos = path.find('?');
		_path = path.substr(0, pos);
		if (pos != std::string::npos)
			_params = path.substr(pos + 1);
		_path = decodePath(_path.c_str());
		_params = decodePath(_params.c_str());
		return true;
	}

};


#endif //DUMMY_CLIENT_URL_H
