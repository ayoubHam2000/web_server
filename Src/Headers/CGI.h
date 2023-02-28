/***************************************************/
/*     created by TheWebServerTeam 2/24/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_CGI_H
#define DUMMY_CLIENT_CGI_H

# include <Libraries.h>
# include <ClientInfo.h>

class CGI{
public:
	enum {
		NONE = 0, DONE = 1, ERROR = -1
	};
public:
	typedef std::map<std::string, std::string> meta_var_type;
private:
	ClientInfo*		_clientInfo;
	int 			_status;
	meta_var_type	_metaVars;
	int 			_CGIPid;
	int 			_bodyFd;
	std::string 	_outFilePath;
	int				_outFileFd;
private:
	CGI(const CGI& other);
	CGI& operator=(const CGI& other);
public:

	CGI():
		_clientInfo(NULL),
		_status(0),
		_metaVars(),
		_CGIPid(-1)
	{
	}

	~CGI(){}

public:

	bool	isPrepared(){
		return (_clientInfo != NULL);
	}

	int getStatus() const {
		return _status;
	}

	int getCgiPid() const {
		return _CGIPid;
	}

	const std::string &getOutFilePath() const {
		return _outFilePath;
	}


private:
	static std::string toEnvHeader(const std::string& header){
		std::string res(header);
		for (std::string::size_type i = 0; i < header.size(); i++){
			if (!(std::isalnum(header[i]) || header[i] == '-'))
				return ("");
			if (header[i] == '-')
				res[i] = '_';
		}
		return ("HTTP_" + res);
	}

public:
	void prepare(ClientInfo* clientInfo){
		_clientInfo = clientInfo;

		if (access(_clientInfo->getCGIPath().c_str(), F_OK | X_OK) != 0)
			throw std::runtime_error(TRACK_WHERE + " CGI not found or not an executable");
		_metaVars["SERVER_PROTOCOL"] = clientInfo->getHeader().getProtocol();
		_metaVars["SERVER_PORT"] = clientInfo->getServerConf().getService();
		_metaVars["REQUEST_METHOD"] = clientInfo->getHeader().getRequestType();
		_metaVars["PATH_INFO"] = clientInfo->getRequestedPath();
		_metaVars["SCRIPT_FILENAME"] = clientInfo->geReqFileFullPath();
		_metaVars["SCRIPT_NAME"] = clientInfo->getCGIPath();
		_metaVars["QUERY_STRING"] = clientInfo->getHeader().getPath().getParams();
		_metaVars["REMOTE_ADDR"] = clientInfo->getIpAddress();
		_metaVars["REMOTE_PORT"] = clientInfo->getService();
		_metaVars["REDIRECT_STATUS"] = "200";
		if (clientInfo->getHeader().has("Content-type"))
			_metaVars["CONTENT_TYPE"] = clientInfo->getHeader().valueOf("Content-type");
		if (clientInfo->getHeader().has("Content-length"))
			_metaVars["CONTENT_LENGTH"] = std::to_string(clientInfo->getContentLength());

		for (std::map<std::string, std::string>::const_iterator iter = clientInfo->getHeader().getHttpHeaders().cbegin();
			 iter != clientInfo->getHeader().getHttpHeaders().cend(); ++iter){
			std::string newHeaderForm = toEnvHeader(iter->first);
			if (!newHeaderForm.empty())
				_metaVars[newHeaderForm] = iter->second;
		}
		_execCGI();
	}

	void read(){
		int status;
		int pid = waitpid(getCgiPid(), &status, WNOHANG);
		if (pid == -1){
			_status = ERROR;
		} else if (pid != 0){
			if (WIFSIGNALED(status)){
				_status = ERROR;
			} else {
				_status = DONE;
			}
		}
		if (_status == DONE || _status == ERROR){
			close(_outFileFd);
			close(_bodyFd);
		}
	}

private:

	void _execCGI(char **args, char **env, int bodyFd){
		_CGIPid = fork();
		if (_CGIPid == -1)
			throw std::runtime_error("fork Failed");
		if (!_CGIPid){
			dup2(bodyFd, 0);
			dup2(_outFileFd, 1);
			if (execve(args[0], args, env) == -1){
				exit(1);
			}
		}
	}

	void _execCGI(){
		std::string cgiPath = _clientInfo->getCGIPath();
		std::string filePath = _clientInfo->getReqFileRelativePath();

		//args
		char**	args;
		args = new char*[3];
		args[0] = new char[cgiPath.size() + 1];
		std::strcpy(args[0], cgiPath.c_str());
		args[1] = new char[filePath.size() + 1];
		std::strcpy(args[1], filePath.c_str());
		args[2] = NULL;

		//env
		char **env = new char*[_metaVars.size() + 1];
		int i = 0;
		for (meta_var_type::iterator iter = _metaVars.begin(); iter != _metaVars.end(); ++iter){
			env[i] = new char[iter->first.size() + iter->second.size() + 2];
			*(env[i]) = 0;
			std::strcat(env[i], iter->first.c_str());
			std::strcat(env[i], "=");
			std::strcat(env[i], iter->second.c_str());
			i++;
		}
		env[i] = NULL;

		//cgi
		if (!_clientInfo->getCreatedFile().empty()){
			_bodyFd = open(_clientInfo->getCreatedFile().c_str(), O_RDONLY);
			if (_bodyFd == -1){
				std::runtime_error("CGI Open Failed");
			}
		} else {
			std::string randomPath = "/tmp/" + FileSystem::generateRandomName() + ".txt";
			_bodyFd = open(randomPath.c_str(), O_RDONLY | O_CREAT | O_TRUNC, 0666);
		}
		_outFilePath = "/tmp/" + FileSystem::generateRandomName() + ".html";
		_outFileFd = open(_outFilePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (_outFileFd == -1){
			std::runtime_error("CGI Open Failed");
		}
		_execCGI(args, env, _bodyFd);

		//free memory
		i = 0;
		while (env[i]){
			delete env[i];
			i++;
		}
		i = 0;
		while (args[i]){
			delete args[i];
			i++;
		}
		delete[] args;
		delete[] env;
	}



public:

	static int test1(){

		int pipefd[2];
		char buffer[1024];
		const char* data = "Hello, world!\0";

		if (pipe(pipefd) == -1) {
			std::cerr << "Failed to create pipe: " << strerror(errno) << std::endl;
			return 1;
		}

		int flags = fcntl(pipefd[0], F_GETFL, 0);
		fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

		int child = fork();
		if (!child){
			std::cout << "Start Child" << std::endl;
			usleep(10000000);
			std::cout << "Child Writing" << std::endl;
			close(pipefd[0]);
			write(pipefd[1], data, strlen(data));
			usleep(1000000);
			write(pipefd[1], data, strlen(data));
			close(pipefd[1]);
			std::cout << "End Child" << std::endl;
			exit(0);
		}

		close(pipefd[1]);
		std::string receivedData;
		while (1){
			int bytes_read = ::read(pipefd[0], buffer, 1024);
			if (bytes_read == 0)
				break;
			if (bytes_read == -1) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					std::cerr << "Waiting..." << std::endl;
				} else {
					break ;
					std::cerr << "Read error: " << strerror(errno) << std::endl;
				}
			} else {

				receivedData += std::string(buffer, bytes_read);
				//std::cout << "Read " << bytes_read << " bytes: " << std::string(buffer, bytes_read) << std::endl;
			}
		}
		close(pipefd[0]);
		std::cout << "End Parent" << std::endl;
		std::cout << receivedData << std::endl;
		return (0);
	}

	static void test2(){
		/*char *args[3] = {
				"../cgi_bin/php-cgi\0",
				"dfs",
				NULL
		};
		char *env[5] = {
				"REQUEST_METHOD=GET\0",
				"PATH_INFO=../public/main.php\0",
				"SCRIPT_FILENAME=//Users/aben-ham/.1337/projects/web_server/../public/main.php\0",
				"REDIRECT_STATUS=200",
				NULL
		};

		int _CGIPid = fork();
		if (_CGIPid == -1)
			throw std::runtime_error("fork Failed");
		if (!_CGIPid){
			//dup2(bodyFd, 0);
			if (execve(args[0], args, env) == -1){
				exit(15);
			}
		}
		int status;
		std::cout << waitpid(_CGIPid, &status, 0) << std::endl;
		std::cout << WEXITSTATUS(status) << std::endl;
		std::cout << access(args[0], X_OK) << std::endl;*/
	}

};


#endif //DUMMY_CLIENT_CGI_H
