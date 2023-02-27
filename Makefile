FILES = \
	Src/Classes/A_Request.cpp \
	Src/Classes/A_Response.cpp \
	Src/Classes/BoundaryHandler.cpp \
	Src/Classes/CGI.cpp \
	Src/Classes/CGIResponse.cpp \
	Src/Classes/ChunckContentHandler.cpp \
	Src/Classes/Client.cpp \
	Src/Classes/ClientInfos.cpp \
	Src/Classes/configParser/ConfigParser.cpp \
	Src/Classes/configParser/Location.cpp \
	Src/Classes/configParser/ServerConfigs.cpp \
	Src/Classes/DeleteRequest.cpp \
	Src/Classes/FileSystem.cpp \
	Src/Classes/GetRequest.cpp \
	Src/Classes/HeaderPath.cpp \
	Src/Classes/Http.cpp \
	Src/Classes/ListClients.cpp \
	Src/Classes/NResponse.cpp \
	Src/Classes/PostRequest.cpp \
	Src/Classes/requestParser/HeaderParser.cpp \
	Src/Classes/Server.cpp \
	Src/Classes/static/ContentTypes.cpp \
	Src/Classes/static/StaticConfig.cpp \
	Src/Classes/static/StaticResponseMessages.cpp \
	Src/Classes/static/StaticResponsePages.cpp \
	Src/Classes/static/SupportedMethods.cpp \
	Src/Utils/HostsManagement.cpp \
	Src/Utils/Utils.cpp \
	main.cpp

OBJ_DIR = _OUT/

OBJ = $(addprefix $(OBJ_DIR), $(FILES:.cpp=.o))
DEPS = $(OBJ:.o=.d)

DEPFLAGS = -MMD -MF $(@:.o=.d)
CFLAGS =  -I ./Src/Includes -I ./Src/Headers  -Wall -Wextra -Werror -std=c++98
#-Wall -Wextra -Werror
NAME = webserv

all: $(NAME)

$(NAME): $(OBJ)
	c++ $(OBJ) -o $(NAME)

$(OBJ): $(OBJ_DIR)%.o:%.cpp
	@mkdir -p $(dir $@)
	c++ $(CFLAGS) -c $(@:$(OBJ_DIR)%.o=%.cpp) $(DEPFLAGS) -o $(@)

-include $(DEPS)

clean:
	rm -rf _OUT

fclean: clean
	rm -f $(NAME)

re: fclean all




