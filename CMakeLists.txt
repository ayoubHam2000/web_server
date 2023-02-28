cmake_minimum_required(VERSION 3.24)
project(web_server)
project(dummy_client)

set(CMAKE_CXX_STANDARD 98)

include_directories(web_server Includes)
include_directories(web_server Src/Headers)

add_executable(web_server main.cpp Includes/Libraries.h Src/Headers/ChunkContentHandler.hpp Includes/FileSystem.hpp Src/Classes/MyWebServer.cpp Src/Headers/MyWebServer.h Src/Classes/Configurations.cpp Src/Headers/Configurations.h Src/Classes/ServerConfig.cpp Src/Headers/ServerConfig.h Src/Classes/LocationConfig.cpp Src/Headers/LocationConfig.h Src/Classes/SyntaxTree.cpp Src/Headers/SyntaxTree.h Src/Classes/Server.cpp Src/Headers/Server.h Src/Classes/Client.cpp Src/Headers/Client.h Src/Utils/Utils.cpp Src/Classes/Header.cpp Src/Headers/Header.h Src/Classes/HeaderPath.cpp Src/Headers/HeaderPath.h Src/Classes/BodyChunk.cpp Src/Headers/BodyChunk.h Src/Classes/ChunkContentHandler.cpp Src/Classes/Response.cpp Src/Headers/Response.h Src/Classes/MyBuffer.cpp Src/Headers/MyBuffer.h Src/Classes/CGI.cpp Src/Headers/CGI.h Src/Classes/ClientInfo.cpp Src/Headers/ClientInfo.h Src/Classes/Const.cpp Src/Headers/Const.h Src/Classes/Chunk.cpp Src/Headers/Chunk.h)

add_executable(dummy_client dummyClient.cpp)
