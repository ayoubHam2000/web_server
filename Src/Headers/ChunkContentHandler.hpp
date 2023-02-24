/***************************************************/
/*     created by TheWebServerTeam 2/14/23         */
/***************************************************/

#ifndef CHUNCKED_
#define CHUNCKED_

# include "Libraries.h"
# include <MyBuffer.h>

class ChunkContentHandler{
public:
	enum {
		DONE, NONE, ERROR = -1, ERR_BUFFER_FULL = -2, ERR_BAD_NUMBER = -3
	};
private:
	static const unsigned int	maxBuffer = 20;
	unsigned long 				_expectedContentSize;
	int							_status;
	MyBuffer					_nbBuffer;
	int 						_endLine;

private:
	ChunkContentHandler(const ChunkContentHandler& other);
	ChunkContentHandler& operator=(const ChunkContentHandler& other);
public:

	ChunkContentHandler():
			_expectedContentSize(0),
			_status(NONE),
			_nbBuffer(20, "\r\n\0"),
			_endLine(0)
	{}

	~ChunkContentHandler(){}

public:
	bool is_done()
	{
		return (_status == DONE);
	}

	bool is_failed()
	{
		return (_status < 0);
	}

	std::string getError(){
		if (_status == ERR_BUFFER_FULL){
			return std::string(_nbBuffer.getBuffer(), _nbBuffer.getSize()) + "->Buffer Is Full";
		} else if (_status == ERR_BAD_NUMBER)
			return "Bad Number";
		return "Error";
	}

	void	getHttpChunkContent(const char *chunk, size_type chunkSize, std::string& res){
		size_t 		nbRead;
		char 		buffer[chunkSize];
		char 		*ptr;

		ptr = buffer;
		while (chunkSize > 0){
			if (_expectedContentSize == 0){
				if (_endLine){
					nbRead = 1;
					_endLine--;
				} else {
					nbRead = _nbBuffer.add(chunk, chunkSize);
					if (_nbBuffer.isFull()){
						_status = ERR_BUFFER_FULL;
						return ;
					}
					if (_nbBuffer.isMatch()){
						if (_parseNb()){
							_nbBuffer.fullReset();
							_endLine = 2;
							if (_expectedContentSize == 0){
								_status = DONE;
								return ;
							}
						} else {
							_status = ERR_BAD_NUMBER;
							return ;
						}
					}
				}
			}
			else {
				nbRead = std::min(chunkSize, _expectedContentSize);
				_expectedContentSize -= nbRead;
				std::memcpy(ptr, chunk, nbRead);
				ptr += nbRead;
			}
			chunk += nbRead;
			chunkSize -= nbRead;
		}
		if (buffer != ptr)
			res = std::string(buffer, ptr);
	}

public:

	//TODO: remove (just for test)
	//file should contain CRLF
	static void testFunction(const std::string& filePath){
		std::string content;
		std::ifstream file (filePath);
		char buffer[1001];
		ChunkContentHandler clientChunk;
		//int chunkedSize[] = {20};
		int chunkedSize[] = {5, 997, 3};
		int nbChunks = sizeof(chunkedSize) / sizeof(int);
		int counter = 0;
		while (counter < nbChunks){
			file.read(buffer, chunkedSize[counter]);
			std::string res;
			clientChunk.getHttpChunkContent(buffer, file.gcount(), res);
			std::cout << "Done: " << (clientChunk.is_done() ? "true" : "false") << std::endl;
			std::cout << "Failed: " << (clientChunk.is_failed() ? "true" : "false") << std::endl;
			if (clientChunk.is_failed() || clientChunk.is_done())
				break;
			content.append(res);
			counter++;
		}
		std::cout << "Content: " << std::endl;
		std::cout << content << std::endl;
	}

private:

	bool _parseNb(){
		const char	*nbBuffer = _nbBuffer.getBuffer();
		int 		i = 0;

		while (isHexChar(nbBuffer[i]))
			i++;
		if (nbBuffer[i] != '\r' || nbBuffer[i + 1] != '\n')
		{
			return (false);
		}
		try{
			_expectedContentSize = std::stoul(nbBuffer, NULL, 16);
		}catch (std::exception &e){
			return (false);
		}
		return (true);
	}
};

#endif