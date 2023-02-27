/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 12:42:18 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/25 17:13:01 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetRequest.hpp"

void GetRequest::handleRequest(std::string &body, size_t size, Client &client)
{
	if (!body.empty())
		client.set_response_code(BAD_REQUEST);
	if (client.isForCgi == false)
		client.tryOpenRessource();
	client.finished_body();
}

GetRequest::~GetRequest()
{

}