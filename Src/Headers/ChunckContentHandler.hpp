/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunckContentHandler.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: klaarous <klaarous@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/20 15:07:36 by klaarous          #+#    #+#             */
/*   Updated: 2023/02/20 15:07:59 by klaarous         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CHUNCKED_
#define CHUNCKED_

#include "includes.hpp"


class ChunkContentHandler{
private:
	static const unsigned int	maxBuffer = 20;
	unsigned long 				expectedContentSize;
	char						nbBuffer[maxBuffer];
	unsigned int				nbBufferCounter;
	short 						contentEnd;
	bool						done;

private:
	ChunkContentHandler(const ChunkContentHandler& other);
	ChunkContentHandler& operator=(const ChunkContentHandler& other);
public:

	ChunkContentHandler(){
		expectedContentSize = 0;
		std::memset(nbBuffer, 0, maxBuffer);
		nbBufferCounter = 0;
		contentEnd = 0;
		done = false;
	}

	~ChunkContentHandler(){}

public:
	bool is_done();

	/**
 	** @brief supply the @param arr with a list of {start_ptr (included) and end_ptr (excluded)} of the chunked content.
	 * @return true if success else false in case of bad hex_number (big than unsigned int or malformed)
 	*/
	bool getHttpChunkContent(const char *chunk, unsigned int chunkSize, std::vector<const char *> &arr);

private:

	bool __parseNb();

	bool _readNb(char c);



};

#endif