/***************************************************/
/*     created by TheWebServerTeam 2/17/23         */
/***************************************************/

#ifndef WEB_SERVER_MYWEBSERVER_H
#define WEB_SERVER_MYWEBSERVER_H

#include <csignal>
#include <iostream>

class MyWebServer {
/*****************************************************************/
// Non-member function overloads (relational operators, swap)
/*****************************************************************/
public:
	static bool webServerIsRunning;
public:
	MyWebServer(){

	}
/*****************************************************************/
// Static Functions
/*****************************************************************/
private:
	static void _handler(int sig, siginfo_t *, void *p){
		if (sig == SIGTERM || sig == SIGINT || sig == SIGQUIT){
			std::cout << "Stop WebServer" << std::endl;
			if (MyWebServer::webServerIsRunning)
				MyWebServer::webServerIsRunning = false;
			else
				std::exit(0);
		}
	}

public:
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
};



#endif //WEB_SERVER_MYWEBSERVER_H
