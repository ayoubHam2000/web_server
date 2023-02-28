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
#include "ClientInfo.h"

class BodyChunk {
public:
	enum {
		NONE, FORBIDDEN, DONE, BODY_TOO_BEG, BAD_REQUEST
	};
private:
	int 				_status;
	ClientInfo			_clientInfo;
	bool 				_isChunked;
	bool 				_isMultiFormData;
	std::string 		_boundary;
	std::string			_uploadFolder;
	std::string			_contentTypeHeader;
	std::string			_fileMimeType;
	size_type 			_contentLength;
	size_type 			_theCGIContentLength;
	ChunkContentHandler _chunkHandler;
	MyBuffer			_myBuffer;
	std::ofstream 		_outFile;
	std::string			_filePath;

	int 				_multiFormStatus;
private:
	BodyChunk(const BodyChunk& other);
	BodyChunk& operator=(const BodyChunk& other);
public:
	BodyChunk():
			_status(NONE),
			_clientInfo(NULL, NULL, NULL),
			_isChunked(false),
			_isMultiFormData(false),
			_boundary(),
			_uploadFolder(),
			_contentTypeHeader(),
			_fileMimeType(),
			_contentLength(0),
			_theCGIContentLength(0),
			_chunkHandler(),
			_myBuffer(read_chunk_size + 500), //max_read_size
			_multiFormStatus(0),
			_outFile(),
			_filePath()
	{}
	~BodyChunk(){}
public:

	const std::string &getLastCreatedFilePath() const {
		return _filePath;
	}

	size_type getTheCgiContentLength() const {
		return _theCGIContentLength;
	}

	bool isIsDone() const {
		return _status == DONE;
	}

	void setDone(){
		_status = DONE;
	}

	int getStatus() const {
		return _status;
	}

public:

	void closeFile(){
		if (_outFile.is_open()){
			_outFile.close();
		}
	}

/*****************************************************************/
// prepare
/*****************************************************************/

	void prepare(const LocationConfig& locationConf, const ServerConfig& serverConfig, const Header& header){
		_clientInfo.setServerConf(&serverConfig);
		_clientInfo.setLocationConf(&locationConf);
		_clientInfo.setHeader(&header);
		_uploadFolder = locationConf.getUploadFolder();
		if (header.has("Content-Length")){
			try{
				_contentLength = std::stoul(header.valueOf("Content-Length"));
				_theCGIContentLength = _contentLength;
				if (_contentLength >= serverConfig.getMaxClientBodySize()){
					_status = BODY_TOO_BEG;
					return ;
				}
				if (_contentLength == 0){
					_status = DONE;
					return ;
				}
			}catch (...){
				_status = BAD_REQUEST;
				return ;
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
	}

/*****************************************************************/
// writeToFile
/*****************************************************************/

	void writeToFile(const char *chunk, size_type size, bool isForCgi = false){
		if (size == 0)
			return ;
		if (!_outFile.is_open()){
			std::string ext = getExtension(_fileMimeType);
			if (isForCgi)
				_filePath = "/tmp/" + FileSystem::generateRandomName() + ext;
			else
				_filePath = _uploadFolder + "/" + FileSystem::generateRandomName() + ext;
			_outFile.open(_filePath);
			if (!_outFile.is_open())
				throw std::runtime_error(TRACK_WHERE + "Can't open " + _filePath);
		}
		_outFile.write(chunk, (long)size);
		if (_outFile.bad()) {
			throw std::runtime_error(TRACK_WHERE + "Write operation failed!" + _filePath);
		}
	}

/*****************************************************************/
// receive and write
/*****************************************************************/

	void handleMultiForm(const char *chunk, size_type size){
		if (_multiFormStatus == 0){
			std::string boundary = "--" + _boundary + "\r\n";
			_myBuffer.setUntil(boundary.c_str());
			_multiFormStatus = 1;
		}
		while (size){
			MyBuffer::size_type nbRead = _myBuffer.add(chunk, size);
			chunk += nbRead;
			size -= nbRead;
			if (_myBuffer.isFull()){
				_status = BAD_REQUEST;
				return ;
			}
			if (_myBuffer.isMatch()){
				if (_multiFormStatus == 1){
					if (_myBuffer.getSize() > _myBuffer.getUntilSize()){
						std::string data(_myBuffer.getBuffer(), _myBuffer.getSize() - _myBuffer.getUntilSize());
						writeToFile(data.c_str(), data.size());
					}
					_myBuffer.setUntil("\r\n\r\n");
					_myBuffer.resetBuffer();
					_multiFormStatus++;
				}
				else if (_multiFormStatus == 2){
					std::string miniHeader = std::string(_myBuffer.getBuffer(), _myBuffer.getSize());
					std::string::size_type contentTypeEndPos = miniHeader.find("Content-Type: ") + 14;
					std::string::size_type contentTypeSize = miniHeader.find("\r\n", contentTypeEndPos) - contentTypeEndPos;
					if (contentTypeEndPos != std::string::npos)
						_fileMimeType = miniHeader.substr(contentTypeEndPos, contentTypeSize);
					closeFile();
					std::string boundary = "\r\n--" + _boundary + "\r\n";
					_myBuffer.setUntil(boundary.c_str());
					_myBuffer.resetBuffer();
					_multiFormStatus = 1;
				}
			} else {
				if (_multiFormStatus == 1){
					if (_myBuffer.getSize() > _myBuffer.getMatchCounter()){
						std::string d(_myBuffer.getBuffer(), _myBuffer.getSize());
						std::string::size_type pos = d.find("\r\n--" + _boundary + "--\r\n");
						if (pos != std::string::npos){
							std::string data = d.substr(0, pos);
							writeToFile(data.c_str(), data.size());
							_status = DONE;
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
			if (_clientInfo.isRequestCanHandledByCgi()){
				writeToFile(chunk, size, true);
			} else {
				handleMultiForm(chunk, size);
			}
		}else{
			writeToFile(chunk, size);
		}
		if (_status == DONE)
			closeFile();
	}

	void receiveUtil(const char *chunk, size_type size){
		std::string newChunk;

		if (!_clientInfo.isSet()){
			_status = FORBIDDEN;
			return ;
		}
		if (_isChunked){
			_chunkHandler.getHttpChunkContent(chunk, size, newChunk);
			if (_chunkHandler.is_failed()){
				std::cerr << "_chunkHandler: " << _chunkHandler.getError() << std::endl;
				_status = BAD_REQUEST;
				return ;
			}
			chunk = newChunk.c_str();
			size = newChunk.size();
			_theCGIContentLength += size;
			if (_theCGIContentLength >= _clientInfo.getServerConf().getMaxClientBodySize()){
				_status = BODY_TOO_BEG;
				return ;
			}
			if (_chunkHandler.is_done())
				_status = DONE;
		} else {
			size = std::min(size, _contentLength);
			_contentLength -= size;
			if (_contentLength == 0){
				_status = DONE;
			}
		}
		try{
			writeChunk(chunk, size);
		}catch (std::exception& e){
			_status = FORBIDDEN;
			return ;
		}
	}

	void receive(const char *chunk, size_type size){
		receiveUtil(chunk, size);
		if (_status != NONE){
			if (_status != DONE && _outFile.is_open()){
				_outFile.close();
				FileSystem::removeFile(_filePath.c_str());
			} else
				closeFile();
		}
	}

};


#endif //DUMMY_CLIENT_BODYCHUNK_H
