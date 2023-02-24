/**************************************************/
/*     created by TheWebServerTeam 2/9/23         */
/**************************************************/

#include "MyWebServer.h"
#include "MyBuffer.h"
#include "FileSystem.hpp"

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


void start(const std::string &confPath){
	/*MyWebServer	myWebServer;

	initSignals();
	myWebServer.setConfigurations(confPath);
	myWebServer.setServers();
	myWebServer.startWebServer();
	myWebServer.stopWebServer();*/


	char cwd[1024];

	// Get the current working directory
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		std::cout << "Current working directory: " << cwd << std::endl;
	} else {
		std::perror("getcwd() error");
	}

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