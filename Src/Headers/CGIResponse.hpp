/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIResponse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 12:00:05 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/26 16:32:57 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include "A_Response.hpp"
#include "Client.hpp"
#include "requestParser/HeaderParser.hpp"


class CGIResponse : public  A_Response
{
	private :
		std::string										_header;
		std::string										_body;
		std::multimap<std::string, std::string > 		_cgiHeaders;
		void 											_parseCgiHeader(std::string buffer ,Client &client);
		void											_setBody(std::string body);
		bool											_cgiHasHeader(const std::string &header);
	public :
		CGIResponse();
		std::string getHeaderResponse(Client &client);
};


#endif
