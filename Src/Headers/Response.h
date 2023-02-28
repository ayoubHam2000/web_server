/***************************************************/
/*     created by TheWebServerTeam 2/20/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_RESPONSE_H
#define DUMMY_CLIENT_RESPONSE_H


# include <Libraries.h>
# include <Header.h>
# include <ServerConfig.h>
# include <CGI.h>
# include <ClientInfo.h>

class Response {
public:
	enum {
		NONE, DONE, SENDING_HEADER_AND_CONTENT, SENDING_FILE, KEEP, OVERRIDE, APPEND
	};
private:
	const static size_type chunkSize = 1024;
	const static size_type maxFileSize = 4294967296UL;
private:
	ClientInfo							*_clientInfo;
	CGI											_cgi;
	int 								_responseStatus;
private:
	std::map<std::string, std::string>	_theHeaders;
	std::string 						_bodyContent;
	std::ifstream						_file;
	size_type 							_fileSize;
	std::string 						_fileRelativePath;
private:
	char								_buffer[chunkSize];
	const char*							_contentPtr;
	size_t 								_contentSize;
	int 								_sendStatus;


private:
	Response(const Response&);
	Response& operator=(const Response&);

public:
	Response():
			_clientInfo(NULL),
			_cgi(),
			_responseStatus(0),
			_theHeaders(),
			_bodyContent(),
			_file(),
			_fileSize(0),
			_fileRelativePath(),
			_contentPtr(NULL),
			_contentSize(0),
			_sendStatus(NONE)

	{}
	~Response(){}

public:

	bool isIsComplete() const {
		return _sendStatus == DONE;
	}

	void setResponseStatus(int errorPage) {
		_responseStatus = errorPage;
	}

	void setClientInfo(ClientInfo *clientInfo) {
		_clientInfo = clientInfo;
	}

	bool	isPrepared(){
		return _clientInfo != NULL;
	}

	void clean(){
		if (_file.is_open())
			_file.close();
	}

private:

	static std::string 	getStatusDescription(int status){
		switch (status) {
			case 200: return "200 OK";
			case 201: return "201 Created";
			case 301: return "301 Moved Permanently";
			case 400: return "400 Bad Request";
			//case 401: return "401 Unauthorized";
			//case 402: return "402 Payment Required";
			case 403: return "403 Forbidden";
			case 404: return "404 Not Found";
			case 405: return "405 Method Not Allowed";
			//case 406: return "406 Not Acceptable";
			//case 408: return "408 Request Timeout";
			case 413: return "413 Content Too Large";
			case 500: return "500 Internal Server Error";
			//case 501: return "501 Not Implemented";
			case 505: return "505 HTTP Version Not Supported";
			//case 507: return "507 Insufficient Storage";
		}
		throw std::runtime_error("UnExpected Status Code");
	}

	std::string getDate() {
		std::time_t time = std::time(NULL);
		std::stringstream ss;
		ss << "Date: " << std::put_time(std::gmtime(&time), "%a, %d %b %Y %H:%M:%S GMT");
		return ss.str();
	}

	std::string fileMemType(const std::string& file){
		const char *ext = FileSystem::fileExtension(file);
		if (ext){
			return (getMIMEType(std::string(ext)));
		}
		return (getMIMEType(""));
	}

	void addToHeaders(const std::string& key, const std::string& value, int flag){
		std::string k = key;
		capitalize(k);
		if (_theHeaders.find(k) == _theHeaders.end()){
			_theHeaders[k] = value;
		} else {
			if (flag == OVERRIDE)
				_theHeaders[k] = value;
			else if (flag == APPEND)
				_theHeaders[k] += "; " + value;
		}
	}

	bool headerHas(const std::string& key){
		return (_theHeaders.find(key) != _theHeaders.end());
	}

	const std::string& headerValueOf(const std::string& key){
		return (_theHeaders.at(key));
	}

	std::string headerConnection(int status){
		if (status < 300 && _clientInfo->getHeader().has("Connection") && _clientInfo->getHeader().valueOf("Connection") == "keep-alive"){
			return "keep-alive";
		}
		return "close";
	}

	void	printClientRequest(std::ostream &os){
		os << YEL"------------------------------------"RESET << std::endl;
		os << "[" << getDate() << "] "GRN << _clientInfo->getHeader().getRequestType() << RESET" " << _clientInfo->getHeader().getPath().getPath() << " . "
		   << _clientInfo->getHeader().getPath().getParams() << std::endl;
		os << BLU"Response Status: "RESET << getStatusDescription(_responseStatus) << std::endl;
		if (_clientInfo->isSet()){
			os << BLU"Location: "RESET << _clientInfo->getLocationConf().getLocation() << std::endl;
			os << BLU"Rout Folder: "RESET << _clientInfo->getLocationConf().getRootFolder() << std::endl;
			os << BLU"RequestedFile: "RESET << _clientInfo->getRequestedFile() << std::endl;
			os << BLU"filePath: "RESET << _clientInfo->getReqFileRelativePath() << std::endl;
			os << BLU"Server: "RESET << _clientInfo->getServerConf().getHost() << ":" << _clientInfo->getServerConf().getService() << std::endl;
			os << BLU"HostName: "RESET << _clientInfo->getServerConf().getServerName() << std::endl;
		}
		_clientInfo->getHeader().display(os);
		os << YEL"------------------------------------"RESET << std::endl;
	}

/*****************************************************************/
// Send Resource
/*****************************************************************/

	bool	getIndexPage(const std::string& dirPath){
		std::ostringstream oss;
		std::string _requestedPath = _clientInfo->getRequestedPath();
		DIR* dir = opendir(dirPath.c_str());
		if (dir)
		{
			struct dirent* entry;
			entry = readdir(dir);
			oss << "<!doctype html>\n"
				   "<html>\n"
				   "  <head>\n"
				   "    <title>Index</title>\n"
				   "  </head>\n"
				   "  <body>\n"
				   "    <h1><strong>Index</strong></h1></br></br>\n"
				   "    <table>\n";
			while (entry)
			{
				oss << "<tr>\n";
				if (entry->d_type == DT_REG || entry->d_type == DT_DIR)
				{
					oss << "<td><a href='";
					oss << _requestedPath;
					if (*(_requestedPath.end() - 1) != '/')
						oss << "/";
					oss << entry->d_name << "'>" << entry->d_name;
					oss << "</a></td>";
				}
				entry = readdir(dir);
				oss << "</tr>\n";
			}
			closedir(dir);
			oss << "</table></body></html>";
			_bodyContent = oss.str();
			return (true);
		}
		return (false);
	}

	std::string findIndexFile(){
		if (!_clientInfo->isSet())
			return ("");
		const std::string &a = _clientInfo->getHeader().getPath().getPath();
		const std::string &b = _clientInfo->getLocationConf().getLocation();
		if (a != b)
			return ("");
		const std::vector<std::string> &l = _clientInfo->getLocationConf().getIndexFiles();
		for (std::vector<std::string>::const_iterator iter = l.cbegin(); iter != l.cend(); ++iter){
			std::string path = _clientInfo->getLocationConf().getRootFolder() + "/" + *iter;
			if (FileSystem::file_exists(path.c_str()) && !FileSystem::isDirectory(path.c_str())){
				return (path);
			}
		}
		return ("");
	}

	void 	sendResource(const std::string& pagePath){
		if (!FileSystem::file_exists(pagePath.c_str())){
			_responseStatus = 404;
			writeResponse();
			return ;
		}
		if (FileSystem::isDirectory(pagePath.c_str())){
			std::string indexFile = findIndexFile();
			if (!indexFile.empty()){
				sendResource(indexFile);
			} else if (_clientInfo->getLocationConf().isAutoIndex()){
				getIndexPage(pagePath);
			} else {
				_responseStatus = 403;
				writeResponse();
			}
		} else {
			_file.open(pagePath);
			if (!_file.is_open()){
				throw std::runtime_error ("Can't open the file : " + pagePath);
			}
			_file.seekg(0, _file.end);
			_fileSize = _file.tellg();
			_file.seekg(0, _file.beg);
		}
	}

