/***************************************************/
/*     created by TheWebServerTeam 2/18/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_HEADER_H
#define DUMMY_CLIENT_HEADER_H

#include <array>
#include "Libraries.h"
#include "HeaderPath.h"
#include "MyBuffer.h"

class Header {
public:
	enum {
		_NONE,
		BAD_REQUEST, HTTP_Version_Not_Supported
	};
private:
	std::string							_requestType;//GET POST DELETE | Get
	HeaderPath							_pathQuery;
	std::string							_protocol;//HTTP/1.1
	std::map<std::string, std::string>	_httpHeaders;//alphaNum-:.*
private:
	MyBuffer							_requestHeader;
	int									_status;

#pragma region Construction Getters Setters

public:

	Header() :
			_requestType(),
			_pathQuery(),
			_protocol(),
			_httpHeaders(),
			_requestHeader(Const::MAX_REQUEST_SIZE, "\r\n\r\n"),
			_status(_NONE)
	{}

	Header(const Header& other) :
			_requestType(other._requestType),
			_pathQuery(other._pathQuery),
			_protocol(other._protocol),
			_httpHeaders(other._httpHeaders)
	{}

	Header& operator=(const Header& other) {
		_requestType = other._requestType;
		_pathQuery = other._pathQuery;
		_protocol = other._protocol;
		_httpHeaders = other._httpHeaders;
		return *this;
	}

	~Header(){}

public:

	bool isDone(){
		return _requestHeader.isMatch();
	}

	int getStatus() const {
		return _status;
	}

	const std::string &getRequestType() const {
		return _requestType;
	}

	const HeaderPath &getPath() const {
		return _pathQuery;
	}

	const std::string &getProtocol() const {
		return _protocol;
	}

	const std::map<std::string, std::string> &getHttpHeaders() const {
		return _httpHeaders;
	}
#pragma  endregion

# pragma region PARSE
public:


	bool	getRequestLine(const std::string &requestLine){
		std::vector<std::string> requestTokens;
		std::string token;
		std::stringstream iss(requestLine);
		while (std::getline(iss, token, ' ')){
			requestTokens.push_back(token);
		}
		//three tokens only
		if (requestTokens.size() != 3)
			return (false);

		//validate request type
		_requestType = requestTokens[0];
		std::transform(_requestType.begin(), _requestType.end(), _requestType.begin(), toupper);

		//validate path
		if (!_pathQuery.parse(requestTokens[1]))
			return (false);

		//validate _protocol
		_protocol = requestTokens[2];
		if (_protocol != "HTTP/1.1"){
			_status = HTTP_Version_Not_Supported;
			return (false);
		}
		return (true);
	}

	bool _isValidContentLength(const std::string& contentLength){
		std::string::size_type i = 0;
		while (i < contentLength.size()){
			char c = contentLength[i];
			if (!(c >= '0' && c <= '9'))
				return (false);
			i++;
		}
		return (true);
	}

	bool _isValidContentType(const std::string& contentType){
		if (contentType.find("multipart/form-data") != std::string::npos){
			if (contentType.find("multipart/form-data; boundary=") != std::string::npos){
				if (contentType.size() > 30)
					return (true);
			}
		}
		return (false);
	}

	bool _checkForHeaderValues(){
		if (
			(has("Connection") && valueOf("Connection") != "close" && valueOf("Connection") != "keep-alive")
			|| (getRequestType() == "GET" && (has("Content-Length") || has("Transfer-Encoding")))
			|| (has("Transfer-Encoding") && has("Content-Length"))
			|| (has("Content-Length") && !_isValidContentLength(valueOf("Content-Length")))
			|| (has("Content-Type") && _isValidContentType(valueOf("Content-Type")))
		){
			return (false);

		}
		return (true);
	}

	bool parse(const std::string &header){
		//the header string is already ended by \r\n\r\n
		std::string::size_type startPos = 0;
		std::string::size_type endPos;
		bool isRequestLine = true;
		endPos = header.find("\r\n", startPos);
		while ((endPos != std::string::npos)){
			if (startPos == endPos){
				if (isRequestLine)
					return (false);
				return (true);
			}
			std::string line = header.substr(startPos, endPos - startPos);
			if (isRequestLine){
				//request Line
				if (!getRequestLine(line)){
					return (false);
				}
				isRequestLine = false;
			}else{
				//key value
				std::string::size_type pos = line.find(':');
				if (pos == std::string::npos)
					return (false);
				std::string key = line.substr(0, pos);
				std::string value = trim(line.substr(pos + 1));
				for (std::string::iterator iter = key.begin(); iter != key.end(); ++iter){
					if (*iter != '-' && !std::isalnum(*iter))
						return (false);
				}
				std::transform(key.begin(), key.end(), key.begin(), toupper);
				if (_httpHeaders.find(key) == _httpHeaders.end()){
					_httpHeaders[key] = value;
				}
				else {
					_httpHeaders[key] += "; " + value;
				}
			}
			startPos = endPos + 2;
			endPos = header.find("\r\n", startPos);
		}
		return (!isRequestLine && _checkForHeaderValues());
	}

	bool has(const std::string &key) const{
		std::string s(key.size(), '*');
		std::transform(key.cbegin(), key.cend(), s.begin(), toupper);
		return _httpHeaders.find(s) != _httpHeaders.end();
	}

	const std::string& valueOf(const std::string& key) const{
		std::string s(key.size(), '*');
		std::transform(key.cbegin(), key.cend(), s.begin(), toupper);
		return _httpHeaders.at(s);
	}

	void display(std::ostream& os) const {
		for (std::map<std::string, std::string>::const_iterator iter = _httpHeaders.cbegin(); iter != _httpHeaders.cend(); ++iter){
			os << iter->first << ": " << iter->second << std::endl;
		}
	}

# pragma endregion

public:

	size_t 	add(char *chunk, size_type chunkSize){
		size_type nbRead = _requestHeader.add(chunk, chunkSize);
		if (_requestHeader.isFull() && !_requestHeader.isMatch()){
			_status = BAD_REQUEST;
		}
		if (_requestHeader.isMatch()){
			int s = parse(std::string(_requestHeader.getBuffer(), _requestHeader.getSize()));
			if (!s && _status == _NONE){
				_status = BAD_REQUEST;
			}
		}
		return (nbRead);
	}

public:

	static void test(const std::string &filePath){
		Header header;
		HeaderPath path;

		std::ifstream file(filePath);
		file.seekg(0, file.end);
		unsigned int length = file.tellg();
		file.seekg(0, file.beg);
		char _buffer[length + 1];
		file.read(_buffer, length);
		_buffer[length] = 0;

		//start
		std::string content(_buffer);
		bool res = header.parse(content);
		std::cout << (res ? "Valid" : "Error") << std::endl;
		if (res){
			std::cout << header.getRequestType() << std::endl;
			std::cout << header.getPath().getPath() << std::endl;
			std::cout << header.getPath().getParams() << std::endl;
			std::cout << header.getProtocol() << std::endl;
			std::map<std::string, std::string> map = header._httpHeaders;
			for (std::map<std::string, std::string>::iterator iter = map.begin(); iter != map.end(); ++iter){
				std::cout << iter->first << " " << iter->second << std::endl;
			}
		}
	}



};


#endif //DUMMY_CLIENT_HEADER_H
