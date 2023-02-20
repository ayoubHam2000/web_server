/***************************************************/
/*     created by TheWebServerTeam 2/9/23         */
/***************************************************/

#ifndef WEB_SERVER_LIBRARIES_H
#define WEB_SERVER_LIBRARIES_H

# include <iostream>
# include <map>
# include <vector>
# include <set>
# include <string>
# include <csignal>
# include <fstream>
# include <exception>
# include <stack>
# include <sstream>
# include <algorithm>
# include <sys/time.h>
# include <fstream>

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <errno.h>
# include <unistd.h>

typedef int													socketType;
typedef unsigned int										size_type;

size_t	get_time_ms(void);
bool isHexChar(char c);

#endif //WEB_SERVER_LIBRARIES_H