/*****************************************************************/
// CGI Response
/*****************************************************************/

	bool parseAndSetCGIHeader(const std::string &header){
		std::string::size_type startPos = 0;
		std::string::size_type endPos;
		endPos = header.find("\r\n", startPos);
		while ((endPos != std::string::npos)){
			if (startPos == endPos){
				return (true);
			}
			std::string line = header.substr(startPos, endPos - startPos);
			std::string::size_type pos = line.find(':');
			if (pos == std::string::npos)
				return (false);
			std::string key = line.substr(0, pos);
			std::string value = trim(line.substr(pos + 1));
			for (std::string::iterator iter = key.begin(); iter != key.end(); ++iter){
				if (*iter != '-' && !std::isalnum(*iter))
					return (false);
			}
			addToHeaders(key, value, APPEND);
			startPos = endPos + 2;
			endPos = header.find("\r\n", startPos);
		}
		return (true);
	}

	void	CGIResponse(const std::string& file){
		MyBuffer		headerBuffer(10000, "\r\n\r\n");
		char			buffer[1000];

		try{
			_file.open(file.c_str());
			if (!_file.is_open()){
				throw std::runtime_error(TRACK_WHERE + "Can't read from " + file);
			}
			while (true){
				_file.read(buffer, 1000);
				headerBuffer.add(buffer, 1000);
				if (headerBuffer.isFull() && !headerBuffer.isMatch()){
					throw std::runtime_error(TRACK_WHERE + "Buffer is Full");
				}
				if (headerBuffer.isMatch()){
					std::string h = std::string(headerBuffer.getBuffer(), headerBuffer.getSize());
					int status = parseAndSetCGIHeader(h);
					if (!status){
						throw std::runtime_error(TRACK_WHERE + " Bad Cgi Headers");
					}
					_file.clear();
					_file.seekg(0, _file.beg);
					_file.seekg(0, _file.end);
					_fileSize = _file.tellg();
					_file.seekg(0, _file.beg);
					_file.seekg(headerBuffer.getSize(), _file.beg);
					_fileSize -= headerBuffer.getSize();
					break ;
				}
			}
		} catch (const std::exception& e){
			_responseStatus = 500;
			std::cerr << "Error: " << e.what() << std::endl;
			writeResponse();
		}
	}

