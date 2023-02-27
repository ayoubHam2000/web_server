/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   A_Response.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 11:27:06 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/27 12:13:29 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "A_Response.hpp"
#include "Client.hpp"


A_Response::A_Response()
{
	_isHeaderSend = false;
}

bool A_Response::isHeaderSend()
{
	return (_isHeaderSend);
}

bool A_Response::sendHeaderResponse(Client &client)
{

	if (client.sendError)
		client.setPathResponse();
	std::string responseHeader = getHeaderResponse(client);
	std::cout << responseHeader << std::endl;
	if (send(client.socket, responseHeader.c_str(), responseHeader.length(), 0)  == -1 ||\
			client.clientInfos._requestHandler->getMethod() == "HEAD")// || isRedirection(client.responseCode))
	{
		fclose(client.fp);
		client.fp = nullptr;
		return (false);
	}
	_isHeaderSend = true;
	return (true);
}


void A_Response::serve_resource(Client &client)
{
	if (client.fp == nullptr)
				return ;
	char body[BUFFER_SIZE + 1];
	memset(body, 0, BUFFER_SIZE + 1);
	int r = fread(body , 1, BUFFER_SIZE, client.fp);
	if (r <= 0)
	{
		fclose(client.fp);
		client.fp = nullptr;
		return ;
	}
	if (send(client.socket, body, r, 0) == -1)
	{
		fclose(client.fp);
		client.fp = nullptr;
	}
}