/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/03 13:04:37 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/11 12:29:28 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "../../includes.hpp"
#include "../../static/SupportedMethods.hpp"

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
		std::vector < std::pair <Extention, std::string > > _cgis;
	public :
		Location()
		{
			_autoIndex = false;
		}
		
		std::string &getRoute()
		{
			return (_route);
		}

		std::map<std::string , bool> &getAllowMethods()
		{
			return (_allowMethods);
		}

		std::string &getRedirect()
		{
			return (_redirect);
		}
		bool getAutoIndex() const
		{
			return (_autoIndex);
		}
		
		std::string &getRoot()
		{
			return (_root);
		}
		
		std::vector <std::string> &getIndexes()
		{
			return (_indexes);
		}

		std::string &getUploadPass()
		{
			return (_uploadPass);
		}

		std::vector < std::pair <Extention, std::string > >  &getCgis()
		{
			return (_cgis);
		}
		
		//setters

		void setRoute(std::string route)
		{
			_route = route;
		}
		
		void setAllowMethods(std::vector<std::string> allowMethods)
		{
			_allowMethods.clear();
			for (int i = 0; i < allowMethods.size(); i++)
			{
				if (SupportedMethods::isAllowedMethod(allowMethods[i]) == true)
					_allowMethods[allowMethods[i]] = true;
				else
				{
					std::cerr << "this Method : " << allowMethods[i] << " not Supported!!" << std::endl;
					exit (FAILURE);
				}
			}
		}

		void setRedirect(std::string redirect)
		{
			_redirect = redirect;
		}
		void  setAutoIndex(bool autoIndex)
		{
			_autoIndex = autoIndex;
		}
		
		void setRoot(std::string root)
		{
			_root = root;
		}
		
		void setIndexes(std::vector <std::string> indexes)
		{
			_indexes = indexes;
		}

		void  setUploadPass(std::string uploadPass)
		{
			_uploadPass = uploadPass;
		}

		void setCgis(std::vector < std::pair <Extention, std::string > > cgis)
		{
			_cgis = cgis;
		}

		bool isLocationValid() const
		{
			return  (!(_route.empty() || _root.empty() || _indexes.empty()));
		}

		void addcgi(Extention lang, std::string path)
		{
			_cgis.push_back(std::make_pair(lang, path));	
		}


		bool isMethodAllowed(std::string &method)
		{
			return (_allowMethods.find(method) != _allowMethods.end());
		}

		bool isRouteMatch(std::string &path)
		{
			if (path.rfind(_route, 0) == 0 && (path.length() == _route.length() || ((path.length() > _route.length() && ( _route[_route.length() - 1] == '/' ||   path[_route.length() + 1] == '/')))))
				return (true);
			return (false);
		}
		
};

#endif