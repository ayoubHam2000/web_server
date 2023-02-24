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
# include <MyBuffer.h>
# include <ClientInfo.h>

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
	MyBuffer					_requestHeader;
	socklen_t					address_length;
	struct sockaddr_storage		address;
	time_t 						lastResponse;
	int 						errorFlag;
	int 						_phase; //receiving, sending
	bool 						_isHeaderComplete;
	Header						_header;
	const ServerConfig*			_serverConf;
	const LocationConfig*		_locationConf;
	BodyChunk					_bodyChunk;
	Response					_response;
	ClientInfo					_clientInfo;

private:
	//Client(const Client &other);
	//Client& operator=(const Client &other);
public:
	Client(const socketType serverSocket, std::vector<ServerConfig*> *socketConfigs) :
			_remoteSocket(serverSocket),
			_socketConfigs(socketConfigs),
			errorFlag(NONE),
			lastResponse(get_time_ms()),
			address_length(sizeof(address)),
			_phase(WAITING_REQ),
			_isHeaderComplete(false),
			_header(),
			_bodyChunk(),
			_response(),
			_requestHeader(MAX_REQUEST_SIZE, "\r\n\r\n"),
			_serverConf(NULL),
			_locationConf(NULL),
			_clientInfo(_header, *_serverConf, *_locationConf)
	{
		_socket = accept(_remoteSocket, (struct sockaddr*)(&address), &address_length);
		if (_socket == -1){
			errorFlag = NOT_ACCEPTED;
		}

		char address_buffer[100];
		char service[100];
		int s = getnameinfo(
				(struct sockaddr*)&address, address_length,
				address_buffer, sizeof(address_buffer),
				service, sizeof(service),
				NI_NUMERICSERV);
		if (!s){
			_clientInfo.setIpAddress(address_buffer);
			_clientInfo.setService(service);
		}else{
			errorFlag = NOT_ACCEPTED;//TODO
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
	//TODO Server Error
	//TODO CGI
	//TODO CONFIG
	//Todo diff close and keep-alive
	//Todo content length not begger then size_type
	//Todo comments
	//Todo Internal Server Error
	//Todo Client OutPut Terminal
	//Todo autoIndex
	//Todo indexFile
	//Todo upload folder
	//Todo ../ ./
	//TODO delete
	//TODO change to clientInfo
	//TODO if listen in non numeric change it to nemeric
	//TODO setFullPathOfTheServer
	//TODO htons
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
		if (host){
			for (std::vector<ServerConfig*>::iterator iter = _socketConfigs->begin(); iter != _socketConfigs->end(); ++iter) {
				if ((*iter)->getHost() == *host)
					return (*iter);
			}
		}
		return ((*_socketConfigs)[0]);
	}

	const LocationConfig *getLocation(const ServerConfig &serverConfig, const std::string &path){
		const LocationConfig *res = NULL;
		int max = 0;
		for (std::map<std::string, LocationConfig>::const_iterator iter = serverConfig.getLocations().cbegin(); iter != serverConfig.getLocations().cend(); ++iter){
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
		if (_header.parse(std::string(_requestHeader.getBuffer(), _requestHeader.getSize())) == false){
			errorFlag = BAD_REQUEST;
			return (false);
		}

		//set
		if (_header.getRequestType() == "GET")
			_bodyChunk.setDone();
		if (_header.getRequestType() == "POST" && !_header.has("Content-Length") && !_header.has("Transfer-Encoding"))
			_bodyChunk.setDone();
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

	void 	prepareToReceiveBody(size_type chunkSize){
		if (_bodyChunk.isIsDone() && chunkSize > 0)
			errorFlag = BAD_REQUEST;
		if (_bodyChunk.isIsDone())
			return ;
		_bodyChunk.prepare(*_locationConf, *_serverConf, _header);
	}

	int	constructResponse(){
		_phase = SENDING_RES;
		_response.setResponseStatus(200);
		_clientInfo.setContentLength(_bodyChunk.getTheCgiContentLength());
		if (errorFlag == BAD_REQUEST){
			_response.setResponseStatus(400);
		}else if (errorFlag == NOT_FOUND){
			_response.setResponseStatus(404);
		}if (_bodyChunk.isIsDone() && _header.getRequestType() == "POST"){
			_response.setResponseStatus(201);
		}
		_response.prepare(&_clientInfo);
		return (SUCCESS);
	}

	//client send data
	void	handleChunk(char *chunk, size_type chunkSize){
		if (_isHeaderComplete == false){
			size_type nbRead = _requestHeader.add(chunk, chunkSize);
			if (_requestHeader.isFull() && !_requestHeader.isMatch())
				errorFlag = BAD_REQUEST;
			if (_requestHeader.isMatch())
				_isHeaderComplete = true;
			chunk += nbRead;
			chunkSize -= nbRead;
			if (_isHeaderComplete == true){
				if (errorFlag == NONE)
					checkHeaderAndSet();
				if (errorFlag == NONE)
					prepareToReceiveBody(chunkSize);
			}
		}
		if (!_bodyChunk.isIsDone() && _isHeaderComplete && errorFlag == NONE && chunkSize > 0){
			if (_bodyChunk.receive(chunk, chunkSize) == BodyChunk::ERROR)
				errorFlag = BAD_REQUEST;
		}
		if (errorFlag != NONE || _bodyChunk.isIsDone()){
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

#endif //WEB_SERVER_CLIENT_H
