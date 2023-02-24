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
		NONE, E404, E400, P300
	};
private:
	const static size_type chunkSize = 1024;
	const static size_type maxFileSize = 4294967296UL;
private:
	bool 				_isPrepared;
	bool 				_isComplete;
	std::ifstream		_inFile;
	size_type 			_fileSize;
	char				_buffer[chunkSize];
	const char* 		_ptr;
	size_type			_counter;
	int 				_responseStatus;
	bool 				_headerDone;
	std::ostringstream	_headerStream;
	std::string 		_headerData;
	std::string 		_requestedPath;
	ClientInfo			*_clientInfo;
	CGI							_cgi;

private:
	Response(const Response&);
	Response& operator=(const Response&);
public:
	Response():
			_isPrepared(false),
			_isComplete(false),
			_ptr(NULL),
			_counter(0),
			_responseStatus(0),
			_fileSize(0),
			_headerDone(false),
			_cgi(),
			_headerStream(),
			_clientInfo(NULL)
	{}
	~Response(){}
public:

	bool isIsComplete() const {
		return _isComplete;
	}

	void setResponseStatus(int errorPage) {
		_responseStatus = errorPage;
	}

public:

	std::string getDate() {
		std::time_t time = std::time(NULL);
		std::stringstream ss;
		ss << "Date: " << std::put_time(std::gmtime(&time), "%a, %d %b %Y %H:%M:%S GMT");
		return ss.str();
	}

	const char *fileExtension(const std::string& file){
		return std::strrchr(file.c_str(), '.');
	}

	std::string fileMemType(const std::string& file){
		const char *ext = fileExtension(file);
		if (ext){
			return (getMIMEType(std::string(ext)));
		}
		return (getMIMEType(""));
	}

	bool	getIndexPage(const std::string& dirPath, std::ostringstream &oss){
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
			return (true);
		}
		return (false);
	}

	void openResource(const std::string& file){
		std::ostringstream indexStream;

		if (getIndexPage(file, indexStream)){
			_headerStream << "Content-Type: text/html; charset=utf-8" << "\r\n";
			_headerStream << "Content-Length: " << indexStream.str().size() << "\r\n\r\n";
			_headerStream << indexStream.str();
		}else{
			_inFile.open(file);
			if (!_inFile.is_open()){
				throw std::runtime_error("Can't Open file -> path:" + file);
			}
			_inFile.seekg(0, _inFile.end);
			_fileSize = _inFile.tellg();
			if (_fileSize > maxFileSize){
				throw std::runtime_error("File is too big");
			}
			_inFile.seekg(0, _inFile.beg);
			_headerStream << "Content-Type: " << fileMemType(file) << "; charset=utf-8"<< "\r\n";
			_headerStream << "Content-Length: " << _fileSize << "\r\n\r\n";
		}
	}

	static std::string 	getStatusDescription(int status){
		switch (status) {
			case 400: return "400 Bad Request";
			case 401: return "401 Unauthorized";
			case 402: return "402 Payment Required";
			case 403: return "403 Forbidden";
			case 405: return "405 Method Not Allowed";
			case 406: return "406 Not Acceptable";
			case 408: return "408 Request Timeout";
			case 414: return "414 URI Too Long";
			case 500: return "500 Internal Server Error";
			case 501: return "501 Not Implemented";
			case 505: return "505 HTTP Version Not Supported";
			case 507: return "507 Insufficient Storage";
			case 200: return "200 OK";
			case 201: return "201 Created";
			case 301: return "301 Moved Permanently";
		}
		throw std::runtime_error("UnExpected Status Code");
	}

	void 	sendFile(const std::string& pagePath){
		_headerStream << "HTTP/1.1 " << getStatusDescription(_responseStatus) << "\r\n";
		_headerStream << "Date: " << getDate() << "\r\n";
		_headerStream << "Server: WebServer1.0 (Mac)\r\n";
		_headerStream << "Connection: keep-alive\r\n";
		try{
			openResource(pagePath); //it will close the header (write an index page or open a file)
		}catch (std::exception &e){
			std::cerr << e.what() << std::endl;
			_headerStream.clear();
			_headerStream << "HTTP/1.1 404 Not Found" << "\r\n";
			_headerStream << "Date: " << getDate() << "\r\n";
			_headerStream << "Server: WebServer1.0 (Mac)\r\n";
			_headerStream << "Connection: close\r\n\r\nNot Found";
		}
	}

	void	sendHeader(const std::string& status, const std::string& info){
		_headerStream << "HTTP/1.1 " << status << "\r\n";
		_headerStream << "Date: " << getDate() << "\r\n";
		_headerStream << "Server: WebServer1.0 (Mac)\r\n";
		if (!info.empty())
			_headerStream << info;
		_headerStream << "Connection: close\r\n";
		_headerStream << "\r\n";
	}

	void 	sendRedirection(const std::string& location){
		std::ostringstream info;
		info << "Location: " << location << "\r\n";
		info << "Content-Length: 0\r\n";
		sendHeader("301 Moved Permanently", info.str());
	}

	void	printClientInfo(std::ostream &os){
		os << YEL"------------------------------------"RESET << std::endl;
		os << "[" << getDate() << "] "GRN << _clientInfo->getHeader().getRequestType() << RESET" " << _clientInfo->getHeader().getPath().getPath() << " . "
			<< _clientInfo->getHeader().getPath().getParams() << std::endl;
		os << BLU"Location: "RESET << _clientInfo->getLocationConf().getLocation() << std::endl;
		os << BLU"Rout Folder: "RESET << _clientInfo->getLocationConf().getRootFolder() << std::endl;
		os << BLU"RequestedFile: "RESET << _clientInfo->getRequestedFile() << std::endl;
		os << BLU"filePath: "RESET << _clientInfo->getReqFileRelativePath() << std::endl;
		os << BLU"Response Status: "RESET << _responseStatus << std::endl;
		os << BLU"Server: "RESET << _clientInfo->getServerConf().getHost() << ":" << _clientInfo->getServerConf().getService() << std::endl;
		os << BLU"HostName: "RESET << _clientInfo->getServerConf().getServerName() << std::endl;
		_clientInfo->getHeader().display(os);
		os << YEL"------------------------------------"RESET << std::endl;
	}

	void	prepare(ClientInfo* clientInfo){
		_isPrepared = true;
		_clientInfo = clientInfo;
		_requestedPath = clientInfo->getRequestedPath();
		printClientInfo(std::cout);
		if (_responseStatus >= 400){
			sendFile(clientInfo->getErrorPage(_responseStatus));
		}
		if (_responseStatus < 300){ //200
			if (clientInfo->isRequestCanHandledByCgi()){
				_cgi.prepare(_clientInfo);
			}else{
				if (_responseStatus == 201){
					sendHeader("201 Success", "");
				}
				else if (_responseStatus == 200){
					sendFile(_clientInfo->getReqFileRelativePath());
				}
			}
		}
		_headerData = _headerStream.str();
		_ptr = _headerData.c_str();
		_counter = _headerData.size();
	}

	void	sendData(socketType sock){
		if (!_isPrepared || !_ptr){
			std::cout << "Can't Send Data" << std::endl;
			return;
		}
		if (!_headerDone){
			size_type nbSend = send(sock, _ptr, _counter, 0);
			if (nbSend == -1){
				throw std::runtime_error("Can't send sendData");
			}
			_counter -= nbSend;
			_ptr += nbSend;
			if (_counter == 0){
				_headerDone = true;
			}
		}
		if (_headerDone && _fileSize){
			if (_counter == 0){
				size_type bbb = _fileSize;
				if (chunkSize < _fileSize)
					bbb = chunkSize;
				_fileSize -= bbb;
				_inFile.read(_buffer, bbb);
				_ptr = _buffer;
				if (_inFile.fail()){
					throw std::runtime_error("Can't read sendData");
				}
				_counter = _inFile.gcount();
			}
			if (_counter == 0){
				_isComplete = true;
				if (_inFile.is_open())
					_inFile.close();
				return ;
			}
			size_type nbSend = send(sock, _ptr, _counter, 0);
			if (nbSend == -1){
				throw std::runtime_error("Can't send sendData");
			}
			_counter -= nbSend;
			_ptr += nbSend;
		} else {
			_isComplete = true;
			if (_inFile.is_open())
				_inFile.close();
		}
	}

};


#endif //DUMMY_CLIENT_RESPONSE_H
