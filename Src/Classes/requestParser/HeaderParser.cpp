/***************************************************/
/*     created by TheWebServerTeam 2/27/23         */
/***************************************************/

# include "requestParser/HeaderParser.hpp"

HeaderParser::HeaderParser() {
	_currPos = 0;
}

HeaderParser::HeaderParser(std::string _request) {
	this ->_request = _request;
	_currPos = 0;
}

bool HeaderParser::isDoneParsing() const {
	return (_currPos >= _request.length());
}

bool HeaderParser::isDelimeter(char c, std::string &delimeter) {
	for (size_t i = 0; i < delimeter.length(); i++)
	{
		if (c == delimeter[i])
			return (true);
	}
	return (false);
}

std::string HeaderParser::getNextToken(std::string &delimeter) {
	std::string nextToken = "";
	while (_currPos < _request.length())
	{
		bool isDel = isDelimeter(_request[_currPos], delimeter);
		if ((_currPos + 1 < _request.length() && _request[_currPos] == '\r' && _request[_currPos + 1] == '\n') \
					|| isspace(_request[_currPos]) || isDel)
		{
			if (nextToken.empty() &&  (isDel))
			{
				nextToken += _request[_currPos];
				_currPos++;
				break;
			}
			if (!nextToken.empty())
				break;
		}
		else
			nextToken += _request[_currPos];
		_currPos++;
	}
	return (nextToken);
}

std::string HeaderParser::getHeaderValue() {
	std::string nextToken = "";
	unsigned int startIdx = _currPos;
	while (_currPos < _request.length())
	{
		if (_request[_currPos] == ':' && _currPos == startIdx)
		{
			_currPos++;
			skipSpaces();
		}
		if (_request[_currPos] == '\n')
			break;
		if ((_currPos + 1 < _request.length() && _request[_currPos] == '\r' && _request[_currPos + 1] == '\n'))
		{
			_currPos++;
			if (!nextToken.empty())
				break;
		}
		else
			nextToken += _request[_currPos];
		_currPos++;
	}
	_currPos++;
	return (nextToken);
}

void HeaderParser::pErrorParsing(const char *msg) const {
	std::cerr << msg << std::endl;
	exit (FAILURE);
}

void HeaderParser::skipSpaces() {
	while (_currPos < _request.length() && _request[_currPos] != '\n' && isspace(_request[_currPos]))
		_currPos++;
}

std::vector<std::string> HeaderParser::getValuesCurrToken(std::string &delimeters, bool &isErrorOccurs) {
	std::vector <std::string> values;
	skipSpaces();
	std::string nextToken = getNextToken(delimeters);
	if (nextToken == ":")
	{
		while (_currPos < _request.length() && _request[_currPos] != '\n')
		{
			skipSpaces();
			if (_request[_currPos] == '\n')
				break;
			nextToken = getNextToken(delimeters);
			if (nextToken.size() > 1 || !isDelimeter(nextToken[0] ,delimeters))
				values.push_back(nextToken);
		}
	}
	else
		isErrorOccurs = true;
	return (values);
}

