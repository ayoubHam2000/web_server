/***************************************************/
/*     created by TheWebServerTeam 2/24/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_CLIENTINFO_H
#define DUMMY_CLIENT_CLIENTINFO_H

# include <Libraries.h>
# include <ServerConfig.h>
# include <Header.h>

class ClientInfo {
private:
	const Header&				_header;
	const ServerConfig&			_serverConf;
	const LocationConfig&		_locationConf;
	size_t 						_contentLength;
	std::string 				_service;
	std::string 				_ipAddress;
private:
	ClientInfo();
	ClientInfo(const ClientInfo&);
	ClientInfo& operator=(const ClientInfo&);

public:
	ClientInfo(const Header &header, const ServerConfig &serverConf, const LocationConfig &locationConf) :
		_header(header),
		_serverConf(serverConf),
		_locationConf(locationConf),
		_contentLength(0),
		_service(),
		_ipAddress()
		{}

	~ClientInfo();

public:

	const Header &getHeader() const {
		return _header;
	}

	const ServerConfig &getServerConf() const {
		return _serverConf;
	}

	const LocationConfig &getLocationConf() const {
		return _locationConf;
	}

	size_t getContentLength() const {
		return _contentLength;
	}

	const std::string &getService() const {
		return _service;
	}

	const std::string &getIpAddress() const {
		return _ipAddress;
	}

	//Setters

	void setContentLength(size_t contentLength) {
		_contentLength = contentLength;
	}

	void setService(const std::string &service) {
		_service = service;
	}

	void setIpAddress(const std::string &ipAddress) {
		_ipAddress = ipAddress;
	}

public:



	bool isRequestCanHandledByCgi() const {
		std::string fileExtension(_header.getPath().getPath().c_str());
		if (_locationConf.getCgi().find(fileExtension) != _locationConf.getCgi().cend()){
			return (true);
		}
		return (false);
	}

	const std::string& getCGIPath() const {
		std::string fileExtension(_header.getPath().getPath().c_str());
		return _locationConf.getCgi().find(fileExtension)->second;
	}

	const std::string& getRequestedPath() const {
		return _header.getPath().getPath();
	}

	const std::string& getErrorPage(int status) const{
		return _serverConf.getErrorPages().at(status);
	}

	std::string getRequestedFile() const{
		return getRequestedPath().substr(_locationConf.getLocation().size());
	}

	std::string getReqFileRelativePath() const{
		return _locationConf.getRootFolder() + "/" + getRequestedFile();
	}

	std::string geReqFileFullPath() const{
		char cwd[1024];

		if (getcwd(cwd, sizeof(cwd)) != NULL){
			return std::string(cwd) + "/" + getReqFileRelativePath();
		} else {
			throw std::runtime_error("CWD Failed");
		}
	}

//#Static
public:

	static const char *fileExtension(const std::string& file){
		return std::strrchr(file.c_str(), '.');
	}

	static std::string fileMemType(const std::string& file){
		const char *ext = fileExtension(file);
		if (ext){
			return (getMIMEType(std::string(ext)));
		}
		return (getMIMEType(""));
	}
};


#endif //DUMMY_CLIENT_CLIENTINFO_H
