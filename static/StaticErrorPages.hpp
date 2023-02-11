/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticErrorPages.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-amma <mel-amma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/07 11:13:53 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/11 17:13:31 by mel-amma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATIC_ERROR_PAGES
#define STATIC_ERROR_PAGES



#include "../includes.hpp"
#include "./StatusCode.hpp"
#define PATH_404 "./public/errors/404.html"
#define PATH_400 "./public/errors/400.html"
#define PATH_405 "./public/errors/405.html"
#define PATH_500 "./public/errors/405.html"


struct StaticErrorPages
{
	static  std::map < int  , std::string>  ERROR_PAGES;
	static 	std::map < int  , std::string> S_InitErrorPages()
	{
		std::map < int  , std::string> errorPages;
		errorPages[NOT_FOUND] = PATH_404;
		errorPages[BAD_REQUEST] = PATH_400;
		errorPages[METHOD_NOT_ALLOWED] = PATH_405;
		errorPages[INTERNAL_SERVER_ERROR] = PATH_500;
		return errorPages;
	}
};



#endif