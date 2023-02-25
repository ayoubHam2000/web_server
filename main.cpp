/**************************************************/
/*     created by TheWebServerTeam 2/9/23         */
/**************************************************/

#include "MyWebServer.h"
#include "MyBuffer.h"
#include "FileSystem.hpp"
#include "Client.h"

static void _handler(int sig, siginfo_t *, void *p){
	if (sig == SIGTERM || sig == SIGINT || sig == SIGQUIT){
		std::cout << "Stop WebServer" << std::endl;
		if (MyWebServer::webServerIsRunning)
			MyWebServer::webServerIsRunning = false;
		else
			std::exit(0);
	}
}

static void	initSignals(){
	struct sigaction	s;
	sigset_t			set;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGQUIT);
	s.sa_sigaction = _handler;
	s.sa_flags = SA_SIGINFO;
	s.sa_mask = set;
	if (sigaction(SIGINT, &s, NULL) == -1
		|| sigaction(SIGTERM, &s, NULL) == -1
		|| sigaction(SIGQUIT, &s, NULL) == -1)
	{
		std::cerr << "SetUp Signal Failed" << std::endl;
		std::exit(1);
	}
}

void _execCGI(){
	char *args[3] = {
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
	std::cout << access(args[0], X_OK) << std::endl;
}

std::string removeDotDot(std::string path) {
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

void start(const std::string &confPath){
	MyWebServer	myWebServer;

	initSignals();
	myWebServer.setConfigurations(confPath);
	myWebServer.setServers();
	myWebServer.startWebServer();
	myWebServer.stopWebServer();

	//std::cout << removeDotDot("/Users/aben-ham/.1337/projects/web_server/cmake-build-debug/./../public/main.php") << std::endl;
	//_execCGI();


	// Get the current working directory


	//ChunkContentHandler::testFunction(confPath);

	/*FileSystem f;

	std::vector<std::string> a;
	f.getListOfFiles("../public", a);*/

	//MyBuffer::test();

}

int	main(int ac, char **av)
{
	if (ac != 2){
		std::cout << "Path to the configuration file is missing." << std::endl;
		return (1);
	}
	try {
		start(av[1]);
	} catch(const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}