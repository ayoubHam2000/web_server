/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticResponsePages.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/07 11:13:53 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/23 14:37:22 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATIC_RESPONSE_PAGES
#define STATIC_RESPONSE_PAGES

#include "includes.hpp"
#include "./StatusCode.hpp"
#define PATH_404 "./static/response_pages/404.html"
#define PATH_400 "./static/response_pages/400.html"
#define PATH_403 "./static/response_pages/403.html"
#define PATH_405 "./static/response_pages/405.html"
#define PATH_413 "./static/response_pages/413.html"
#define PATH_414 "./static/response_pages/414.html"
#define PATH_494 "./static/response_pages/494.html"
#define PATH_500 "./static/response_pages/500.html"
#define PATH_501 "./static/response_pages/501.html"
#define PATH_200 "./static/response_pages/200.html"
#define PATH_201 "./static/response_pages/201.html"
#define PATH_204 "./static/response_pages/204.html"
#define PATH_301 "./static/response_pages/301.html"




struct StaticResponsePages
{
	static  std::map < int  , std::string>  RESPONSE_PAGES;

	static 	std::map < int  , std::string> S_InitResponsePages();
};



#endif