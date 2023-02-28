# include "static/StaticResponseMessages.hpp"

std::map < StatusCode  , std::string> StaticResponseMessages::MAPPING_RESPONSE_CODE_TO_MESSAGES = StaticResponseMessages::S_initResponseMessages();

std::map<StatusCode, std::string> StaticResponseMessages::S_initResponseMessages() {
	std::map<StatusCode, std::string> Messages;
	Messages[OK] = " OK ";
	Messages[CREATED] = " Created ";

	Messages[BAD_REQUEST] = " Bad Request ";
	Messages[NOT_FOUND] = " Not Found ";
	Messages[BAD_REQUEST] = " Bad Request ";
	Messages[METHOD_NOT_ALLOWED] = " Method Not Allowed ";
	Messages[FORBIDDEN] = " Forbidden ";
	Messages[REQUEST_ENTITY_TOO_LARGE] = " Request Entity Too Large ";
	Messages[REQUEST_HEADER_TOO_LARGE] = " Request Header Too Large ";
	Messages[REQUEST_URI_TOO_LONG] = " Request Uri Too Long ";


	Messages[INTERNAL_SERVER_ERROR] = " Internal Server Error ";
	Messages[NOT_IMPLEMENTED] = " Not Implemented ";

	Messages[MOVED_PERMANETLY] = " Moved Permanetly ";
	Messages[FOUND] = " Found ";
	Messages[PERMANENT_REDIRECT] = " Permanent Redirect";
	return (Messages);
}

std::string &StaticResponseMessages::getMessageResponseCode(StatusCode responseCode) {
	return (MAPPING_RESPONSE_CODE_TO_MESSAGES[responseCode]);
}
