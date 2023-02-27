/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 17:48:36 by mel-amma          #+#    #+#             */
/*   Updated: 2023/02/27 14:02:31 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostRequest.hpp"
#include "ChunckContentHandler.hpp"

PostRequest::PostRequest()
{
    file_initialized = false;
    is_chunked = false;
	received = 0;
};

bool PostRequest::post_init()
{
	std::map<std::string , std::vector < std::string > >::iterator it = _headers.find("Content-Length");
    if (it != _headers.end())
    {
        std::vector<std::string> content_type_vector = it->second;
        std::string content_len = content_type_vector[0];
        body_length = std::stoi(content_len);
		
    }
    else
    {
        body_length = 0;
        is_chunked = true;
    }
    received = 0;
    file_initialized = false;
    if(is_chunked)
    {
		std::map<std::string , std::vector < std::string > >::iterator it = _headers.find("Transfer-Encoding");
        if (it == _headers.end() || it->second[0] != "chunked")
            return false;
    }
    return (true);
}


bool PostRequest::open_file(std::string &contentType, Client &client)
{
	std::string pathDir;
	if (client.isForCgi)
		pathDir = "/tmp/";
	else
		pathDir = client.clientInfos._bestLocationMatched->getUploadPass();
	if (FileSystem::isDirectory(pathDir.c_str()))
	{
		std::cout << "created File\n";
		pathDir += "_upload";
		// upload_store check if its there, check upload pass or just put in default upload path
		fs = FileSystem(pathDir /*get best match*/, WRITE, ContentTypes::getExtention(contentType));
		fs.open();
		if (client.isForCgi)
			client.clientInfos._cgiFilePath = fs.getPath();
		file_initialized = true;
		return (true);
	}
	else
	{
		setBodyAsFinished(client, FORBIDDEN);
		return (false);
	}
	
}

/*  send in the buffer itself or its address and the size to write  */
void PostRequest::handleRequest(std::string &body, size_t size, Client &client)
{
	//size is what received read
    // std::cout << body << std::endl;
	size_t total_size = body.length();
	std::map<std::string , std::vector < std::string > >::iterator it = _headers.find("Content-Type");
	std::vector<std::string> content_type_vector;
    std::string content_type;
    if (it != _headers.end())
    {
       	content_type_vector = it->second;
        content_type = content_type_vector[0];
	}

    if (!file_initialized)
    {  
        if(!post_init())
        {
            client.set_response_code(BAD_REQUEST);
            client.finished_body();
            return ;
        }
        if(!client.clientInfos._serverConfigs->size_limit_found())
            size_limit = std::string::npos;
        else
            size_limit = client.clientInfos._serverConfigs->getMaxClientBodySize(); 
    }
    
    std::vector<const char *>  chunks;
    if(is_chunked)
    {
        if(!chunk_handler.getHttpChunkContent(body.c_str(),size,chunks))
        {
            client.set_response_code(BAD_REQUEST);
            client.finished_body();
            return ;
        }
    }
   	if (content_type == "multipart/form-data" && !client.isForCgi)
    {
        if(is_chunked)
        {
            for (size_t i = 0; i < chunks.size(); i +=2)
            {
                std::string chunk_body(chunks[i],chunks[i + 1]- chunks[i]);
                if(!handle_boundary(chunk_body,size,client))
                    return ;
            }

        }
        else if(!handle_boundary(body,size,client))
            return;

    }
    else
    {
        if(!file_initialized && !open_file(content_type, client))
			return ;
        is_chunked? write_body(chunks,size): write_body(body,size, total_size) ;
    }
    if(received > size_limit)
    {
        client.set_response_code(REQUEST_ENTITY_TOO_LARGE);
        client.finished_body();
        return ;
    }

    if (!is_chunked && (body_length <= received || boundary_handler.is_done()))
    {
        fs.close();
        setBodyAsFinished(client);
    }
    else if (is_chunked && chunk_handler.is_done())
    {
        fs.close();
        setBodyAsFinished(client);
    }
};


void PostRequest::write_body(std::string& body, size_t size, size_t total_size)
{
    received += total_size;
    fs.Write_chunk(body,size);
}

void PostRequest::write_body(std::vector<const char *>  &chunks, size_t )
{
    for (size_t i = 0; i < chunks.size(); i +=2)
    {
        const char *a =  chunks[i];
        int _size = chunks[i + 1] - chunks[i];
        received += _size;
        fs.Write_chunk(a,_size);
    }
};


PostRequest::~PostRequest()
{
    if(fs.is_open())
        fs.close();
}

void PostRequest::setBodyAsFinished(Client &client, StatusCode responseCode)
{
	client.finished_body();
	client.set_response_code(responseCode);
}

bool PostRequest::handle_boundary(std::string &body, size_t , Client &client)
{
	size_t total_size = body.size();
    if (!boundary_handler.is_initialized())
    {
		std::map<std::string , std::vector < std::string > >::iterator it = _headers.find("boundary");
        if (it == _headers.end())
        {
            std::cout << "boundary doesnt exist error\n";
            client.set_response_code(BAD_REQUEST);
            client.finished_body();
            return 0;
        }
        boundary_handler.set_boundary(it->second[0]);
    }
    BoundaryHandler::BoundaryRetType res = boundary_handler.clean_body(body, body.size());
    if(boundary_handler.failed())
    {
        client.set_response_code(BAD_REQUEST);
        client.finished_body();
        return (0);
    }
    if (res.size() != 0)
    {
        for (size_t i = 0; i < res.size(); i++)
        {
            if (res[i].first.empty() && res[i].second.empty())
            {
                std::cout << "contiuin" << std::endl;
                continue;
            }
            if (!res[i].second.empty())
            {
                if (fs.is_open())
                    fs.close();
                if (!open_file(res[i].second, client))
					return (0);
            }
            if (fs.is_open())
            {
                write_body(res[i].first, res[i].first.size(), total_size);
            } 
            else
            {
                std::cout << "?" << res[i].first << "?";
                std::cout << "what just happened?" << std::endl;
            }
        }
    }
    return 1;
}