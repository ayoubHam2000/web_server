/***************************************************/
/*     created by TheWebServerTeam 2/27/23         */
/***************************************************/

# include "configParser/ConfigParser.hpp"

ConfigParser::ConfigParser() {
	_currPos = 0;
}

ConfigParser::ConfigParser(std::string _serverConfig) {
	this ->_serverConfig = _serverConfig;
	_currPos = 0;
	processComments();
}

void ConfigParser::processComments() {
	size_t i = 0;
	while (i < _serverConfig.length())
	{
		if (_serverConfig[i] == '#')
		{
			while (i < _serverConfig.length() && _serverConfig[i] != '\n')
			{
				_serverConfig[i] = ' ';
				i++;
			}

		}
		i++;
	}
}

std::string ConfigParser::getNextToken() {
	std::string nextToken = "";
	while (_currPos < _serverConfig.length())
	{
		if (isspace(_serverConfig[_currPos]) || _serverConfig[_currPos] == '{' || _serverConfig[_currPos] == '}')
		{
			//std::cout << "_currPos = " << _currPos << " val = " << _serverConfig[_currPos] <<  std::endl;
			if (nextToken.empty() &&  (_serverConfig[_currPos] == '{' || _serverConfig[_currPos] == '}'))
			{
				//std::cout << "here = " << _currPos << " val = " << _serverConfig[_currPos] <<  std::endl;
				nextToken += _serverConfig[_currPos];
				_currPos++;
				break;
			}
			if (!nextToken.empty())
				break;
		}
		else
			nextToken += _serverConfig[_currPos];
		_currPos++;
	}
	return (nextToken);
}

void ConfigParser::skipSpaces() {
	while (_currPos < _serverConfig.length() && _serverConfig[_currPos] != '\n' && isspace(_serverConfig[_currPos]))
		_currPos++;
}

std::vector<std::string> ConfigParser::getValuesCurrToken() {
	std::vector <std::string> values;
	while (_currPos < _serverConfig.length() && _serverConfig[_currPos] != '\n')
	{
		skipSpaces();
		if (_serverConfig[_currPos] == '\n')
			break;
		std::string nextToken = getNextToken();
		if (nextToken == "{" || nextToken == "}")
		{
			_currPos--;
			break;
		}
		values.push_back(nextToken);
	}
	return (values);
}

void ConfigParser::pErrorParsing(const char *msg) const {
	std::cerr << msg << std::endl;
	exit (FAILURE);
}

bool ConfigParser::isInsideServer(std::stack<std::string> &bracketStack) {
	return (bracketStack.size() == 1);
}

std::pair<std::string, int>
ConfigParser::getConfig(std::map<std::string, int> &supportedConfigs, std::string &currConfig) const {
	//std::cout << "currConfig = " << currConfig << std::endl;
	std::map<std::string, int>::iterator it = supportedConfigs.find(currConfig);
	if (it == supportedConfigs.end())
	{
		std::string err  = "Config " + currConfig+   " Not Supported!!";
		pErrorParsing(err.c_str());
	}
	return (std::make_pair(currConfig, it ->second));
}

void ConfigParser::settingConfigsFactory(ServerConfigs &serverConfigs, bool isInsideServer, std::string &ConfigName,
										 std::vector<std::string> &values) {
	if (!isInsideServer)
	{
		std::vector <Location>  &locations =  serverConfigs.getLocations();
		setLocationConfigs(locations[locations.size() - 1], ConfigName, values);
		return ;
	}
	setServerConfigs(serverConfigs, ConfigName, values);

}

void ConfigParser::setLocationConfigs(Location &location, std::string &ConfigName, std::vector<std::string> &values) {
	if (ConfigName == "allow_methods")
	{
		location.setAllowMethods(values);
	}
	else if (ConfigName == "location")
	{
		location.setRoute(values[0]);
	}
	else if (ConfigName == "redirect")
	{
		location.setRedirect(values[0]);
	}
	else if (ConfigName == "autoindex")
	{
		location.setAutoIndex(values[0] == "on");
	}
	else if (ConfigName == "root")
	{
		location.setRoot(values[0]);
	}
	else if (ConfigName == "index")
	{
		location.setIndexes(values);
	}
	else if (ConfigName == "upload_pass")
	{
		location.setUploadPass(values[0]);
	}
	else if (ConfigName == "cgi_pass")
	{
		location.addcgi(values[0], values[1]);
	}
}

