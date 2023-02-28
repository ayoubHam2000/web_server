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
	const Header*				_header;
	const ServerConfig*			_serverConf;
	const LocationConfig*		_locationConf;
	size_t 						_contentLength;
	std::string 				_service;
	std::string 				_ipAddress;
	std::string 				_createdFile;
private:
	ClientInfo();
	ClientInfo(const ClientInfo&);
	ClientInfo& operator=(const ClientInfo&);

public:
	ClientInfo(const Header *header, const ServerConfig *serverConf, const LocationConfig *locationConf) :
		_header(header),
		_serverConf(serverConf),
		_locationConf(locationConf),
		_contentLength(0),
		_service(),
		_ipAddress()
		{}

	~ClientInfo(){};

public:

	bool isSet(){
		return (_serverConf != NULL && _locationConf != NULL);
	}

	bool isServerConfSet(){
		return _serverConf != NULL;
	}

	void setHeader(const Header *header) {
		_header = header;
	}

	void setServerConf(const ServerConfig *serverConf) {
		_serverConf = serverConf;
	}

	void setLocationConf(const LocationConfig *locationConf) {
		_locationConf = locationConf;
	}

	const Header &getHeader() const {
		return *_header;
	}

	const ServerConfig &getServerConf() const {
		return *_serverConf;
	}

	const LocationConfig &getLocationConf() const {
		return *_locationConf;
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

	const std::string &getCreatedFile() const {
		return _createdFile;
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

	void setCreatedFile(const std::string &createdFile) {
		_createdFile = createdFile;
	}

public:



	bool isRequestCanHandledByCgi() const {
		const char *ptr = FileSystem::fileExtension(getHeader().getPath().getPath().c_str());
		if (ptr){
			std::string ext(ptr);
			if (getLocationConf().getCgi().find(ptr) != getLocationConf().getCgi().cend()){
				return (true);
			}
		}
		return (false);
	}

	const std::string& getCGIPath() const {
		std::string ext(FileSystem::fileExtension(getHeader().getPath().getPath().c_str()));
		return getLocationConf().getCgi().find(ext)->second;
	}

	const std::string& getRequestedPath() const {
		return getHeader().getPath().getPath();
	}

	std::string getRequestedFile() const{
		return getRequestedPath().substr(getLocationConf().getLocation().size());
	}

	std::string getReqFileRelativePath() const{
		return FileSystem::removeDotDot(getLocationConf().getRootFolder() + "/" + getRequestedFile());
	}

	std::string geReqFileFullPath() const{
		char cwd[1024];

		if (getcwd(cwd, sizeof(cwd)) != NULL){
			return FileSystem::removeDotDot(std::string(cwd) + "/" + getReqFileRelativePath());
		} else {
			throw std::runtime_error("CWD Failed");
		}
	}

};


#endif //DUMMY_CLIENT_CLIENTINFO_H
