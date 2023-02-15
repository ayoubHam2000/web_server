/***************************************************/
/*     created by TheWebServerTeam 2/14/23         */
/***************************************************/

#ifndef CHUNCKED_
#define CHUNCKED_

# include <iostream>
# include <map>
# include <vector>
# include <set>
# include <string>
# include <fstream>
# include <exception>
# include <stack>
# include <sstream>
# include <algorithm>


class ChunkContentHandler{
private:
	static const unsigned int	maxBuffer = 20;
	unsigned long 				expectedContentSize;
	char						nbBuffer[maxBuffer];
	unsigned int				nbBufferCounter;
	short 						contentEnd;
	bool						done;

private:
	ChunkContentHandler(const ChunkContentHandler& other);
	ChunkContentHandler& operator=(const ChunkContentHandler& other);
public:

	ChunkContentHandler(){
		expectedContentSize = 0;
		std::memset(nbBuffer, 0, maxBuffer);
		nbBufferCounter = 0;
		contentEnd = 0;
		done = false;
	}

	~ChunkContentHandler(){}

public:
	bool is_done()
	{
		return done;
	}

	/**
 	** @brief supply the @param arr with a list of {start_ptr (included) and end_ptr (excluded)} of the chunked content.
	 * @return true if success else false in case of bad hex_number (big than unsigned int or malformed)
 	*/
	bool getHttpChunkContent(const char *chunk, unsigned int chunkSize, std::vector<const char *> &arr){
		if (chunkSize == 0) {
			return (true);
		}
		while (chunkSize--){
			if (expectedContentSize == 0)
			{
				if (arr.size() % 2 == 1){
					arr.push_back(chunk);
				}
				if (!_readNb(*chunk))
					return (false);
				if (done)
					break;
			}
			else
			{
				if (arr.size() % 2 == 0)
					arr.push_back(chunk);
				expectedContentSize--;
			}
			chunk++;
		}
		if (arr.size() % 2 == 1){
			arr.push_back(chunk);
		}
		return (true);
	}

public:

	//TODO: remove (just for test)
	//file should contain CRLF
	static void testFunction(const std::string& filePath){
		std::vector<std::string> contentResult;
		std::ifstream file (filePath);
		char buffer[1001];
		ChunkContentHandler clientChunk;
		//int chunkedSize[] = {20};
		int chunkedSize[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
		int nbChunks = sizeof(chunkedSize) / sizeof(int);
		int counter = 0;
		while (counter < nbChunks){
			file.read(buffer, chunkedSize[counter]);
			std::vector<const char *> res;
			bool clientResult = clientChunk.getHttpChunkContent(buffer, file.gcount(), res);
			std::cout << (clientResult ? "true" : "false") << std::endl;
			if (!clientResult)
				break;
			int i = 0;
			while (i < res.size()){
				contentResult.push_back(std::string(res[i], res[i + 1]));
				i+=2;
			}
			std::cout << "Is Done: " << (clientChunk.is_done() ? "true" : "false") << std::endl;
			if (clientChunk.is_done())
				break;
			counter++;
		}
		std::cout << "Content: " << std::endl;
		for (std::vector<std::string>::iterator iter = contentResult.begin(); iter != contentResult.end(); iter++){
			std::cout << *iter;
		}
	}

private:

	bool __parseNb(){
		int i = 0;

		while (_isHexChar(nbBuffer[i]))
			i++;
		if (nbBuffer[i] != '\r' || nbBuffer[i + 1] != '\n')
		{
			return (false);
		}
		nbBuffer[i] = 0;
		try{
			expectedContentSize = std::stoul(nbBuffer, NULL, 16);
		}catch (std::exception &e){
			return (false);
		}
		return (true);
	}

	bool _readNb(char c){
		if ((contentEnd == 2 && c != '\r') || (contentEnd == 1 && c != '\n'))
			return (false);
		if (contentEnd != 0){
			contentEnd--;
			return (true);
		}
		if (nbBufferCounter == maxBuffer){
			return (false);
		}
		nbBuffer[nbBufferCounter++] = c;
		if (nbBufferCounter >= 2 && !std::strncmp(nbBuffer + (nbBufferCounter - 2), "\r\n", 2)){
			if (!__parseNb())
				return (false);
			else{
				std::memset(nbBuffer, 0, maxBuffer);
				nbBufferCounter = 0;
				contentEnd = 2;
			}
			if (expectedContentSize == 0){
				done = true;
			}
		}
		return (true);
	}


	bool _isHexChar(char c){
		if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'))
			return (true);
		return (false);
	}

};

#endif