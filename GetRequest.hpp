/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 14:37:58 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/09 12:47:16 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_REQUEST_HPP
#define GET_REQUEST_HPP

#include "A_Request.hpp"

class GetRequest : public  A_Request
{
	public :
		void handleRequest(std::string &body, Client &client);
		~GetRequest();
};


#endif

