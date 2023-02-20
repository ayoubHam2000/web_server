/***************************************************/
/*     created by TheWebServerTeam 2/18/23         */
/***************************************************/

#include "Libraries.h"

size_t	get_time_ms(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000 + t.tv_usec / 1000);
}

bool isHexChar(char c){
	if ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'))
		return (true);
	return (false);
}
