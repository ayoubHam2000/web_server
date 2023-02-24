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

# include <stdio.h>
# include <dirent.h>
# include <sys/stat.h>
# include <unistd.h>

class FileSystem{
public:
	enum {
		T_DIR, T_RF
	};


	static bool file_exists(const char *path)
	{
		return (access(path, F_OK) == 0);
	}

	static bool isDirectory(const char *path){
		DIR* dir = opendir(path);
		if (dir != NULL)
		{
			closedir(dir);
			return true;
		}
		return false;
	}

	static void getListOfFiles(const char* path, std::vector<std::string> &list)
	{
		DIR* dir = opendir(path);
		if (dir != NULL)
		{
			struct dirent* entry;
			entry = readdir(dir);
			while (entry)
			{
				if (entry->d_type == DT_REG || entry->d_type == DT_DIR)
				{
					std::cout << entry->d_name << std::endl;
					list.push_back(entry->d_name);
				}
				entry = readdir(dir);
			}
			closedir(dir);
		}
	}

	static int removeEmptyDir(const char *path){
		return (0);
		return (rmdir(path));
	}

	static int removeFile(const char *path){
		return (0);
		return (unlink(path));
	}

	static int remove(const char *path){
		if (isDirectory(path))
			return removeEmptyDir(path);
		else
			return removeFile(path);
	}

	static char* pathJoin(const char *path, const char *name){
		size_t pathLen = std::strlen(path);
		size_t nameLen = std::strlen(name);
		char *newPath = new char[pathLen + nameLen + 2];
		std::strcpy(newPath, path);
		std::strncat(newPath, "/", 1);
		std::strncat(newPath, name, nameLen);
		return (newPath);
	}

	static void	removeAll(const char *path, bool verbose = false, bool allowed = false){
		DIR* dir = opendir(path);

		//TODO: remove
		if (!allowed){
			std::cout << "Warning this folder will be removed (0/1): " << path << ": ";
			std::cin >> allowed;
			std::cout << std::endl;
			if (!allowed)
				return ;
		}
		if (dir != NULL)
		{
			struct dirent* entry;
			entry = readdir(dir);
			while (entry)
			{
				char *filePath = pathJoin(path, entry->d_name);
				if (entry->d_type == DT_REG)
				{
					if (removeFile(filePath) != 0)
						std::cerr << "can't remove " << filePath << std::endl;
					else if (verbose)
						std::cout << "rm file: " << filePath << std::endl;
				} else if (entry->d_type == DT_DIR && std::strcmp(entry->d_name, ".") != 0 && std::strcmp(entry->d_name, "..") != 0){
					removeAll(filePath, verbose, allowed);
				}
				delete filePath;
				entry = readdir(dir);
			}
			closedir(dir);
			if (removeEmptyDir(path) != 0)
				std::cerr << "can't remove " << path << std::endl;
			else if (verbose)
				std::cout << "rm dir: " << path << std::endl;
		} else if (file_exists(path)){
			if (removeFile(path) != 0)
				std::cerr << "can't remove " << path << std::endl;
		}
	}



};

#endif //WEB_SERVER_FILESYSTEM_HPP
