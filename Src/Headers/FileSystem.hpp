#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

//default var// enumeration
#include <unistd.h>

#include <ios>
#include <fstream>
#include "includes.hpp"



enum Mode {
    READ,
    WRITE,
    APPEND
};


//delete directory/file //excve

class FileSystem{
    std::string path;
    Mode mode;
    std::fstream file;
    std::string extension;


public:
    FileSystem(){};

	FileSystem(const FileSystem &other);
	FileSystem& operator= (const FileSystem &other);

    FileSystem(std::string& path, Mode mode ,std::string extension = "");;
    
    bool open();;
    
    void set_extension(std::string extension);
    void close();

    std::string Read(int max_bytes);

    std::string readFile();;

    void Write_chunk(std::string& content, size_t size);

    void Write_chunk(const char *content, size_t size);

    void Write(const std::string& contents);

    void Append(const std::string& contents);

    bool is_open()const;

	static std::string generateRandomString(int length);

	static std::string get_current_dir();

	static std::string getExtention(std::string &path, bool withPoint = false);

	static bool file_exists(const char *path);

	static bool isDirectory(const char *path);

	static void getListOfFiles(const char* path, std::vector<std::string> &list);

	static int removeEmptyDir(const char *path);

	static int removeFile(const char *path);

	static int remove(const char *path);

	static char* pathJoin(const char *path, const char *name);

	static bool	removeAll(const char *path);

	static std::string getFullPath(std::string &path);

	const std::string getPath();
};


#endif