/***************************************************/
/*     created by TheWebServerTeam 2/20/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_BODYCHUNK_H
#define DUMMY_CLIENT_BODYCHUNK_H


#include <Libraries.h>
#include <Header.h>
#include <ServerConfig.h>
#include <ChunkContentHandler.hpp>

class BodyChunk {
private:
	bool 				_isPrepared;
	bool 				_isChunked;
	bool 				_isMultiFormData;
	bool 				_isDone;
	bool 				_fileIsOpen;
	std::string 		_boundary;
	std::string			_uploadFolder;
	size_type 			_contentLength;
	ChunkContentHandler _chunkHandler;
public:
	enum {
		SUCCESS = 0, ERROR = -1, BODY_TOO_BIG = 1
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
		_boundary()
	{}
	~BodyChunk(){}
public:

	bool isIsDone() const {
		return _isDone;
	}

public:

	std::string _generateRandomName(){
		const int	length = 20;
		const int	buffer_size = 500;
		char		name[length + 1];
		char		buffer[buffer_size];
		int			i = 0;

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
		return (std::string(name));
	}

	void setUploadFolder(const LocationConfig& locationConf){
		_uploadFolder = locationConf.getRootFolder() + locationConf.getUploadFolder();
	}

	int prepare(const LocationConfig& locationConf, const ServerConfig& serverConfig, const Header& header){
		setUploadFolder(locationConf);
		if (header.has("Content-Length")){
			try{
				_contentLength = std::stoul(header.valueOf("Content-Length"));
				if (_contentLength > serverConfig.getMaxClientBodySize())
					return (BODY_TOO_BIG);
			}catch (...){
				return (ERROR);
			}
		} else if (header.has("Transfer-Encoding") && header.valueOf("Transfer-Encoding") == "chunk"){
			_isChunked = true;
		}
		if (header.has("Content-Type") && header.valueOf("Content-Type").find("multipart/form-data") != std::string::npos){
			_isMultiFormData = true;
			_boundary = header.valueOf("Content-Type").substr(header.valueOf("Content-Type").find("boundary=") + 9);
		}
		_isPrepared = true;
		return (SUCCESS);
	}

	int receive(const char *chunk, size_type size){
		static std::ofstream outFile;

		if (!_isPrepared)
			return (ERROR);
		if (_isChunked){
			std::vector<const char *> arr;
			std::stringstream ss;
			_chunkHandler.getHttpChunkContent(chunk, size, arr);
			if (_chunkHandler.is_done())
				_isDone = true;
			for (int i = 0; i < arr.size(); i += 2){
				ss << std::string(arr[0], arr[1]);
			}
			chunk = ss.str().c_str();
			size = ss.str().size();
		} else {
			size = std::min(size, _contentLength);
			_contentLength -= size;
			if (_contentLength == 0)
				_isDone = true;
		}
		if (!_fileIsOpen){
			_fileIsOpen = true;
			std::string filePath = _uploadFolder + "/" + _generateRandomName();
			outFile.open(_uploadFolder + "/" + _generateRandomName());
			if (!outFile.is_open())
				throw std::runtime_error("Can't open " + filePath);
		}
		outFile.write(chunk, size);
		return (SUCCESS);
	}

};


#endif //DUMMY_CLIENT_BODYCHUNK_H
