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




#include "../includes.hpp"
#define SERVER_STATIC_CONFIG 0
#define LOCATION_STATIC_CONFIG 1

struct StaticConfig
{
	static std::vector< std::map <std::string, int > > SERVER_CONFIGS;
	static std::vector< std::map <std::string, int > >  MakeServerConfigVector()
	{
		std::vector< std::map <std::string, int > > v(2, std::map <std::string , int>());
		// initialize server Static Config
		std::map <std::string, int > &serverStaticConfig = v[SERVER_STATIC_CONFIG];
	
		serverStaticConfig["listen"] =  1;
		serverStaticConfig["server_name"] =  1;
		serverStaticConfig["max_client_body_size"] =  1;
		serverStaticConfig["error_page"] =  2;
		serverStaticConfig["location"] =  1;

		// initialize Location Static Config
		std::map <std::string, int > &serverLocationConfig = v[LOCATION_STATIC_CONFIG];

		serverLocationConfig["allow_methods"] = INFINI;
		serverLocationConfig["redirect"] = 1;
		serverLocationConfig["autoindex"] = 1;
		serverLocationConfig["root"] = 1;
		serverLocationConfig["index"] = INFINI;
		serverLocationConfig["upload_pass"] = 1;
		serverLocationConfig["cgi_pass"] = 2;
		return v;
	}
};



#endif