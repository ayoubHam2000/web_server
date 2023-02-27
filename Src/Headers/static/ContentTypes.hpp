/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ContentTypes.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 15:07:32 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/26 11:55:25 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTENT_TYPES_HPP
#define CONTENT_TYPES_HPP

#include "includes.hpp"

struct ContentTypes
{
	static std::map <std::string , std::string > S_CONTENT_TYPES_MAPPING;
	static std::map <std::string , std::string > S_EXTENTIONS_MAPPING;

	static std::map <std::string , std::string >  S_setContentTypesMapping();;

	static std::map <std::string , std::string >  S_setExtentionsMapping();

	static std::string &getContentType( std::string &extention);

	static std::string &getExtention( std::string &contentType);
};



#endif