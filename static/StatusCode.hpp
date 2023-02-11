/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusCode.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-amma <mel-amma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/07 11:19:04 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/11 17:13:24 by mel-amma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUS_CODE_HPP
#define STATUS_CODE_HPP


enum StatusCode
{
	NOT_FOUND = 404,
	BAD_REQUEST = 400,
	METHOD_NOT_ALLOWED = 405,
	INTERNAL_SERVER_ERROR = 500,
	OK = 200,
};


#endif