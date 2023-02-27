# include "static/StaticConfig.hpp"

std::vector<std::map <std::string, int > > StaticConfig::SERVER_CONFIGS = StaticConfig::MakeServerConfigVector();

std::vector<std::map<std::string, int> > StaticConfig::MakeServerConfigVector() {
	std::vector< std::map <std::string, int > > v(2, std::map <std::string , int>());
	// initialize server Static Config
	std::map <std::string, int > &serverStaticConfig = v[SERVER_STATIC_CONFIG];

	serverStaticConfig["listen"] =  1;
	serverStaticConfig["server_name"] =  1;
	serverStaticConfig["max_client_body_size"] =  1;
	serverStaticConfig["error_page"] =  2;
	serverStaticConfig["location"] =  1;

	// initialize Location Static Config
	std::map <std::string, int > &serverLocationConfig = v[LOCATION_STATIC_CONFIG];

	serverLocationConfig["allow_methods"] = INFINI;
	serverLocationConfig["redirect"] = 1;
	serverLocationConfig["autoindex"] = 1;
	serverLocationConfig["root"] = 1;
	serverLocationConfig["index"] = INFINI;
	serverLocationConfig["upload_pass"] = 1;
	serverLocationConfig["cgi_pass"] = 2;
	return v;
}
