#include "static/ContentTypes.hpp"


std::map <std::string, std::string > ContentTypes::S_CONTENT_TYPES_MAPPING =  ContentTypes::S_setContentTypesMapping();
std::map <std::string, std::string > ContentTypes::S_EXTENTIONS_MAPPING =  ContentTypes::S_setExtentionsMapping();

std::map<std::string, std::string> ContentTypes::S_setContentTypesMapping() {
	std::map <std::string , std::string > contentTypeMapping;
	contentTypeMapping[".css"] = "text/css";
	contentTypeMapping[".csv"] = "text/csv";
	contentTypeMapping[".gif"] = "image/gif";
	contentTypeMapping[".htm"] = "text/html";
	contentTypeMapping[".html"] = "text/html";
	contentTypeMapping[".ico"] = "image/x-icon";
	contentTypeMapping[".jpeg"] = "image/jpeg";
	contentTypeMapping[".jpg"] = "image/jpeg";
	contentTypeMapping[".js"] = "application/javascript";
	contentTypeMapping[".json"] = "application/json";
	contentTypeMapping[".png"] = "image/png";
	contentTypeMapping[".pdf"] = "application/pdf";
	contentTypeMapping[".svg"] = "image/svg+xml";
	contentTypeMapping[".txt"] = "text/plain";
	contentTypeMapping[".mp4"] = "video/mp4";
	contentTypeMapping[".mjs"] = "text/javascript";
	contentTypeMapping[".mp3"] = "audio/mpeg";
	contentTypeMapping[".mpeg"] = "video/mpeg";
	contentTypeMapping[".php"] = "application/x-httpd-php";
	contentTypeMapping[".c"] = "text/html";
	contentTypeMapping[""] = "application/octet-stream";
	return (contentTypeMapping);
}

std::map<std::string, std::string> ContentTypes::S_setExtentionsMapping() {
	std::map <std::string , std::string > extentionsMapping;
	extentionsMapping["text/css"] = ".css";
	extentionsMapping["text/csv"] = ".csv";
	extentionsMapping["image/gif"] = ".gif";
	extentionsMapping["text/html"] = ".htm";
	extentionsMapping["text/html"] = ".html";
	extentionsMapping["image/x-icon"] = ".ico";
	extentionsMapping["image/jpeg"] = ".jpeg";
	extentionsMapping["image/jpeg"] = ".jpg";
	extentionsMapping["application/javascript"] = ".js";
	extentionsMapping["application/json"] = ".json";
	extentionsMapping["image/png"] = ".png";
	extentionsMapping["application/pdf"] = ".pdf";
	extentionsMapping["image/svg+xml"] = ".svg";
	extentionsMapping["text/plain"] = ".txt";
	extentionsMapping["application/octet-stream"] = "";
	extentionsMapping["video/mp4"] = ".mp4";
	extentionsMapping["text/javascript"] = ".mjs";
	extentionsMapping["audio/mpeg"] = ".mp3";
	extentionsMapping["video/mpeg"] = ".mpeg";
	extentionsMapping["application/x-httpd-php"] = ".php";
	return (extentionsMapping);
}

std::string &ContentTypes::getContentType(std::string &extention) {
	if (S_CONTENT_TYPES_MAPPING.find(extention) == S_CONTENT_TYPES_MAPPING.end())
		return (S_CONTENT_TYPES_MAPPING[""]);
	return (S_CONTENT_TYPES_MAPPING[extention]);
}

std::string &ContentTypes::getExtention(std::string &contentType) {
	if (S_EXTENTIONS_MAPPING.find(contentType) == S_EXTENTIONS_MAPPING.end())
		return (S_EXTENTIONS_MAPPING["application/octet-stream"]);
	return (S_EXTENTIONS_MAPPING[contentType]);
}
