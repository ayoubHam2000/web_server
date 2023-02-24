/***************************************************/
/*     created by TheWebServerTeam 2/21/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_MYBUFFER_H
#define DUMMY_CLIENT_MYBUFFER_H

#include <Libraries.h>

class MyBuffer {
public:
	typedef unsigned int size_type;
private:
	char*						_buffer;
	size_type 					_counter;
	size_type					_buffer_size;
	char*						_until;
	size_type 					_until_size;
	size_type 					_matchCounter;
public:
	MyBuffer(const size_type size = 100, const char *until = NULL)
	: _buffer_size(size),
	_buffer(new char[size]),
	_until(NULL),
	_counter(0),
	_matchCounter(0),
	_until_size(0)
	{
		setUntil(until);
	}

	~MyBuffer(){
		delete[] _buffer;
		if (_until){
			delete[] _until;
		}
	}

	MyBuffer (const MyBuffer& other):
			_buffer_size(other._buffer_size),
			_buffer(new char[other._buffer_size]),
			_until(NULL),
			_until_size(other._until_size),
			_matchCounter(other._matchCounter),
			_counter(other._counter)
	{
		std::memcpy(_buffer, other._buffer, _buffer_size);
		setUntil(other._until);
	}

	MyBuffer& operator=(const MyBuffer& other){
		char *tmp = new char[other._buffer_size];
		std::memcpy(tmp, other._buffer, other._buffer_size);
		delete[] _buffer;
		_buffer_size = other._buffer_size;
		_until_size = other._until_size;
		setUntil(other._until);
		_buffer = tmp;
		_matchCounter = other._matchCounter;
		_counter = other._counter;
		return (*this);
	}
public:

	char *getBuffer() const {
		return _buffer;
	}

	size_type getSize() const {
		return _counter;
	}

	size_type getUntilSize() const {
		return _until_size;
	}

	size_type getMatchCounter() const {
		return _matchCounter;
	}

	bool 	isUntilSet(){
		return _until != NULL;
	}

	bool	isMatch(){
		return (_until_size == _matchCounter);
	}

	bool	isFull(){
		return (_buffer_size == _counter);
	}


	//----

	void setUntil(const char *until) {
		char* tmp = _until;
		if (until){
			_until_size = std::strlen(until);
			tmp = new char[_until_size + 1];
			std::strcpy(tmp, until);
			if (_until)
				delete[] _until;
			_matchCounter = 0;
		}
		_until = tmp;
	}

public:

	size_type add(const char* chunk, size_type chunkSize){
		size_type counter = 0;

		if (_matchCounter == _until_size)
			return (counter);
		while (counter < chunkSize){
			if (_counter >= _buffer_size)
				return (counter);
			_buffer[_counter++] = *chunk;
			counter++;
			if (_until[_matchCounter] == *chunk){
				_matchCounter++;
			} else {
				_matchCounter = 0;
				if (*chunk == _until[_matchCounter])
					_matchCounter++;
			}
			if (_matchCounter == _until_size)
				return (counter);
			chunk++;
		}
		return (counter);
	}

	void shift(){
		_counter = 0;
		while (_counter < _matchCounter){
			_buffer[_counter] = _until[_counter];
			_counter++;
		}
	}

	void resetBuffer(){
		_counter = 0;
	}

	void resetUntil(){
		_matchCounter = 0;
	}

	void fullReset(){
		resetBuffer();
		resetUntil();
	}

public:

	static void test(){
		MyBuffer::size_type nbRead;

		std::string until ("abc\0");
		MyBuffer buffer(100, until.c_str());

		nbRead = buffer.add("bc1238", 6);
		std::cout << nbRead << std::endl;
		nbRead = buffer.add(std::string(91, 'd').c_str(), 91);
		std::cout << nbRead << std::endl;
		nbRead = buffer.add("abc", 3);
		std::cout << nbRead << std::endl;

		//buffer.setUntil("new\0");

		//nbRead = buffer.add("ttynew7", 6);
		//std::cout << nbRead << std::endl;

		std::cout << "isFull " << buffer.isFull() << std::endl;
		std::cout << "isMatch " << buffer.isMatch() << std::endl;
		std::cout << "Size " << buffer.getSize() << std::endl;
		std::cout << "str: " << std::string(buffer.getBuffer(), buffer.getSize()) << std::endl;
	}

};


#endif //DUMMY_CLIENT_MYBUFFER_H
