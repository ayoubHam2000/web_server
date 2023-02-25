FILES = \
	Src/Classes/BodyChunk.cpp \
	Src/Classes/CGI.cpp \
	Src/Classes/ChunkContentHandler.cpp \
	Src/Classes/Client.cpp \
	Src/Classes/ClientInfo.cpp \
	Src/Classes/Configurations.cpp \
	Src/Classes/Header.cpp \
	Src/Classes/HeaderPath.cpp \
	Src/Classes/LocationConfig.cpp \
	Src/Classes/MyBuffer.cpp \
	Src/Classes/MyWebServer.cpp \
	Src/Classes/Response.cpp \
	Src/Classes/Server.cpp \
	Src/Classes/ServerConfig.cpp \
	Src/Classes/SyntaxTree.cpp \
	Src/Utils/Utils.cpp \
	main.cpp

OBJ_DIR = _OUT/

OBJ = $(addprefix $(OBJ_DIR), $(FILES:.cpp=.o))
DEPS = $(OBJ:.o=.d)

DEPFLAGS = -MMD -MF $(@:.o=.d)
CFLAGS =  -I ./Includes -I ./Src/Headers -std=c++98
#-Wall -Wextra -Werror
NAME = web_server

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