unsigned long long ConfigParser::toUnsigendLL(std::string &str) {
	unsigned long long nb;
	try
	{
		nb = std::stoull(str);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit (FAILURE);
	}
	return (nb);
}

unsigned long long ConfigParser::getErrorCode(std::string &ErrorCode) {
	unsigned long long nb = toUnsigendLL(ErrorCode);

	if (nb < 400 || nb >= 600)
	{
		std::cerr << "errorCode not Valid" << '\n';
		exit (FAILURE);
	}
	return (nb);
}

void
ConfigParser::setServerConfigs(ServerConfigs &ServerConfigs, std::string &ConfigName, std::vector<std::string> &values) {
	if (ConfigName == "listen")
	{
		ServerConfigs.setListen(values[0]);
	}
	else if (ConfigName == "server_name")
	{
		ServerConfigs.setServerName(values[0]);
	}
	else if (ConfigName == "max_client_body_size")
	{
		unsigned long long MaxBodySize = toUnsigendLL(values[0]);
		if (MaxBodySize < 0)
		{
			std::cerr << "Max Body size not Valid" << '\n';
			exit (FAILURE);
		}
		ServerConfigs.size_limit_in();
		ServerConfigs.setMaxClientBodySize(MaxBodySize);
	}
	else if (ConfigName == "error_page")
	{
		ServerConfigs.addErrorPage(getErrorCode(values[0]), values[1]);
	}
}

std::map<std::string, ServerMap > ConfigParser::parseServerConfig() {
	std::stack <std::string> bracketStack;
	std::map<std::string, ServerMap > servers;
	Server server;
	while (_currPos < _serverConfig.length() - 1)
	{
		std::string nextToken = getNextToken();
		if (bracketStack.empty())
		{
			if (nextToken.empty())
			{
				if (servers.empty())
					pErrorParsing("File is Empty!");
				else
					continue;
			}
			if (nextToken == "server")
			{

				nextToken = getNextToken();
				if (nextToken != OPEN_BRACKET)
					pErrorParsing("Parsing Error");
				bracketStack.push(OPEN_BRACKET);
				server = Server();
			}
			else
				pErrorParsing("Parsing Error");
		}
		else
		{
			if (nextToken == CLOSE_BRACKET)
			{
				if (bracketStack.empty())
					pErrorParsing("Bracket Error");
				bracketStack.pop();
				if (bracketStack.empty())
				{
					//add server if its valid
					ServerConfigs &currServerConfigs = server.getServerConfigs();
					if (!currServerConfigs.isValidConfigs())
						pErrorParsing("Server Configuration Not Valid!");
					if (servers.find(currServerConfigs.getHost()+ currServerConfigs.getServ()) == servers.end())
					{
						ServerMap serverMap;
						serverMap[currServerConfigs.getServerName()] = server;
						servers[currServerConfigs.getHost()+ currServerConfigs.getServ()] = serverMap;
					}
					else
					{
						ServerMap &serverMap = servers[currServerConfigs.getHost()+ currServerConfigs.getServ()];
						if (serverMap.find(currServerConfigs.getServerName()) != serverMap.end())
							pErrorParsing("this Server Name already exist in this listen!!");
						serverMap[currServerConfigs.getServerName()] = server;
						servers[currServerConfigs.getHost()+ currServerConfigs.getServ()] = serverMap;
					}
				}
				continue;
			}
			std::map <std::string, int > supportedConfigs = StaticConfig::SERVER_CONFIGS[bracketStack.size() - 1];
			std::pair <std::string, int > currConfig = getConfig(supportedConfigs, nextToken);
			std::vector <std::string> values = getValuesCurrToken();
			if (values.size() == 0 || (currConfig.second != 0 &&  (int)values.size() != currConfig.second ))
				pErrorParsing("values Config not Valid !!");
			if (nextToken == "location")
			{
				if (getNextToken() != OPEN_BRACKET || bracketStack.size() >= 2)
					pErrorParsing("Parsing Error");
				bracketStack.push(OPEN_BRACKET);
				server.getServerConfigs().getLocations().push_back(Location());
			}
			settingConfigsFactory(server.getServerConfigs(), isInsideServer(bracketStack), \
											nextToken, values);
		}
	}
	if (bracketStack.size())
		pErrorParsing("Bracket Error");
	//PrintServers(servers);
	return (servers);
}
