/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SupportedMethods.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 15:07:32 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/10 12:15:59 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SUPPORTED_METHODS_HPP
#define SUPPORTED_METHODS_HPP

#include "includes.hpp"
#define SERVER_STATIC_CONFIG 0
#define LOCATION_STATIC_CONFIG 1

struct SupportedMethods
{
	static std::map <std::string, bool > 	SUPPORTED_METHODS;

	static std::map <std::string, bool >  	S_SetSupportedMethods();

	static bool isAllowedMethod(std::string &method);
};

#endif
