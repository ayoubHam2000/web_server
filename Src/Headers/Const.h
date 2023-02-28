/***************************************************/
/*     created by TheWebServerTeam 2/26/23         */
/***************************************************/

#ifndef DUMMY_CLIENT_CONST_H
#define DUMMY_CLIENT_CONST_H

#include <unistd.h>

struct Const {
public:
	static const size_t MAX_REQUEST_SIZE = 8192;
	static const size_t MAX_LISTEN = 10000;
};


#endif //DUMMY_CLIENT_CONST_H
