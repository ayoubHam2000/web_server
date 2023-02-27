#ifndef HTTP__HPP
#define HTTP__HPP

#include "includes.hpp"
#include "StatusCode.hpp"
#include "Server.hpp"
#include "configParser/ConfigParser.hpp"

namespace ft
{


struct Http{
    fd_set& 		_reads;
    fd_set& 		_writes;
    ListClients& 	_clients;
    Server& 		_server;

    Http(fd_set& reads, fd_set& writes, ListClients& clients, Server& server);;

    void getRequest(int Client_Number, ServerMap& Servers);;

    void sendResponse(int Client_Number);;

};

}
#endif 