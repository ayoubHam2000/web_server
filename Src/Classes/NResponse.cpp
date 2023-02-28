/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NResponse.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 11:42:23 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/28 13:33:06 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "NResponse.hpp"


NResponse::NResponse()
{
	
}


std::string NResponse::getHeaderResponse(Client &client)
{
	ClientInfos &ClientInfos = client.clientInfos;
	std::string headerRespone = ClientInfos._requestHandler->getHttpVersion() + " " +  std::to_string(client.responseCode);
	fseek(client.fp, 0L, SEEK_END);
	size_t fileSize = ftell(client.fp);
	rewind(client.fp);
	std::string extention = FileSystem::getExtention(client.path);
	std::string contentType =  ContentTypes::getContentType(extention);
	headerRespone += StaticResponseMessages::getMessageResponseCode(client.responseCode);
	if (client.responseCode == MOVED_PERMANETLY)
		headerRespone += "\r\nLocation: " + ClientInfos._bestLocationMatched->getRedirect();
	headerRespone += "\r\nConnection: close\r\nContent-Length: " + std::to_string(fileSize) +  "\r\nContent-Type: " + contentType + "; charset=utf-8 \r\n\r\n";
	return (headerRespone);
}