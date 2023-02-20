/***************************************************/
/*     created by TheWebServerTeam 2/17/23         */
/***************************************************/

#ifndef WEB_SERVER_LOCATIONCONFIG_H
#define WEB_SERVER_LOCATIONCONFIG_H

#include "Libraries.h"

class LocationConfig
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

	LocationConfig() :
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

	~LocationConfig(){}

	LocationConfig(const LocationConfig& other){
		*this = other;
	}

	LocationConfig& operator=(const LocationConfig& other){
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

	const std::string &getLocation() const {
		return _location;
	}

	const std::map<std::string, bool> &getAllowedMethods() const {
		return _allowedMethods;
	}

	const std::string &getRedirection() const {
		return _redirection;
	}

	bool isAutoIndex() const {
		return _autoIndex;
	}

	const std::string &getRootFolder() const {
		return _rootFolder;
	}

	const std::vector<std::string> &getIndexFiles() const {
		return _indexFiles;
	}

	const std::string &getUploadFolder() const {
		return _uploadFolder;
	}

	const std::map<std::string, std::string> &getCgi() const {
		return _cgi;
	}

public:

/*****************************************************************/
// Set
/*****************************************************************/


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



#endif //WEB_SERVER_LOCATIONCONFIG_H
