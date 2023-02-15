/***************************************************/
/*     created by TheWebServerTeam 2/14/23         */
/***************************************************/

#ifndef WEB_SERVER_FILESYSTEM_HPP
#define WEB_SERVER_FILESYSTEM_HPP

# include <iostream>
# include <map>
# include <vector>
# include <set>
# include <string>
# include <fstream>
# include <exception>
# include <stack>
# include <sstream>
# include <algorithm>

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

class FileSystem{
private:
	const char *_path;
public:
	enum {
		T_DIR, T_RF
	};
public:
	FileSystem(const std::string& path) : _path(path.c_str()){
		FileSystem(path.c_str());
	}
	FileSystem(const char *path): _path(path){
		if (!file_exists())
			throw std::runtime_error("File Not Found");
	}

	FileSystem(const FileSystem& other){}
	FileSystem& operator=(const FileSystem& other){
		return (*this);
	}
	~FileSystem(){};

public:


	bool file_exists()
	{
		return (access(_path, F_OK) == 0);
	}

	bool directory_exists()
	{
		return (access(_path, F_OK | R_OK | X_OK) == 0);
	}

	bool isDirectory(){
		DIR* dir = opendir(_path);
		if (dir != NULL)
		{
			closedir(dir);
			return true;
		}
		return false;
	}

	void directoryIter(void (*f)(const char *path, int type))
	{
		DIR* dir = opendir(_path);
		if (dir != NULL)
		{
			struct dirent* entry;
			entry = readdir(dir);
			while (entry)
			{
				if (entry->d_type == DT_REG)
				{
					f(entry->d_name, T_RF);
				} else if (entry->d_type == DT_DIR){
					f(entry->d_name, T_DIR);
				}
				entry = readdir(dir);
			}
			closedir(dir);
		}
	}

	bool deleteFile(const char *path, int type){
		return (remove(path) == 0);
	}

	bool deleteFolder(const char *path){
		
	}

};

#endif //WEB_SERVER_FILESYSTEM_HPP
