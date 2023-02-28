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
		BAD_REQUEST, NOT_FOUND, METHOD_NOT_ALLOWED, HTTP_Version_Not_Supported
		, NOT_ACCEPTED, DONE, SUCCESS, FAILED, WAITING_REQ, SENDING_RES, BODY_TOO_BIG, FORBIDDEN
	};
public:
	static size_type 		nbClients;
private:
	socketType 					_socket;
	const socketType			_remoteSocket;
	std::vector<ServerConfig*>	_socketConfigs;
	socklen_t					address_length;
	struct sockaddr_storage		address;
	int 						errorFlag;
	int 						_phase; //receiving, sending
	Header						_header;
	const ServerConfig*			_serverConf;
	const LocationConfig*		_locationConf;
	BodyChunk					_bodyChunk;
	Response					_response;
	ClientInfo					_clientInfo;

//private:
	//Client(const Client &other);
	//Client& operator=(const Client &other);
public:
	explicit Client(const socketType serverSocket, std::vector<ServerConfig*> &socketConfigs) :
			_remoteSocket(serverSocket),
			_socketConfigs(socketConfigs),
			errorFlag(NONE),
			address_length(sizeof(address)),
			address(),
			_phase(WAITING_REQ),
			_header(),
			_bodyChunk(),
			_response(),
			_serverConf(NULL),
			_locationConf(NULL),
			_clientInfo(&_header, _socketConfigs[0], NULL)
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
				NI_NUMERICSERV | NI_NUMERICHOST);
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

	int getPhase() const {
		return _phase;
	}



public:
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
			for (std::vector<ServerConfig*>::iterator iter = _socketConfigs.begin(); iter != _socketConfigs.end(); ++iter) {
				if ((*iter)->getServerName() == *host)
					return (*iter);
			}
		}
		return ((_socketConfigs)[0]);
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

	void 	setServerAndLocation(){
		_serverConf = getServerConfigForLocation();
		if (!_serverConf){
			errorFlag = BAD_REQUEST; //no host name;
		}
		_locationConf = getLocation(*_serverConf, _header.getPath().getPath());
		if (!_locationConf){
			errorFlag = NOT_FOUND;
		}
		if (_locationConf->getAllowedMethods().find(_header.getRequestType()) == _locationConf->getAllowedMethods().end()){
			errorFlag = METHOD_NOT_ALLOWED;
		}
		_clientInfo.setServerConf(_serverConf);
		_clientInfo.setLocationConf(_locationConf);
	}

	bool	checkHeaderAndSet(){
		setServerAndLocation();
		if (errorFlag != NONE)
			return (false);
		if (_locationConf->getAllowedMethods().find(_header.getRequestType()) == _locationConf->getAllowedMethods().end()){
			errorFlag = METHOD_NOT_ALLOWED;
			return (false);
		}
		if (_header.getRequestType() == "GET")
			_bodyChunk.setDone();
		if (_header.getRequestType() == "POST" && !_header.has("Content-Length") && !_header.has("Transfer-Encoding"))
			_bodyChunk.setDone();
		if (_header.getRequestType() == "DELETE")
			_bodyChunk.setDone();
		if (_locationConf && !_locationConf->getRedirection().empty())
			_bodyChunk.setDone();
		if (FileSystem::isPathDotDot(_header.getPath().getPath())){
			errorFlag = BAD_REQUEST;
			return (false);
		}
		return (true);
	}



	int	constructResponse(){
		_phase = SENDING_RES;
		_response.setResponseStatus(200);
		_clientInfo.setContentLength(_bodyChunk.getTheCgiContentLength());
		if (errorFlag == BAD_REQUEST){
			_response.setResponseStatus(400);
		} else if (errorFlag == NOT_FOUND){
			_response.setResponseStatus(404);
		} else if (errorFlag == FORBIDDEN){
			_response.setResponseStatus(403);
		} else if (errorFlag == BODY_TOO_BIG){
			_response.setResponseStatus(413);
		} else if (errorFlag == METHOD_NOT_ALLOWED){
			_response.setResponseStatus(405);
		} else if (errorFlag == HTTP_Version_Not_Supported){
			_response.setResponseStatus(505);
		} else if (_locationConf && !_locationConf->getRedirection().empty()){
			_response.setResponseStatus(301);
		} else if (_bodyChunk.isIsDone() && _header.getRequestType() == "POST"){
			_response.setResponseStatus(201);
		}
		_response.setClientInfo(&_clientInfo);
		_clientInfo.setCreatedFile(_bodyChunk.getLastCreatedFilePath());
		_response.prepare(&_clientInfo);
		return (SUCCESS);
	}

/*****************************************************************/
// Client Core
/*****************************************************************/


	void 	readHeader(char*& chunk, size_type& chunkSize){
		size_t nbRead = _header.add(chunk, chunkSize);
		if (_header.getStatus() != Header::_NONE){
			errorFlag = BAD_REQUEST;
			if (_header.getStatus() == Header::HTTP_Version_Not_Supported)
				errorFlag = HTTP_Version_Not_Supported;
		}
		chunk += nbRead;
		chunkSize -= nbRead;
		if (_header.isDone()){
			if (errorFlag == NONE)
				checkHeaderAndSet();
			if (errorFlag == NONE){
				if (_bodyChunk.isIsDone() && chunkSize > 0)
					errorFlag = BAD_REQUEST;
				if (_bodyChunk.isIsDone())
					return ;
				_bodyChunk.prepare(*_locationConf, *_serverConf, _header);
			}
		}
	}

	void 	readBody(char *chunk, size_type chunkSize){
		if (_bodyChunk.getStatus() == BodyChunk::NONE)
			_bodyChunk.receive(chunk, chunkSize);
		switch(_bodyChunk.getStatus()){
			case BodyChunk::FORBIDDEN:
				errorFlag = FORBIDDEN;
				break;
			case BodyChunk::BODY_TOO_BEG:
				errorFlag = BODY_TOO_BIG;
				break;
			case BodyChunk::BAD_REQUEST:
				errorFlag = BAD_REQUEST;
				break;
		}
	}

	void	handleChunk(char *chunk, size_type chunkSize){
		if (!_header.isDone()){
			readHeader(chunk, chunkSize);
		}
		if (!_bodyChunk.isIsDone() && _header.isDone() && errorFlag == NONE && chunkSize > 0){
			readBody(chunk, chunkSize);
		}
		if (errorFlag != NONE || _bodyChunk.isIsDone()){
			constructResponse();
		}
	}

	//client want to read data
	int	sendResponse(){
		try{
			_response.sendData(_socket);
			if (_response.isIsComplete()){
				_response.clean();
				return (DONE);
			}
		}catch (...){
			_response.clean();
			return (FAILED);
		}
		return (SUCCESS);
	}


};

#endif //WEB_SERVER_CLIENT_H
