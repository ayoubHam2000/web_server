/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/03 13:04:37 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/23 16:26:59 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "includes.hpp"
#include "static/SupportedMethods.hpp"

#define DEFAULT_UPLOAD_PASS "./public/uploads/"

#define  Extention std::string


class Location
{
	private :
		std::string 										_route;
		std::map<std::string , bool>						_allowMethods;
		std::string 										_redirect;
		bool 												_autoIndex;
		std::string 										_root;
		std::vector <std::string> 							_indexes;
		std::string 										_uploadPass;
		std::map < Extention, std::string  > 				_cgis;
	public :
		Location();
		
		std::string &getRoute();

		std::map<std::string , bool> &getAllowMethods();

		std::string &getRedirect();

		bool getAutoIndex() const;
		
		std::string &getRoot();
		
		std::vector <std::string> &getIndexes();

		std::string &getUploadPass();

		std::map < Extention, std::string  >   &getCgis();
		
		//setters

		void setRoute(std::string route);
		
		void setAllowMethods(std::vector<std::string> allowMethods);

		void setRedirect(std::string redirect);

		void  setAutoIndex(bool autoIndex);
		
		void setRoot(std::string root);
		
		void setIndexes(std::vector <std::string> indexes);

		void  setUploadPass(std::string uploadPass);

		void setDefaultUploadPass();

		void setCgis(std::map < Extention, std::string  >  &cgis);

		bool isLocationValid() const;

		void addcgi(Extention &lang, std::string &path);


		bool isMethodAllowed(std::string &method);

		bool isRouteMatch(std::string &path);

		std::string getPathCgi(Extention &extention);
};

#endif