/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoundaryHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/15 15:44:45 by mel-amma          #+#    #+#             */
/*   Updated: 2023/02/28 13:30:21 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BoundaryHandler.hpp"


size_t BoundaryHandler::findSubstring(const std::string& str, const std::string& substr)
{
    const char* strData = str.data();
    const char* substrData = substr.data();
    size_t strLength = str.length();
    size_t substrLength = substr.length();

    for (size_t i = 0; i <= strLength - substrLength; i++)
    {
        size_t len = std::min(substrLength, strLength - i);
        if (memcmp(strData + i, substrData, len) == 0)
        {
            return i;
        }
    }

    return std::string::npos;
}

BoundaryHandler::BoundaryHandler()
{
    stage = IN_BODY;
    initialized = false;
    done = false;
    first_boundary = true;
    error = false;
};

//map of what to (write+size) + (content-type)OrEmpty to add on last file opened


//if returned map size is 0 then just go again till next iteration
//stop when is_done()
BoundaryHandler::BoundaryRetType BoundaryHandler::clean_body(std::string &body, size_t size)
{
    BoundaryRetType res;
    std::string before_boundary;
    bool boundary_found;
    std::string empty_content = "";
    // std::cout << "from last extra "<< extra << std::endl <<"!!!" << std::endl;
    join_up_receives(body);
    // std::cout << "body thatll be handled :" << body <<std::endl<< "!!!" <<std::endl;
    if(stage == IN_BODY)
    {
        boundary_found = clean_boundary(body,size,before_boundary);//first before_boundary throw away
    }
    
    if(stage == NEED_CONTENT_TYPE)
    { 
        
        //if boundary found//
        std::string contentType = parse_mini_header(body);//returns new contentType -> set it in the map
        
        if(contentType.empty())
        {//need content type next iteration
            insert_raw(res,before_boundary,empty_content);
            fill_extra(body,BEFORE_CONTENT_TYPE);//k (saving whats in body rn(whats after boundary)
        }
        else
        {//->we found content type and we have it (we may have before boundary and we are   after miniheader in "body" and stage INBODY)
        
        //!!make sure this all is good and test
            insert_raw(res,before_boundary,empty_content);
            size_t s = body.size();
            if(clean_boundary(body,s,before_boundary))
            {
                insert_raw(res,before_boundary,contentType);
                fill_extra(body, BEFORE_CONTENT_TYPE);
                body.clear();
                //so it handles all boundaries properly in one call
                BoundaryRetType sec_result = clean_body(body,body.size());
                for(size_t i = 0; i < sec_result.size(); i++)
                    res.push_back(sec_result[i]);
            }
            else
            {
                fill_extra(body,EXTRA100);
                insert_raw(res,body,contentType);
            }

        }
    }
    else
    {
        fill_extra(body,EXTRA100);
        insert_raw(res,body,empty_content);
    }
    
    return res;
};

//return true if boundry found
//if boundary found -> body= purified body after the boundary end
bool BoundaryHandler::clean_boundary(std::string &body, size_t &size, std::string &before_boundary)
{

    size_t pos = find_boundary_start(body);
    if(pos == std::string::npos || pos + boundary.size() + 2 > body.size())//(so boundary ends with \r\n or --)
        return 0;
    stage = NEED_CONTENT_TYPE;
    //clean body from boundary// store whats before boundary
    before_boundary = body.substr(0,pos - 2);//\r\n before boundary
    size_t after_boundary_pos;
    after_boundary_pos = find_boundary_end(body,pos);
    if(after_boundary_pos == std::string::npos)
    {
        error = true;
        //std::cout << "npos clean boundary" << std::endl;
        done = true;
        return 0;
    }

    if(!done)
    {
        body = body.substr(after_boundary_pos, size);
        size = body.size();
    }
    return 1;
}

size_t BoundaryHandler::find_boundary_end(std::string &body,size_t start)
{
    size_t cursor = start;
    const char *strData = body.data();
    size_t strLength = body.size();
    
    for (; cursor <= strLength - 2; cursor++)
    {
        if(memcmp(strData + cursor, "\r\n", 2) == 0)
            return cursor + 2;
        if(cursor - start > 6 && memcmp(strData + cursor, "--", 2) == 0)//to not take firsts ones
        {
            
            //std::cout << "last boundary found" << std::endl;
            //std::cout << body<<std::endl;
            done = true;
            return cursor + 2;
        }
    }
    return std::string::npos;//meaning boundary was syntax failure
}



std::string BoundaryHandler::parse_mini_header(std::string &body)
{//send body after boundary
    //do I parse also contentDisposition//if so return pair of string and map of keyvalue
    size_t header_end_found;
    std::string contentType = get_content_type(body,header_end_found);
    if(header_end_found == std::string::npos)
    {
        stage = NEED_CONTENT_TYPE;
        return(std::string());
    }
    if(contentType.empty())//case only contentDisposition
        contentType = "text/plain";
    

    stage = IN_BODY;
    body = body.substr(header_end_found + 4 ,body.length());

    return contentType;
}

void BoundaryHandler::join_up_receives(std::string &body)
{
    body = extra + body;
    extra.clear();
};


std::string BoundaryHandler::get_content_type(std::string &body, size_t &header_end_found)
{
    size_t pos = findSubstring(body, "\r\n\r\n");
    header_end_found = pos;
    if(pos == std::string::npos)
        return std::string();
    std::string keysDelimeters = ":";
    std::string valuesDelimeters = ";,=:";
    HeaderParser parser(body.substr(0, header_end_found));
    std::string contentType;

    std::map<std::string , std::vector < std::string > >	_headers;
    while (!parser.isDoneParsing())
    {
        std::string requestHeader = parser.getNextToken(keysDelimeters);
        if (!requestHeader.empty())
        {
			bool isErrorOccured = false;
            std::vector<std::string> values = parser.getValuesCurrToken(valuesDelimeters, isErrorOccured);
            _headers[requestHeader] = values;
            if (requestHeader == "Content-Type")
                contentType = values[0];
        }
    }
    return contentType;
    //try to cut typevalue to \r\n if not possible return -
    //  - cant find contentType now, till next() // empty string
    

    //protect it that if its second time in and no content type return error to close client socket
};



//protect it incase its the end then just return the whole extra
void BoundaryHandler::fill_extra(std::string &str, bool type)
{
    //if to cut 100 or to just put the whatever is in str(right whats after boundary) 
    if(type == BEFORE_CONTENT_TYPE)
        extra = str;
    else if(type == EXTRA100)
    {
        if(str.length() > 100)
        {
            extra = str.substr(str.length() - 100, str.length());
            str = str.substr(0, str.length() - 100);
        }
        else
        {
            extra = str;
            str.clear();
        }
    }
};

void BoundaryHandler::set_boundary(std::string& boundary)
{
    this->boundary = "--"+boundary;
    initialized = true;
};

size_t BoundaryHandler::find_boundary_start(std::string &body)
{
    size_t pos = findSubstring(body, boundary);
    return pos;
}


bool BoundaryHandler::is_done()
{
    return done;
};


bool BoundaryHandler::is_initialized()
{
    return initialized;
}
BoundaryHandler::~BoundaryHandler()
{

};

std::string& BoundaryHandler::get_extra()
{
    return extra;
};

bool BoundaryHandler::failed()
{
    return error;
};

void BoundaryHandler::insert_raw(BoundaryRetType& res ,std::string& raw,std::string& contentType)
{
    if(first_boundary)
    {
        raw.clear();
        first_boundary = false;
    }
    else
        res.push_back(std::make_pair(raw,contentType));
};