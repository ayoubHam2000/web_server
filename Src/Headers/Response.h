/***************************************************/
/*     created by TheWebServerTeam 2/20/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_RESPONSE_H
#define DUMMY_CLIENT_RESPONSE_H


#include <Libraries.h>
#include <Header.h>
#include <ServerConfig.h>

class Response {
public:
	enum {
		NONE, E404, E400, P300
	};
private:
	const static size_type chunk_size = 1024;
private:
	bool 			_isPrepared;
	bool 			_isComplete;
	std::ifstream	_inFile;
	char			_buffer[chunk_size];
	char* 			_ptr;
	size_type		_counter;
	int 			_errorPage;
private:
	Response(const Response&);
	Response& operator=(const Response&);
public:
	Response():
		_isPrepared(false),
		_isComplete(false),
		_ptr(NULL),
		_counter(0),
		_errorPage(0)
	{}
	~Response(){}
public:

	bool isIsComplete() const {
		return _isComplete;
	}

	void setErrorPage(int errorPage) {
		_errorPage = errorPage;
	}

public:

	void	prepare(const LocationConfig& locationConf, const ServerConfig& serverConfig, const Header& header){
		_isPrepared = true;
		std::stringstream ss;
		ss << "HTTP/1.1 ";
		switch (_errorPage) {
			case E404:ss << "404 Not Found\r\n"; break;
			case E400:ss << "400 Bad Request\r\n"; break;
			default:ss << "200 OK";
		}
		ss << "Connection: close\r\r";
		ss << "\r\n";
		_counter = ss.str().size();
		std::strncpy(_buffer, ss.str().c_str(), _counter);
	}

	void	sendData(socketType sock){
		if (!_isPrepared)
			return;
		if (_counter == 0){
			_inFile.read(_buffer, chunk_size);
			_ptr = _buffer;
			if (_inFile.fail()){
				throw std::runtime_error("Can't read sendData");
			}
			_counter = _inFile.gcount();
			if (_counter == 0){
				_isComplete = true;
				return ;
			}
		}
		size_type nbSend = send(sock, _ptr, _counter, 0);
		if (nbSend == -1){
			throw std::runtime_error("Can't send sendData");
		}
		_counter -= nbSend;
		_ptr += nbSend;
	}

};


#endif //DUMMY_CLIENT_RESPONSE_H
