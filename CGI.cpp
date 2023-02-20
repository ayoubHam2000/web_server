/***************************************************/
/*     created by TheWebServerTeam 2/16/23         */
/***************************************************/


#include "Libraries.h"
#include "testFolder/A_Request.hpp"

/*
meta-variable-name = "AUTH_TYPE" | "CONTENT_LENGTH" |
                           "CONTENT_TYPE" | "GATEWAY_INTERFACE" |
                           "PATH_INFO" | "PATH_TRANSLATED" |
                           "QUERY_STRING" | "REMOTE_ADDR" |
                           "REMOTE_HOST" | "REMOTE_IDENT" |
                           "REMOTE_USER" | "REQUEST_METHOD" |
                           "SCRIPT_NAME" | "SERVER_NAME" |
                           "SERVER_PORT" | "SERVER_PROTOCOL" |
                           "SERVER_SOFTWARE"
 * */

class CGI{
public:
	typedef std::map<std::string, std::string> meta_var_type;
private:
	char* CGIPath;
private:
	CGI(const CGI& other);
	CGI& operator=(const CGI& other);
public:

	CGI(char* &CGIPath) : CGIPath(CGIPath){
		if (access(CGIPath, F_OK | X_OK) != 0)
			throw std::runtime_error("Can't execute cgi or cgi not found");
	}

	~CGI(){}
public:

	char** getEnv(meta_var_type &metaVars){
		char **env = new char*[metaVars.size()];
		int i = 0;
		for (meta_var_type::iterator iter = metaVars.begin(); iter != metaVars.end(); ++iter){
			env[i] = new char[iter->first.size() + iter->second.size() + 2];
			*(env[i]) = 0;
			std::strcat(env[i], iter->first.c_str());
			std::strcat(env[i], "=");
			std::strcat(env[i], iter->second.c_str());
			i++;
		}
		return (env);
	}

	void getMetaVars(meta_var_type &metaVars, meta_var_type &headers){
		metaVars["SERVER_PROTOCOL"] = headers["protocol"];
		metaVars["SERVER_PORT"] = headers["port"]; //decimal
		metaVars["REQUEST_METHOD"] = headers["method"];
		metaVars["PATH_INFO"] = headers["path"]; //requested path
		metaVars["PATH_TRANSLATED"] = headers["fullPath"];
		metaVars["SCRIPT_NAME"] = headers["cgi_path"];
		metaVars["QUERY_STRING"] = headers["query"];
		//metaVars["REMOTE_HOST"] = headers["asd"];//host name of the client, unset if server did not perform such lookup.
		metaVars["REMOTE_ADDR"] = headers[""];//IP address of the client (dot-decimal).
		metaVars["REMOTE_PORT"] = headers[""];
		metaVars["CONTENT_TYPE"] = headers[""];
		metaVars["CONTENT_LENGTH"] = headers[""];

		//metaVars["AUTH_TYPE"] = headers[""];
		//metaVars["REMOTE_USER"] = headers[""];
		//metaVars["REMOTE_IDENT"] = headers[""];

	}

	bool execCGI(char *filePath, std::map<std::string, std::string> &headers, int fd, int outfd){
		char* args[3] = {CGIPath, filePath, NULL};

		int pid = fork();
		if (pid == -1)
			return (false);
		if (!pid){
			char** env = getEnv(headers);
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
};