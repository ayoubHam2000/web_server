/***************************************************/
/*     created by TheWebServerTeam 2/27/23         */
/***************************************************/

#include "ChunckContentHandler.hpp"

bool ChunkContentHandler::is_done() {
	return done;
}

bool ChunkContentHandler::getHttpChunkContent(const char *chunk, unsigned int chunkSize, std::vector<const char *> &arr) {
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

bool ChunkContentHandler::__parseNb() {
	int i = 0;

	while (isHexChar(nbBuffer[i]))
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

bool ChunkContentHandler::_readNb(char c) {
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
