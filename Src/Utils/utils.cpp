/***************************************************/
/*     created by TheWebServerTeam 2/18/23         */
/***************************************************/

#include "Utils.hpp"

bool isHexChar(char c) {
	if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'))
		return (true);
	return (false);
}