/*****************************************************************/
// WriteResponse
/*****************************************************************/

	void	writeResponsePage(){
		if (_clientInfo->getHeader().getRequestType() == "DELETE"){
			FileSystem::removeAll(_clientInfo->getReqFileRelativePath().c_str());
		}
		else if (_clientInfo->isRequestCanHandledByCgi()){
			try{
				_cgi.prepare(_clientInfo);
			}catch (...){
				_responseStatus = 400;
				writeResponse();
			}
		}else{
			if (_responseStatus == 201){
				_bodyContent = "Success";
			}
			else if (_responseStatus == 200){
				_fileRelativePath = _clientInfo->getReqFileRelativePath();
				sendResource(_fileRelativePath);
			}
		}
	}

	void 	writeRedirectionPage(){
		const std::string& location = _clientInfo->getLocationConf().getRedirection();
		addToHeaders("Location", location, KEEP);
		addToHeaders("Content-Length", "0", OVERRIDE);
	}

	void 	writeErrorPage(){
		if (_clientInfo->isServerConfSet()){
			const std::map<int, std::string> &e = _clientInfo->getServerConf().getErrorPages();
			if (e.find(_responseStatus) != e.end()){
				_fileRelativePath = _clientInfo->getServerConf().getErrorPages().at(_responseStatus);
				sendResource(_fileRelativePath);
				return ;
			}
		}
		_responseStatus = 500;
		writeResponse();
	}

	void 	writeInternalError(){
		_bodyContent = "Internal Server Error " + std::to_string(_responseStatus);
	}

	void 	writeResponse(){
		_theHeaders.clear();
		_bodyContent.clear();
		if (_file.is_open())
			_file.close();
		if (_responseStatus >= 500)
			writeInternalError();
		else {
			try {
				if (_responseStatus >= 400)
					writeErrorPage();
				else {
					if (!_clientInfo->isSet()){
						throw std::runtime_error("clientInfo not set");
					} else {
						if (_responseStatus >= 300)
							writeRedirectionPage();
						else //200
							writeResponsePage();
					}
				}
			} catch (std::exception& e){
				std::cerr << e.what() << std::endl;
				_responseStatus = 500;
				writeResponse();
			}
		}
	}

public:

/*****************************************************************/
// Prepare
/*****************************************************************/

	void	prepare(ClientInfo* clientInfo){
		_clientInfo = clientInfo;

		printClientRequest(std::cout);
		writeResponse();
	}

