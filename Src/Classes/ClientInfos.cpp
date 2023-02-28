# include "ClientInfos.hpp"

ClientInfos::ClientInfos() {
	_requestHandler = nullptr;
	_responseHandler = nullptr;
	_bestLocationMatched = nullptr;
	_responseHandler = nullptr;
}

void ClientInfos::setRequestHandler(A_Request *req) {
	_requestHandler = req;
}

void ClientInfos::setResponseHandler(A_Response *res) {
	_responseHandler = res;
}

void ClientInfos::setServerConfigs(ServerConfigs *conf) {
	_serverConfigs = conf;
}

void ClientInfos::setBestLocation(Location *location) {
	_bestLocationMatched = location;
}

void ClientInfos::setCgiPath(std::string cgiPath) {
	_cgiPath = cgiPath;
}

void ClientInfos::setIndexPath(std::string indexPath) {
	_indexPath = indexPath;
}

void ClientInfos::setCgiFilePath(std::string cgiFilePath) {
	_cgiFilePath = cgiFilePath;
}

ClientInfos::~ClientInfos() {
	//std::cout << "Destruct ClientInfos\n\n";
	// if (_requestHandler)
	// 	delete _requestHandler;
	// if (_responseHandler)
	// 	delete _responseHandler;
	// _requestHandler = nullptr;
	// _responseHandler = nullptr;
}
