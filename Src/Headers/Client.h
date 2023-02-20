/***************************************************/
/*     created by TheWebServerTeam 2/17/23         */
/***************************************************/

#ifndef WEB_SERVER_CLIENT_H
#define WEB_SERVER_CLIENT_H

# include <Libraries.h>
# include <ServerConfig.h>
# include <Header.h>
# include <BodyChunk.h>
# include <Response.h>

class Client {
public:
	enum {
		NONE,
		BAD_REQUEST, NOT_FOUND, METHOD_NOT_ALLOWED
		, NOT_ACCEPTED, DONE, SUCCESS, FAILED, WAITING_REQ, SENDING_RES
	};
public:
	const static size_type MAX_REQUEST_SIZE = 8192;
	static size_type 		nbClients;
private:
	socketType 					_socket;
	const socketType			_remoteSocket;
	std::vector<ServerConfig*>	*_socketConfigs;
	char						requestHeader[MAX_REQUEST_SIZE];
	socklen_t					address_length;
	struct sockaddr_storage		address;
	time_t 						lastResponse;
	int 						errorFlag;
	int 						_phase; //receiving, sending
	bool 						_isHeaderComplete;
	bool 						_isBodyComplete;
	size_type 					_counter;
	Header						_header;
	const ServerConfig*			_serverConf;
	const LocationConfig*		_locationConf;
	BodyChunk					_bodyChunk;
	Response					_response;

public:
	Client(const socketType serverSocket, std::vector<ServerConfig*> *socketConfigs) :
			_remoteSocket(),
			_socketConfigs(socketConfigs),
			errorFlag(NONE),
			lastResponse(get_time_ms()),
			address_length(sizeof(address)),
			_phase(WAITING_REQ),
			_isHeaderComplete(false),
			_isBodyComplete(false),
			_counter(0),
			_header(),
			_bodyChunk(),
			_response()
	{
		_socket = accept(_remoteSocket, (struct sockaddr*)(&address), &address_length);
		if (_socket == -1){
			errorFlag = NOT_ACCEPTED;
		}
		nbClients++;
	}
	~Client(){
		nbClients--;
	};
public:
	const socketType getSocket() const {
		return _socket;
	}

	const socketType getServerSocket() const {
		return _remoteSocket;
	}

	int getErrorFlag() const {
		return errorFlag;
	}

	time_t getLastResponse() const {
		return lastResponse;
	}

	int getPhase() const {
		return _phase;
	}


	//------

	void setLastResponse() {
		Client::lastResponse = get_time_ms();
	}

public:
	//TODO in serverConfig not server hostname duplicate
	//TODO if not error page found in a location get a default one for bad_req and page not found
	//TODO config if not protocol chose 80
	//TODO if redirection redirect automatically without check (ask for GET POST)
	//TODO other_proj check client disconnect
	//TOD case of reading a chunk containing the header and the body
	//TODO if location has POST is must have (default folder to upload or upload folder set in the config)
	//TODO Header Accept
	//content length not begger then size_type
	//comments
	//Internal Server Error
	// /1/
	// /1/
	int	nbLocationMatch(const std::string &locServer, const std::string &locClient){
		std::string::size_type locServerSize = locServer.size();
		if (locServerSize > locClient.size())
			return (-1);
		std::string::size_type idx = locClient.find(locServer);
		if (idx == 0){
			if (locServer.size() == locClient.size() || locServer[locServerSize - 1] == '/')
				return (locServerSize);
		}
		return (-1);
	}

	const ServerConfig *getServerConfigForLocation(){
		const std::string* host = NULL;
		if (_header.has("HOST"))
			host = &_header.valueOf("HOST");
		if (!host){
			return ((*_socketConfigs)[0]);
		} else {
			for (std::vector<ServerConfig*>::iterator iter = _socketConfigs->begin(); iter != _socketConfigs->end(); ++iter){
				if ((*iter)->getHost() == *host)
					return (*iter);
			}
		}
		return (NULL);
	}

	const LocationConfig *getLocation(const ServerConfig &serverConfig, const std::string &path){
		const LocationConfig *res = NULL;
		int max = 0;
		for (std::map<std::string, LocationConfig>::const_iterator iter = serverConfig.getLocations().cend(); iter != serverConfig.getLocations().end(); ++iter){
			int nbMatch = nbLocationMatch(iter->first, path);
			if (nbMatch > max){
				max = nbMatch;
				res = &iter->second;
			}
		}
		return (res);
	}

	bool	checkHeaderAndSet(){
		//check header
		if (_header.parse(std::string(requestHeader, _counter)) == false){
			errorFlag = BAD_REQUEST;
			return (false);
		}

		//set
		if (_header.getRequestType() == "GET")
			_isBodyComplete = true;

		//server config
		_serverConf = getServerConfigForLocation();
		if (!_serverConf){
			errorFlag = BAD_REQUEST; //no host name;
			return (false);
		}
		_locationConf = getLocation(*_serverConf, _header.getPath().getPath());
		if (!_locationConf){
			errorFlag = NOT_FOUND;
			return (false);
		}
		if (_locationConf->getAllowedMethods().at(_header.getRequestType()) == false){
			errorFlag = METHOD_NOT_ALLOWED;
			return (false);
		}
		return (true);
	}

	void	constructHeader(char *chunk, size_type chunkSize, size_type &nbRead){
		nbRead = std::min(MAX_REQUEST_SIZE - _counter, chunkSize);
		std::memcpy(requestHeader + _counter, chunk, nbRead);
		_counter += nbRead;
		if (std::strstr(requestHeader, "\r\n\r\n")){
			_isHeaderComplete = true;
		} else if (nbRead != chunkSize){
			errorFlag = BAD_REQUEST;
		}
	}

	void 	prepareToReceiveBody(size_type chunkSize){
		if (_isBodyComplete && chunkSize > 0)
			errorFlag = BAD_REQUEST;
		if (_isBodyComplete)
			return ;
		_bodyChunk.prepare(*_locationConf, *_serverConf, _header);
	}

	int	constructResponse(){
		_phase = SENDING_RES;
		if (errorFlag == BAD_REQUEST){
			_response.setErrorPage(Response::E400);
		}else if (errorFlag == NOT_FOUND){
			_response.setErrorPage(Response::E404);
		}
		_response.prepare(*_locationConf, *_serverConf, _header);
		return (SUCCESS);
	}

	//client send data
	void	handleChunk(char *chunk, size_type chunkSize){
		if (_isHeaderComplete == false){
			size_type nbRead = 0;
			constructHeader(chunk, chunkSize, nbRead);
			chunk += nbRead;
			chunkSize -= nbRead;
			if (_isHeaderComplete == true){
				if (errorFlag == NONE)
					checkHeaderAndSet();
				if (errorFlag == NONE)
					prepareToReceiveBody(chunkSize);
			}
		}
		if (!_isBodyComplete && _isHeaderComplete && errorFlag == NONE && chunkSize > 0){
			_bodyChunk.receive(chunk, chunkSize);
			if (_bodyChunk.isIsDone())
				_isBodyComplete = true;
		}
		if (errorFlag != NONE || _isBodyComplete){
			constructResponse();
		}
	}

	//client want to read data
	int	sendResponse(){
		try{
			_response.sendData(_socket);
			if (_response.isIsComplete())
				return (DONE);
		}catch (...){
			return (FAILED);
		}
		return (SUCCESS);
	}


};

(ls && pwd) > (pwd && ls)

#endif //WEB_SERVER_CLIENT_H
