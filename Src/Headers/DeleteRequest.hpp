/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteRequest.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 14:37:58 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/20 19:52:17 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DELETE_REQUEST_HPP
#define DELETE_REQUEST_HPP

#include "A_Request.hpp"
#include "Client.hpp"

class DeleteRequest : public  A_Request
{

	public :
		
		void handleRequest(std::string &body, size_t size, Client &client);
		
		~DeleteRequest();
};


#endif
