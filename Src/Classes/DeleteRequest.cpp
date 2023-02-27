/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 12:42:18 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/25 17:12:15 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteRequest.hpp"

void DeleteRequest::handleRequest(std::string &, size_t , Client &client)
{
	client.isForCgi = false;
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