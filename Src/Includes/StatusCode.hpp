/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusCode.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/07 11:19:04 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/28 12:38:13 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUS_CODE_HPP
#define STATUS_CODE_HPP


enum StatusCode
{
	NOT_FOUND = 404,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	METHOD_NOT_ALLOWED = 405,
	REQUEST_ENTITY_TOO_LARGE = 413,
	REQUEST_URI_TOO_LONG = 414,
	REQUEST_HEADER_TOO_LARGE = 494,
	INTERNAL_SERVER_ERROR = 500,
	OK = 200,
	CREATED = 201,
	NO_CONTENT = 204,
	NOT_IMPLEMENTED = 501,
	MOVED_PERMANETLY = 301,
	FOUND = 302,
	PERMANENT_REDIRECT = 308
};


#endif