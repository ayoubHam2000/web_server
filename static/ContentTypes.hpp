/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ContentTypes.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 15:07:32 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/09 15:56:25 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CONTENT_TYPES_HPP
#define CONTENT_TYPES_HPP




#include "../includes.hpp"


struct ContentTypes
{
	static std::map <std::string , std::string > S_CONTENT_TYPES_MAPPING;
	static std::map <std::string , std::string > S_EXTENTIONS_MAPPING;
	static std::map <std::string , std::string >  S_setContentTypesMapping()
	{
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
		contentTypeMapping[""] = "application/octet-stream";
		return (contentTypeMapping);
	};

	static std::map <std::string , std::string >  S_setExtentionsMapping()
	{
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
		return (extentionsMapping);
	}

	static std::string &getContentType( std::string &extention)
	{
		if (S_CONTENT_TYPES_MAPPING.find(extention) == S_CONTENT_TYPES_MAPPING.end())
			return (S_CONTENT_TYPES_MAPPING[""]);
		return (S_CONTENT_TYPES_MAPPING[extention]);	
	}

	static std::string &getExtention( std::string &contentType)
	{
		if (S_EXTENTIONS_MAPPING.find(contentType) == S_EXTENTIONS_MAPPING.end())
			return (S_EXTENTIONS_MAPPING["application/octet-stream"]);
		return (S_EXTENTIONS_MAPPING[contentType]);	
	}
};



#endif