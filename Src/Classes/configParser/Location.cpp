# include "configParser/Location.hpp"

Location::Location() {
	_autoIndex = false;
	setDefaultUploadPass();
}

std::string &Location::getRoute() {
	return (_route);
}

std::map<std::string, bool> &Location::getAllowMethods() {
	return (_allowMethods);
}

std::string &Location::getRedirect() {
	return (_redirect);
}

bool Location::getAutoIndex() const {
	return (_autoIndex);
}

std::string &Location::getRoot() {
	return (_root);
}

std::vector<std::string> &Location::getIndexes() {
	return (_indexes);
}

std::string &Location::getUploadPass() {
	return (_uploadPass);
}

std::map<Extention, std::string> &Location::getCgis() {
	return (_cgis);
}

void Location::setRoute(std::string route) {
	_route = route;
}

void Location::setAllowMethods(std::vector<std::string> allowMethods) {
	_allowMethods.clear();
	for (size_t i = 0; i < allowMethods.size(); i++)
	{
		if (SupportedMethods::isAllowedMethod(allowMethods[i]) == true)
			_allowMethods[allowMethods[i]] = true;
		else
		{
			std::cerr << "this Method : " << allowMethods[i] << " not Supported!!" << std::endl;
			exit (FAILURE);
		}
	}
}

void Location::setRedirect(std::string redirect) {
	_redirect = redirect;
}

void Location::setAutoIndex(bool autoIndex) {
	_autoIndex = autoIndex;
}

void Location::setRoot(std::string root) {
	_root = root;
}

void Location::setIndexes(std::vector<std::string> indexes) {
	_indexes = indexes;
}

void Location::setUploadPass(std::string uploadPass) {
	_uploadPass = uploadPass;
}

void Location::setDefaultUploadPass() {
	_uploadPass = DEFAULT_UPLOAD_PASS;
}

void Location::setCgis(std::map<std::string, std::string> &cgis) {
	_cgis = cgis;
}

bool Location::isLocationValid() const {
	return  (!(_route.empty() || _root.empty() || _indexes.empty()));
}

void Location::addcgi(std::string &lang, std::string &path) {
	_cgis[lang] = path;
}

bool Location::isMethodAllowed(std::string &method) {
	return (_allowMethods.find(method) != _allowMethods.end());
}

bool Location::isRouteMatch(std::string &path) {
	if (path.rfind(_route, 0) == 0 && (path.length() == _route.length() || ((path.length() > _route.length() && ( _route[_route.length() - 1] == '/' ||   path[_route.length()] == '/')))))
		return (true);
	return (false);
}

std::string Location::getPathCgi(std::string &extention) {
	std::string pathCgi;
	std::map < Extention, std::string  >::iterator it = _cgis.find(extention);
	if (it != _cgis.end())
		pathCgi = it ->second;
	return (pathCgi);
}
