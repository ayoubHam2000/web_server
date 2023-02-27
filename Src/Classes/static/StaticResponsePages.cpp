# include "static/StaticResponsePages.hpp"

std::map<int, std::string> StaticResponsePages::S_InitResponsePages() {
	std::map < int  , std::string> responsePages;
	responsePages[NOT_FOUND] = PATH_404;
	responsePages[BAD_REQUEST] = PATH_400;
	responsePages[FORBIDDEN] = PATH_403;
	responsePages[METHOD_NOT_ALLOWED] = PATH_405;
	responsePages[REQUEST_URI_TOO_LONG] = PATH_414;
	responsePages[REQUEST_ENTITY_TOO_LARGE] = PATH_413;
	responsePages[REQUEST_HEADER_TOO_LARGE] = PATH_494;



	responsePages[INTERNAL_SERVER_ERROR] = PATH_500;
	responsePages[NOT_IMPLEMENTED] = PATH_501;


	responsePages[OK] = PATH_200;
	responsePages[CREATED] = PATH_201;
	responsePages[NO_CONTENT] = PATH_204;

	responsePages[MOVED_PERMANETLY] = PATH_301;

	return responsePages;
}
