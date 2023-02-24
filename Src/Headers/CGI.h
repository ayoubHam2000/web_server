/***************************************************/
/*     created by TheWebServerTeam 2/24/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_CGI_H
#define DUMMY_CLIENT_CGI_H

# include <Libraries.h>
# include <ClientInfo.h>

class CGI{
public:
	typedef std::map<std::string, std::string> meta_var_type;
private:
	ClientInfo*		_clientInfo;
	int 			_status;
	meta_var_type	_metaVars;
private:
	CGI(const CGI& other);
	CGI& operator=(const CGI& other);
public:

	CGI():
		_clientInfo(NULL),
		_status(0),
		_metaVars()
	{
	}

	~CGI(){}

public:

	static std::string toEnvHeader(const std::string& header){
		std::string res(header);
		for (std::string::size_type i = 0; i < header.size(); i++){
			if (!(std::isalnum(header[i]) || header[i] == '-'))
				return ("");
			if (header[i] == '-')
				res[i] = '_';
		}
		return (res);
	}

	void prepare(ClientInfo* clientInfo){
		_clientInfo = clientInfo;

		_metaVars["SERVER_PROTOCOL"] = clientInfo->getHeader().getProtocol();
		_metaVars["SERVER_PORT"] = clientInfo->getServerConf().getService();
		_metaVars["REQUEST_METHOD"] = clientInfo->getHeader().getRequestType();
		_metaVars["PATH_INFO"] = clientInfo->getRequestedPath();
		_metaVars["PATH_TRANSLATED"] = clientInfo->geReqFileFullPath();
		_metaVars["SCRIPT_NAME"] = clientInfo->getCGIPath();
		_metaVars["QUERY_STRING"] = clientInfo->getHeader().getPath().getParams();
		_metaVars["REMOTE_ADDR"] = clientInfo->getIpAddress();
		_metaVars["REMOTE_PORT"] = clientInfo->getService();
		if (clientInfo->getHeader().has("Content-type"))
			_metaVars["CONTENT_TYPE"] = clientInfo->getHeader().valueOf("Content-type");
		if (clientInfo->getHeader().has("Content-length"))
			_metaVars["CONTENT_LENGTH"] = clientInfo->getContentLength();

		for (std::map<std::string, std::string>::const_iterator iter = clientInfo->getHeader().getHttpHeaders().cbegin();
			 iter != clientInfo->getHeader().getHttpHeaders().cend(); ++iter){
			std::string newHeaderForm = toEnvHeader(iter->first);
			if (!newHeaderForm.empty())
				_metaVars[newHeaderForm] = iter->second;
		}

	}

	char** getEnv(){
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
		return (env);
	}


	bool execCGI(const char *filePath, const char *cgiPath, int fd, int outfd){
		const char* args[3] = {filePath, filePath, NULL};

		int pid = fork();
		if (pid == -1)
			return (false);
		if (!pid){
			char* const * env = getEnv();
			dup2(fd, 0);
			dup2(outfd, 1);
			if (execve(args[0], args, env) == -1){
				return (2);
			}
		}
		int status;
		if (waitpid(pid, &status, 0) != -1){
			if (WIFEXITED(status)){
				return (WEXITSTATUS(status) == 0);
			} else if (WIFSIGNALED(status)){
				return (false);
			}
		}
		return (false);
	}

	bool execCGI(){
		std::string cgiPath = _clientInfo->getCGIPath();
		execCGI(cgiPath.c_str())
	}
};


#endif //DUMMY_CLIENT_CGI_H
