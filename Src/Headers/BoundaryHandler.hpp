/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoundaryHandler.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 15:48:13 by mel-amma          #+#    #+#             */
/*   Updated: 2023/02/22 17:51:16 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOUNDRY_HANDLER__
#define BOUNDRY_HANDLER__

#define EXTRA_BOUNDARY
#include <map>
#include <cstring>
#include <vector>
#include <iostream>
#include <utility>
#include "FileSystem.hpp"
#include "requestParser/HeaderParser.hpp"


//test substr size andd test join with past null string
#define BEFORE_CONTENT_TYPE 0 
#define EXTRA100 1 

enum STAGE{
    IN_BODY,
    NEED_CONTENT_TYPE
};

struct BoundaryHandler{

	std::string extra;
	STAGE stage;
	bool first_boundary;
	bool initialized;
	std::string boundary;
	bool done;
	bool error;
	//vector of pairs of what to write + (content-type)OrEmpty to add on last file opened
	typedef std::vector<std::pair<std::string, std::string> > BoundaryRetType;


	BoundaryHandler();
	BoundaryRetType clean_body(std::string &body, size_t size);
	void set_boundary(std::string& boundary);
	bool is_initialized();
	bool clean_boundary(std::string &body, size_t &size, std::string &before_boundary);//clean body from boundary
	std::string parse_mini_header(std::string &body);
	std::string get_content_type(std::string &body, size_t &header_end_found);
	size_t find_boundary_start(std::string &body);
	size_t find_boundary_end(std::string &body,size_t start);
	bool is_done();
	void insert_raw(BoundaryRetType& res ,std::string& raw,std::string& contentType);

	~BoundaryHandler();

	void fill_extra(std::string &str, bool Type);//split string, saves extra, returns what to be written
	void join_up_receives(std::string &body);
	std::string& get_extra();
	bool failed();
	private :
	size_t findSubstring(const std::string& str, const std::string& substr);

};


#endif