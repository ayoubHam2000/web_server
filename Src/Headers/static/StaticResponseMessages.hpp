/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticResponseMessages.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 12:26:09 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/26 13:35:26 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATIC_RESPONSE_MESSSAGES_HPP
#define STATIC_RESPONSE_MESSSAGES_HPP

#include "includes.hpp"
#include "StatusCode.hpp"

class StaticResponseMessages
{
	public :
		static std::map<StatusCode, std::string> MAPPING_RESPONSE_CODE_TO_MESSAGES;

		static  std::map<StatusCode, std::string> S_initResponseMessages();

		static  std::string &getMessageResponseCode(StatusCode responseCode);

};



#endif
