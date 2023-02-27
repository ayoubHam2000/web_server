/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 15:58:42 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/26 17:14:08 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI():
	_clientInfos(0),
	_status(0),
	_metaVars(),
	_CGIPid(-1)
{
}

CGI::~CGI(){}

int CGI::getStatus() const {
	return _status;
}

int CGI::getCgiPid() const {
	return _CGIPid;
}

const std::string &CGI::getOutFilePath() const {
	return _outFilePath;
}

std::string  CGI::toEnvHeader(const std::string& header){
	std::string res(header);
	for (std::string::size_type i = 0; i < header.size(); i++){
		if (!(std::isalnum(header[i]) || header[i] == '-'))
			return ("");
		if (header[i] == '-')
			res[i] = '_';
	}
	std::transform(res.begin(), res.end(), res.begin(), toupper);
	return ("HTTP_" + res);
}

std::string CGI::_generateRandomPath(){
	const int	length = 7;
	const int	buffer_size = 100;
	char		name[length + 1];
	char		buffer[buffer_size];
	int			i = 0;
	std::time_t time = std::time(NULL);

	std::ifstream file("/dev/random");
	if (!file.is_open())
		throw std::runtime_error("Error can't open /dev/random");
	file.read(buffer, buffer_size);
	for (int j = 0; j < buffer_size; j++){
		if (std::isalnum(buffer[j]))
			name[i++] = buffer[j];
		if (i == length){
			name[i] = 0;
			break ;
		}
	}
	std::stringstream ss;
	ss << std::put_time(std::gmtime(&time), "%d%m%Y%_%H%M%S") << "__" << name;
	std::string filePath =  "/tmp/" + ss.str() + ".txt";
	return (filePath);
}

std::string CGI::removeDotDot(std::string path) {
	std::string result;

	// Split the input path into segments
	std::size_t start = 0;
	std::size_t end = path.find('/', start);
	while (end != std::string::npos) {
		std::string segment = path.substr(start, end - start);
		start = end + 1;
		end = path.find('/', start);

		// Ignore "." segments
		if (segment == ".") {
			continue;
		}

		// Remove ".." segments by backtracking one segment in the result
		if (segment == "..") {
			std::size_t lastSlash = result.find_last_of('/');
			if (lastSlash == std::string::npos) {
				// If there are no more segments to remove, return an empty string
				return "";
			}
			result = result.substr(0, lastSlash);
		} else {
			// Append all other segments to the result
			result += "/" + segment;
		}
	}

	// Append the final segment (if any) to the result
	std::string finalSegment = path.substr(start);
	if (finalSegment != "." && finalSegment != "..") {
		result += "/" + finalSegment;
	}

	return result;
}

void CGI::prepare(ClientInfos *clientInfos)
{
	_clientInfos = clientInfos;
	A_Request *request = clientInfos->_requestHandler;
	ServerConfigs *serverConfigs = clientInfos->_serverConfigs;
	_metaVars["SERVER_PROTOCOL"] = request->getHttpVersion();
	_metaVars["SERVER_PORT"] 	 = serverConfigs->getServ();
	_metaVars["REQUEST_METHOD"]  = request->getMethod();
	_metaVars["PATH_INFO"] 		 = clientInfos->_indexPath;
	std::string fullPath = FileSystem::getFullPath(clientInfos->_indexPath);
	_metaVars["SCRIPT_FILENAME"] = removeDotDot(fullPath);
	_metaVars["SCRIPT_NAME"] = clientInfos->_cgiPath;
	_metaVars["QUERY_STRING"] = request->getQuery();
	_metaVars["REMOTE_ADDR"] = clientInfos->_addr;
	_metaVars["REMOTE_PORT"] = clientInfos->_port;
	_metaVars["REDIRECT_STATUS"] = "200";
	if (request->isCgiHeaderHasValue("Content-Type"))
		_metaVars["CONTENT_TYPE"] = request->getCgiHeaderValue("Content-Type");
	if (request->isCgiHeaderHasValue("Content-Type"))
		_metaVars["CONTENT_LENGTH"] = request->getCgiHeaderValue("Content-Length");

	for (std::map<std::string, std::string>::const_iterator iter = request->getCgiMap().cbegin();
			iter != request->getCgiMap().cend(); ++iter){
		std::string newHeaderForm = toEnvHeader(iter->first);
		if (!newHeaderForm.empty())
			_metaVars[newHeaderForm] = iter->second;
	}
	_execCGI();

}

void CGI::read(){
	int status;
	int pid = waitpid(_CGIPid, &status, WNOHANG);
	if (pid == -1){
		_status = ERROR;
	} else if (pid != 0){
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0){
			_status = DONE;
		} else {
			_status = DONE;
		}
	}
	if (_status == DONE || _status == ERROR){
		close(_outFileFd);
		close(_bodyFd);
	}
}


void CGI::_execCGI(char **args, char **env, int bodyFd){
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

void CGI::_execCGI(){
	std::string cgiPath = _clientInfos->_cgiPath;
	std::string filePath = FileSystem::getFullPath(_clientInfos->_indexPath);

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
		std::cout << env[i] << std::endl;
		i++;
	}
	env[i] = NULL;

	//cgi
	if (!_clientInfos->_cgiFilePath.empty()){
		_bodyFd = open(_clientInfos->_cgiFilePath.c_str(), O_RDONLY);
		if (_bodyFd == -1){
			std::runtime_error("CGI Open Failed");
		}
	} else {
		_bodyFd = open(_generateRandomPath().c_str(), O_RDONLY | O_CREAT | O_TRUNC, 0666);
	}
	std::string f = _generateRandomPath().c_str();
	_outFilePath = _generateRandomPath();
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
