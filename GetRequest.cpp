/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/09 12:42:18 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/10 15:11:55 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GetRequest.hpp"
#include "client.hpp"

void GetRequest::handleRequest(std::string &body, Client &client)
{
	if (!body.empty())
	{
		client.set_error_code(BAD_REQUEST);
	}
}

GetRequest::~GetRequest()
{

}