/*****************************************************************/
// Send
/*****************************************************************/
private:

	bool 	prepareCgiResponse(){
		if (_cgi.isPrepared()){
			if (_cgi.getStatus() != CGI::DONE){
				_cgi.read();
				if (_cgi.getStatus() == CGI::DONE){
					_fileRelativePath = _cgi.getOutFilePath();
					CGIResponse(_cgi.getOutFilePath());
					return (true);
				} else if (_cgi.getStatus() == CGI::ERROR){
					_responseStatus = 500;
					writeResponse();
					return (true);
				}
			}
			return (false);
		}
		return (true);
	}

	void 	prepareToSendMemoryData(){
		std::ostringstream oss;
		std::string status;

		//in case 400 or 500 the _clientInfo location may not exist
		if (_clientInfo->isSet() && !prepareCgiResponse())
			return ;
		status = getStatusDescription(_responseStatus);
		if (_cgi.isPrepared() && _cgi.getStatus() != CGI::ERROR){
			addToHeaders("Content-Type", "text/html; charset=utf-8", KEEP);
			if (headerHas("Location")){
				_responseStatus = 301;
				status = getStatusDescription(301);
			}
			if (headerHas("Status")){
				status = headerValueOf("Status");
				_responseStatus = std::stoi(status);
			}
		}
		addToHeaders("Date", getDate(), KEEP);
		addToHeaders("Server", "WebServer1.0 (Mac)", KEEP);
		addToHeaders("Connection", headerConnection(_responseStatus), KEEP);
		if (_file.is_open()){
			addToHeaders("Content-Type", fileMemType(_fileRelativePath) + "; charset=utf-8", KEEP);
			addToHeaders("Content-Length", std::to_string(_fileSize), OVERRIDE);
		} else if (!_bodyContent.empty()){
			addToHeaders("Content-Type", "text/html; charset=utf-8", KEEP);
			addToHeaders("Content-Length", std::to_string(_bodyContent.size()), OVERRIDE);
		}

		oss << "HTTP/1.1 " + status + "\r\n";
		for (std::map<std::string, std::string>::iterator iter = _theHeaders.begin(); iter != _theHeaders.end(); ++iter){
			oss << iter->first << ": " << iter->second << "\r\n";
		}
		oss << "\r\n";
		std::cout << "Server Response: " << std::endl << oss.str() << std::endl;
		_bodyContent = oss.str() + _bodyContent;
		_contentPtr = _bodyContent.c_str();
		_contentSize = _bodyContent.size();
		_sendStatus = SENDING_HEADER_AND_CONTENT;
	}

	void 	sendMemContent(socketType sock){
		size_type nbSend = send(sock, _contentPtr, _contentSize, 0);
		if (nbSend == -1){
			throw std::runtime_error("Can't send sendData");
		}
		_contentSize -= nbSend;
		_contentPtr += nbSend;
		if (_contentSize == 0){
			_sendStatus = SENDING_FILE;
		}
	}

	void 	sendFile(socketType sock){
		if (_contentSize == 0){
			size_t minChunkSize = _fileSize;
			if (chunkSize < _fileSize)
				minChunkSize = chunkSize;
			_fileSize -= minChunkSize;
			_file.read(_buffer, (int)minChunkSize);
			_contentPtr = _buffer;
			if (_file.fail()){
				throw std::runtime_error("Can't read : sendData");
			}
			_contentSize = _file.gcount();
		}
		if (_contentSize == 0){
			_sendStatus = DONE;
			return ;
		}
		size_type nbSend = send(sock, _contentPtr, _contentSize, 0);
		if (nbSend == -1){
			throw std::runtime_error("Can't send sendData");
		}
		_contentSize -= nbSend;
		_contentPtr += nbSend;
	}

public:

	void 	sendData(socketType sock){
		if (!isPrepared()){
			std::cerr << "Can't Send Data" << std::endl;
			_sendStatus = DONE;
		}
		if (_sendStatus == NONE){
			prepareToSendMemoryData();
		}
		if (_sendStatus == SENDING_HEADER_AND_CONTENT){
			sendMemContent(sock);
		}
		if (_sendStatus == SENDING_FILE){
			sendFile(sock);
		}
	}

};


#endif //DUMMY_CLIENT_RESPONSE_H
