/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   A_Response.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 11:22:57 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/24 15:37:18 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef A_RESPONSE_HPP
#define A_RESPONSE_HPP

#include "includes.hpp"
#include "StatusCode.hpp"
#include "configParser/Location.hpp"
#include "requestParser/HeaderParser.hpp"
#include "static/StaticResponseMessages.hpp"
#include "HeaderPath.hpp"

class Client;

class A_Response
{
	private:
		bool 		_isHeaderSend;
	public :
		A_Response();

		bool 				sendHeaderResponse(Client &client);
		void 		 		serve_resource(Client &client);
		
		virtual std::string getHeaderResponse(Client &client) = 0;

		bool 				isHeaderSend();



		virtual ~A_Response(){};

};

#endif