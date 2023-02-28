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
# include <fcntl.h>
# include <unordered_map>

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <cerrno>
# include <unistd.h>


# include <dirent.h>
# include <sys/stat.h>

# include "Const.h"
# include "Chunk.h"
# include "FileSystem.hpp"

typedef int													socketType;
typedef unsigned long 										size_type;

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

# define TRACK_WHERE std::string(__FILE_NAME__) + ":" + std::to_string(__LINE__) + ":"
# define read_chunk_size 102400 //100kB

size_t	get_time_ms(void);
void	capitalize(std::string& str);
bool isHexChar(char c);
const std::string&	getMIMEType(const std::string& extension);
const std::string&	getExtension(const std::string& mimeType);
std::string trim(const std::string& str);
#endif //WEB_SERVER_LIBRARIES_H
