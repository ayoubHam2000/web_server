/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NResponse.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 11:38:41 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/24 12:08:10 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef N_RESPONSE_HPP
#define N_RESPONSE_HPP


#include "A_Response.hpp"
#include "Client.hpp"

class NResponse : public  A_Response
{
	public :
		NResponse();
		std::string getHeaderResponse(Client &client);
};


#endif