/***************************************************/
/*     created by TheWebServerTeam 2/20/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_BODYCHUNK_H
#define DUMMY_CLIENT_BODYCHUNK_H


#include <Libraries.h>
#include <Header.h>
#include <ServerConfig.h>
#include <ChunkContentHandler.hpp>
#include <MyBuffer.h>

class BodyChunk {
private:
	bool 				_isPrepared;
	bool 				_isChunked;
	bool 				_isMultiFormData;
	bool 				_isDone;
	bool 				_fileIsOpen;
	std::string 		_boundary;
	std::string			_uploadFolder;
	std::string			_contentTypeHeader;
	std::string			_fileMimeType;
	size_type 			_contentLength;
	size_type 			_theCGIContentLength;
	ChunkContentHandler _chunkHandler;
	MyBuffer			_myBuffer;
	std::ofstream 		_outFile;
	int 				_error;

	int 				__multiFormStatus;
	size_type 			__nbRead;
public:
	enum {
		SUCCESS = 0, ERROR = -1, BODY_TOO_BIG = -2
	};
private:
	BodyChunk(const BodyChunk& other);
	BodyChunk& operator=(const BodyChunk& other);
public:
	BodyChunk():
			_isPrepared(false),
			_isChunked(false),
			_isDone(false),
			_uploadFolder(),
			_contentLength(0),
			_isMultiFormData(false),
			_fileIsOpen(false),
			_boundary(),
			_contentTypeHeader(),
			_fileMimeType(),
			_myBuffer(read_chunk_size + 500), //max_read_size
		__multiFormStatus(0),
			__nbRead(0),
			_error(0),
			_theCGIContentLength(0)
	{}
	~BodyChunk(){}
public:

	size_type getTheCgiContentLength() const {
		return _theCGIContentLength;
	}

	bool isIsDone() const {
		return _isDone;
	}

	void setDone(){
		_isDone = true;
	}

public:

	void closeFile(){
		if (_fileIsOpen){
			_fileIsOpen = false;
			_outFile.close();
		}
	}

	std::string _generateRandomName(){
		const int	length = 7;
		const int	buffer_size = 100;
		char		name[length + 1];
		char		buffer[buffer_size];
		int			i = 0;
		std::time_t time = std::time(NULL);

		std::ifstream file("/dev/random");
		if (!file.is_open())
			throw std::runtime_error("Error can't open /dev/random");
		file.read(buffer, buffer_size);
		for (int j = 0; j < buffer_size; j++){
			if (std::isalnum(buffer[j]))
				name[i++] = buffer[j];
			if (i == length){
				name[i] = 0;
				break ;
			}
		}
		std::stringstream ss;
		ss << std::put_time(std::gmtime(&time), "%d%m%Y%_%H%M%S") << "__" << name;
		return (ss.str());
	}

	void setUploadFolder(const LocationConfig& locationConf){
		_uploadFolder = locationConf.getRootFolder() + locationConf.getUploadFolder().substr(locationConf.getUploadFolder().find('/'));
	}

	int prepare(const LocationConfig& locationConf, const ServerConfig& serverConfig, const Header& header){
		setUploadFolder(locationConf);
		if (header.has("Content-Length")){
			try{
				_contentLength = std::stoul(header.valueOf("Content-Length"));
				_theCGIContentLength = _contentLength;
				if (_contentLength > serverConfig.getMaxClientBodySize())
					return (BODY_TOO_BIG);
				if (_contentLength == 0)
					_isDone = true;
			}catch (...){
				return (ERROR);
			}
		} else if (header.has("Transfer-Encoding") && header.valueOf("Transfer-Encoding") == "chunked"){
			_isChunked = true;
		}
		if (header.has("Content-Type")){
			_contentTypeHeader = header.valueOf("Content-Type");
			_fileMimeType = _contentTypeHeader;
			if (header.valueOf("Content-Type").find("multipart/form-data") != std::string::npos){
				_isMultiFormData = true;
				_boundary = header.valueOf("Content-Type").substr(header.valueOf("Content-Type").find("boundary=") + 9);
			}
		}
		_isPrepared = true;
		return (SUCCESS);
	}

	void writeToFile(const char *chunk, size_type size){
		if (size == 0)
			return ;
		if (!_fileIsOpen){
			_fileIsOpen = true;
			std::string ext = getExtension(_fileMimeType);
			std::string filePath = _uploadFolder + "/" + _generateRandomName() + ext;
			_outFile.open(filePath);
			if (!_outFile.is_open())
				throw std::runtime_error("Can't open " + filePath);
		}
		_outFile.write(chunk, size);
		if (_outFile.bad()) {
			std::cerr << "Write operation failed!" << std::endl;
		}
	}

	void handleMultiForm(const char *chunk, size_type size){
		if (__multiFormStatus == 0){
			std::string boundary = "--" + _boundary + "\r\n";
			_myBuffer.setUntil(boundary.c_str());
			__multiFormStatus = 1;
		}
		while (size){
			MyBuffer::size_type nbRead = _myBuffer.add(chunk, size);
			chunk += nbRead;
			size -= nbRead;
			if (_myBuffer.isFull()){
				_error = ERROR;
				return ;
			}
			if (_myBuffer.isMatch()){
				if (__multiFormStatus == 1){
					if (_myBuffer.getSize() > _myBuffer.getUntilSize()){
						std::string data(_myBuffer.getBuffer(), _myBuffer.getSize() - _myBuffer.getUntilSize());
						writeToFile(data.c_str(), data.size());
					}
					_myBuffer.setUntil("\r\n\r\n");
					_myBuffer.resetBuffer();
					__multiFormStatus++;
				}
				else if (__multiFormStatus == 2){
					std::string miniHeader = std::string(_myBuffer.getBuffer(), _myBuffer.getSize());
					std::string::size_type contentTypeEndPos = miniHeader.find("Content-Type: ") + 14;
					std::string::size_type contentTypeSize = miniHeader.find("\r\n", contentTypeEndPos) - contentTypeEndPos;
					if (contentTypeEndPos != std::string::npos)
						_fileMimeType = miniHeader.substr(contentTypeEndPos, contentTypeSize);
					closeFile();
					std::string boundary = "\r\n--" + _boundary + "\r\n";
					_myBuffer.setUntil(boundary.c_str());
					_myBuffer.resetBuffer();
					__multiFormStatus = 1;
				}
			} else {
				if (__multiFormStatus == 1){
					if (_myBuffer.getSize() > _myBuffer.getMatchCounter()){
						std::string d(_myBuffer.getBuffer(), _myBuffer.getSize());
						std::string::size_type pos = d.find("\r\n--" + _boundary + "--\r\n");
						if (pos != std::string::npos){
							std::string data = d.substr(0, pos);
							writeToFile(data.c_str(), data.size());
							_isDone = true;
							return ;
						} else {
							std::string data(_myBuffer.getBuffer(), _myBuffer.getSize() - _myBuffer.getMatchCounter());
							writeToFile(data.c_str(), data.size());
						}
					}
					_myBuffer.shift();
				}
			}
		}
	}

	void writeChunk(const char *chunk, size_type size){
		if (_isMultiFormData){
			//writeToFile(chunk, size);
			handleMultiForm(chunk, size);
		}else{
			writeToFile(chunk, size);
		}
		if (_isDone){
			closeFile();
		}
	}

	int receive(const char *chunk, size_type size){
		std::string newChunk;
		int 		res = SUCCESS;

		if (!_isPrepared)
			return (ERROR);
		//read chunk
		if (_isChunked){
			_chunkHandler.getHttpChunkContent(chunk, size, newChunk);
			if (_chunkHandler.is_failed()){
				std::cerr << "_chunkHandler: " << _chunkHandler.getError() << std::endl;
				return (ERROR);
			}
			_isDone = _chunkHandler.is_done();
			chunk = newChunk.c_str();
			size = newChunk.size();
			_theCGIContentLength += size;
		} else {
			size = std::min(size, _contentLength);
			_contentLength -= size;
			if (_contentLength == 0){
				_isDone = true;
			}
		}
		writeChunk(chunk, size);
		if (_error < 0){
			res = _error;
		}
		if (_isDone){
			closeFile();
		}
		return (res);
	}

};


#endif //DUMMY_CLIENT_BODYCHUNK_H
