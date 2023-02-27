# include "static/SupportedMethods.hpp"

std::map <std::string, bool> SupportedMethods::SUPPORTED_METHODS =  SupportedMethods::S_SetSupportedMethods();

std::map<std::string, bool> SupportedMethods::S_SetSupportedMethods() {
	std::map <std::string, bool > supportedMethods;
	supportedMethods["GET"] = true;
	supportedMethods["POST"] = true;
	supportedMethods["DELETE"] = true;
	return supportedMethods;
}

bool SupportedMethods::isAllowedMethod(std::string &method) {
	if (SUPPORTED_METHODS.find(method) == SUPPORTED_METHODS.end())
	{
		return (false);
		std::cerr << "this Method : " << method << " not Supported!!" << std::endl;
		exit (FAILURE);
	}
	return (true);
}
