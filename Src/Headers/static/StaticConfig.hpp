/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticConfiguration.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/01 15:07:32 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/09 16:02:09 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATIC_CONFIGURATION_HPP
#define STATIC_CONFIGURATION_HPP

#include "includes.hpp"
#define SERVER_STATIC_CONFIG 0
#define LOCATION_STATIC_CONFIG 1

struct StaticConfig
{
	static std::vector< std::map <std::string, int > > SERVER_CONFIGS;

	static std::vector< std::map <std::string, int > >  MakeServerConfigVector();
};



#endif