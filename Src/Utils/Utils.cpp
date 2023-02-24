/***************************************************/
/*     created by TheWebServerTeam 2/18/23         */
/***************************************************/

#include "Libraries.h"

size_t	get_time_ms(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000 + t.tv_usec / 1000);
}

bool isHexChar(char c){
	if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'))
		return (true);
	return (false);
}

const std::string&	getMIMEType(const std::string& extension){
	static std::string def = "aplication/octet-stream";
	static std::map<std::string, std::string> mime_types;
	mime_types[".aac"] = "audio/aac";
	mime_types[".avi"] = "video/x-msvideo";
	mime_types[".bmp"] = "image/bmp";
	mime_types[".css"] = "text/css";
	mime_types[".csv"] = "text/csv";
	mime_types[".doc"] = "application/msword";
	mime_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mime_types[".gif"] = "image/gif";
	mime_types[".htm"] = "text/html";
	mime_types[".html"] = "text/html";
	mime_types[".ico"] = "image/x-icon";
	mime_types[".jpeg"] = "image/jpeg";
	mime_types[".jpg"] = "image/jpeg";
	mime_types[".js"] = "application/javascript";
	mime_types[".json"] = "application/json";
	mime_types[".mp3"] = "audio/mpeg";
	mime_types[".mp4"] = "video/mp4";
	mime_types[".mpeg"] = "video/mpeg";
	mime_types[".oga"] = "audio/ogg";
	mime_types[".ogv"] = "video/ogg";
	mime_types[".pdf"] = "application/pdf";
	mime_types[".png"] = "image/png";
	mime_types[".ppt"] = "application/vnd.ms-powerpoint";
	mime_types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mime_types[".rar"] = "application/x-rar-compressed";
	mime_types[".rtf"] = "application/rtf";
	mime_types[".svg"] = "image/svg+xml";
	mime_types[".tar"] = "application/x-tar";
	mime_types[".txt"] = "text/plain";
	mime_types[".wav"] = "audio/wav";
	mime_types[".webm"] = "video/webm";
	mime_types[".webp"] = "image/webp";
	mime_types[".xls"] = "application/vnd.ms-excel";
	mime_types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mime_types[".xml"] = "application/xml";
	mime_types[".zip"] = "application/zip";
	if (mime_types.find(extension) != mime_types.end())
		return (mime_types[extension]);
	return (def);
}

const std::string&	getExtension(const std::string& mimeType){
	const static std::string def = "";
	static std::map<std::string, std::string> mime_to_ext;
	mime_to_ext["application/andrew-inset"] = ".ez";
	mime_to_ext["text/plain"] = ".txt";
	mime_to_ext["image/jpeg"] = ".jpg";
	mime_to_ext["image/png"] = ".png";
	mime_to_ext["image/gif"] = ".gif";
	mime_to_ext["image/bmp"] = ".bmp";
	mime_to_ext["image/tiff"] = ".tiff";
	mime_to_ext["text/plain"] = ".txt";
	mime_to_ext["text/html"] = ".html";
	mime_to_ext["text/css"] = ".css";
	mime_to_ext["text/javascript"] = ".js";
	mime_to_ext["application/applixware"] = ".aw";
	mime_to_ext["application/atom+xml"] = ".atom";
	mime_to_ext["application/atomcat+xml"] = ".atomcat";
	mime_to_ext["application/atomserv+xml"] = ".atomsrv";
	mime_to_ext["application/bdoc"] = ".bdoc";
	mime_to_ext["application/ccxml+xml"] = ".ccxml";
	mime_to_ext["application/cdmi-capability"] = ".cdmia";
	mime_to_ext["application/cdmi-container"] = ".cdmic";
	mime_to_ext["application/cdmi-domain"] = ".cdmid";
	mime_to_ext["application/cdmi-object"] = ".cdmio";
	mime_to_ext["application/cdmi-queue"] = ".cdmiq";
	mime_to_ext["application/cu-seeme"] = ".cu";
	mime_to_ext["application/dash+xml"] = ".mpd";
	mime_to_ext["application/davmount+xml"] = ".davmount";
	mime_to_ext["application/docbook+xml"] = ".dbk";
	mime_to_ext["application/dssc+der"] = ".dssc";
	mime_to_ext["application/dssc+xml"] = ".xdssc";
	mime_to_ext["application/ecmascript"] = ".ecma";
	mime_to_ext["application/emma+xml"] = ".emma";
	mime_to_ext["application/epub+zip"] = ".epub";
	mime_to_ext["application/exi"] = ".exi";
	mime_to_ext["application/font-tdpfr"] = ".pfr";
	mime_to_ext["application/font-woff"] = ".woff";
	mime_to_ext["application/font-woff2"] = ".woff2";
	mime_to_ext["application/gml+xml"] = ".gml";
	mime_to_ext["application/gpx+xml"] = ".gpx";
	mime_to_ext["application/gxf"] = ".gxf";
	mime_to_ext["application/hyperstudio"] = ".stk";
	mime_to_ext["application/inkml+xml"] = ".ink";
	mime_to_ext["application/ipfix"] = ".ipfix";
	mime_to_ext["application/java-archive"] = ".jar";
	mime_to_ext["application/java-serialized-object"] = ".ser";
	mime_to_ext["application/java-vm"] = ".class";
	mime_to_ext["application/javascript"] = ".js";
	mime_to_ext["application/json"] = ".json";
	mime_to_ext["application/json5"] = ".json5";
	mime_to_ext["application/jsonml+json"] = ".jsonml";
	mime_to_ext["application/ld+json"] = ".jsonld";
	mime_to_ext["application/lost+xml"] = ".lostxml";
	mime_to_ext["application/mac-binhex40"] = ".hqx";
	mime_to_ext["application/mac-compactpro"] = ".cpt";
	mime_to_ext["application/mads+xml"] = ".mads";
	mime_to_ext["application/marc"] = ".mrc";
	mime_to_ext["application/marcxml+xml"] = ".mrcx";
	mime_to_ext["application/mathematica"] = ".ma";
	mime_to_ext["application/mathml+xml"] = ".mathml";
	mime_to_ext["application/mbox"] = ".mbox";
	mime_to_ext["application/mediaservercontrol+xml"] = ".mscml";
	mime_to_ext["application/metalink+xml"] = ".metalink";
	mime_to_ext["application/metalink4+xml"] = ".meta4";
	mime_to_ext["application/mets+xml"] = ".mets";
	mime_to_ext["application/mods+xml"] = ".mods";
	mime_to_ext["application/mp21"] = ".m21";
	mime_to_ext["application/mp4"] = ".mp4";
	mime_to_ext["application/msword"] = ".doc";
	mime_to_ext["application/mxf"] = ".mxf";
	mime_to_ext["application/octet-stream"] = ".bin";
	mime_to_ext["application/oda"] = ".oda";
	mime_to_ext["application/oebps-package+xml"] = ".opf";
	mime_to_ext["application/ogg"] = ".ogx";
	mime_to_ext["application/omdoc+xml"] = ".omdoc";
	mime_to_ext["application/onenote"] = ".onetoc";
	mime_to_ext["application/oxps"] = ".oxps";
	mime_to_ext["application/patch-ops-error+xml"] = ".xer";
	mime_to_ext["application/pdf"] = ".pdf";
	if (mime_to_ext.find(mimeType) != mime_to_ext.end())
		return (mime_to_ext[mimeType]);
	return (def);
}