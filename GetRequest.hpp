/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetRequest.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-amma <mel-amma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 14:37:58 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/12 18:15:17 by mel-amma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_REQUEST_HPP
#define GET_REQUEST_HPP

#include "A_Request.hpp"

class GetRequest : public  A_Request
{
	public :
		void handleRequest(std::string &body, size_t size, Client &client);
		~GetRequest();
};


#endif

