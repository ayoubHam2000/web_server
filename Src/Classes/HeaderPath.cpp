#include "HeaderPath.hpp"

HeaderPath::HeaderPath() {}

HeaderPath::HeaderPath(const HeaderPath &other) :
		_path(other._path),
		_params(other._params)
{}

HeaderPath &HeaderPath::operator=(const HeaderPath &other) {
	_path = other._path;
	_params = other._params;
	return *this;
}

HeaderPath::~HeaderPath() {}

const std::string &HeaderPath::getPath() const {
	return _path;
}

const std::string &HeaderPath::getParams() const {
	return _params;
}

bool HeaderPath::validatePath(const char *path) {
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

std::string HeaderPath::decodePath(const char *path) {
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

bool HeaderPath::parse(const std::string &path) {
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
