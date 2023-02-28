/***************************************************/
/*     created by TheWebServerTeam 2/26/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_CHUNK_H
#define DUMMY_CLIENT_CHUNK_H

#include <unistd.h>

class Chunk {
private:
	char	*_chunk;
	size_t	_size;
	size_t	_nbRead;
public:
	explicit Chunk(char *chunk = NULL, size_t size = 0): _chunk(chunk), _size(size), _nbRead(0){

	}

	~Chunk(){}

	Chunk(const Chunk& other): _chunk(other._chunk), _size(other._size), _nbRead(other._nbRead){

	}

	Chunk& operator=(const Chunk& other){
		_chunk = other._chunk;
		_size = other._size;
		_nbRead = other._nbRead;
		return (*this);
	}

public:

	char *getChunk() const {
		return _chunk;
	}

	size_t getSize() const {
		return _size;
	}

	size_t getNbRead() const {
		return _nbRead;
	}

	//Setters

	void setChunk(char *chunk, size_t size) {
		_chunk = chunk;
		_size = size;
		_nbRead = 0;
	}

public:

	Chunk& operator++(){
		++_chunk;
		++_nbRead;
		--_size;
		return (*this);
	}

	Chunk& operator--(){
		--_chunk;
		--_nbRead;
		++_size;
		return (*this);
	}

	Chunk operator++(int){
		Chunk tmp(*this);
		++_chunk;
		++_nbRead;
		--_size;
		return (tmp);
	}

	Chunk operator--(int){
		Chunk tmp(*this);
		--_chunk;
		--_nbRead;
		++_size;
		return (tmp);
	}

	Chunk& operator+=(size_t nb){
		_chunk += nb;
		_nbRead += nb;
		_size -= nb;
		return (*this);
	}

	Chunk& operator-=(size_t nb){
		_chunk -= nb;
		_nbRead -= nb;
		_size += nb;
		return (*this);
	}
};


#endif //DUMMY_CLIENT_CHUNK_H
