/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 12:42:18 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/21 17:18:50 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteRequest.hpp"

void DeleteRequest::handleRequest(std::string &body, size_t size, Client &client)
{
	char* path = strcpy(new char[client.path.length() + 1], client.path.c_str());
	if (!FileSystem::isDirectory(path) && !FileSystem::file_exists(path))
			client.set_response_code(NOT_FOUND);
	else if (FileSystem::removeAll(path))
		client.set_response_code(NO_CONTENT);
	else
		client.set_response_code(FORBIDDEN);
	client.finished_body();
	delete []path;
}

DeleteRequest::~DeleteRequest()
{

}