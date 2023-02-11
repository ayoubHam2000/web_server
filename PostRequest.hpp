/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-amma <mel-amma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 14:37:58 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/11 15:56:30 by mel-amma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_REQUEST_HPP
#define POST_REQUEST_HPP

#include "A_Request.hpp"
#include "filesystem.hpp"

class PostRequest : public  A_Request
{
	FileSystem fs;
	bool file_initialized;
	public :
		PostRequest()
		{
			file_initialized = false;
		}
		
		void handleRequest(std::string &body, Client &client)
		{
			//open file where to 
			if(!file_initialized)
			{
				// fs = FileSystem(_path/*get best batch*/, WRITE, get/*get extension*/)
				file_initialized = true;
			}


			// std::vector < std::string > content_type_vector = headers.at("Content-Type");
			// std::string content_type = content_type_vector[0];
			// if(content_type == "multipart/form-data")
			// {
			// 	// take care content-type or content_disposition with their boundaries having boundary set somewhere
			// }

			std::cout << "aa\n";
		};

		~PostRequest()
		{

		}
	
};


#endif

