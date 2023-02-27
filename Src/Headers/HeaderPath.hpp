/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderPath.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 14:58:22 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/20 14:59:36 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DUMMY_CLIENT_URL_H
#define DUMMY_CLIENT_URL_H

#include "includes.hpp"

class HeaderPath {
private:
	std::string	_path;
	std::string _params;
public:
	HeaderPath();

	HeaderPath(const HeaderPath& other);

	HeaderPath& operator=(const HeaderPath& other);

	~HeaderPath();
public:

	const std::string &getPath() const;

	const std::string &getParams() const;

public:

	static bool validatePath(const char *path);

	static std::string decodePath(const char *path);

	bool parse(const std::string &path);

};


#endif //DUMMY_CLIENT_URL_H