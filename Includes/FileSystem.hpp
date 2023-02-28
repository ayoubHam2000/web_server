/***************************************************/
/*     created by TheWebServerTeam 2/14/23         */
/***************************************************/

#ifndef WEB_SERVER_FILESYSTEM_HPP
#define WEB_SERVER_FILESYSTEM_HPP

# include <dirent.h>
# include <sys/stat.h>
# include <unistd.h>
# include <iostream>
# include <string>
# include <strstream>

class FileSystem{
public:
	enum {
		T_DIR, T_RF
	};

	static std::string generateRandomName(){
		const int	length = 7;
		const int	buffer_size = 100;
		char		name[length + 1];
		char		buffer[buffer_size];
		int			i = 0;
		std::time_t time = std::time(NULL);

		std::ifstream file("/dev/random");
		if (!file.is_open())
			throw std::runtime_error("Error can't open /dev/random");
		file.read(buffer, buffer_size);
		for (int j = 0; j < buffer_size; j++){
			if (std::isalnum(buffer[j]))
				name[i++] = buffer[j];
			if (i == length){
				name[i] = 0;
				break ;
			}
		}
		std::stringstream ss;
		ss << std::put_time(std::gmtime(&time), "%d%m%Y%_%H%M%S") << "__" << name;
		return (ss.str());
	}

	static const char *fileExtension(const std::string& file){
		return std::strrchr(file.c_str(), '.');
	}

	static bool isPathDotDot(const std::string& path){
		std::size_t start = 0;
		std::size_t end = path.find('/', start);
		while (end != std::string::npos) {
			std::string segment = path.substr(start, end - start);
			start = end + 1;
			end = path.find('/', start);
			if (segment == ".." || segment == "."){
				return (true);
			}
		}
		std::string segment = path.substr(start);
		if (segment == ".." || segment == "."){
			return (true);
		}
		return (false);
	}

	static std::string removeDotDot(const std::string& path) {
		std::vector<std::string> list;
		std::vector<std::string> stack;
		std::string res;
		bool 		isAbs = false;

		if (!path.empty() && path[0] == '/')
			isAbs = true;
		std::size_t start = 0;
		std::size_t end = path.find('/', start);
		while (end != std::string::npos) {
			std::string segment = path.substr(start, end - start);
			start = end + 1;
			end = path.find('/', start);
			if (!segment.empty())
				list.push_back(segment);
		}
		std::string s = path.substr(start);
		if (!s.empty())
			list.push_back(s);
		for (std::vector<std::string>::iterator iter = list.begin(); iter != list.end(); ++iter){
			if (stack.empty())
				stack.push_back(*iter);
			else if (*iter == ".."){
				if (*(stack.end() - 1) == ".." || *(stack.end() - 1) == ".")
					throw std::runtime_error("Wrong Path");
				stack.pop_back();
			} else if (*iter != ".") {
				stack.push_back(*iter);
			}
		}
		for (std::vector<std::string>::iterator iter = stack.begin(); iter != stack.end(); ++iter){
			res += *iter;
			if (iter != stack.end() - 1){
				res += '/';
			}
		}
		if (isAbs)
			res = "/" + res;
		return res;
	}

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

	static void	createFolder(const char* folderName, bool verbose = false){
		int status = mkdir(folderName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (status != 0) {
			throw std::runtime_error("Error creating folder: " + std::string(folderName));
		}
		if (verbose)
			std::cout << "Folder Created: " << folderName << std::endl;
	}

	static void 	createFolderRecursively(const char* folderName, bool verbose = false){
		char *path = new char[strlen(folderName) + 1];
		strcpy(path, folderName);
		char *ptr = path;

		char* pos;
		while (true){
			pos = strchr(ptr, '/');
			if (pos){
				*pos = 0;
				if (!FileSystem::file_exists(path)){
					createFolder(path);
				}
				*pos = '/';
			} else {
				if (!FileSystem::file_exists(path)){
					createFolder(path);
				}
				break;
			}
			ptr = pos + 1;
		}
		delete[] path;
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
		//return (0);
		return (rmdir(path));
	}

	static int removeFile(const char *path){
		//return (0);
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

	static void	removeAll(const char *path, bool verbose = false){
		DIR* dir = opendir(path);


		/*if (!allowed){
			std::cout << "Warning this folder will be removed (0/1): " << path << ": ";
			std::cin >> allowed;
			std::cout << std::endl;
			if (!allowed)
				return ;
		}*/
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
					removeAll(filePath, verbose);
				}
				delete filePath;
				entry = readdir(dir);
			}
			closedir(dir);
			if (removeEmptyDir(path) != 0){
				throw std::runtime_error("can't remove " + std::string(path));
			}
			else if (verbose)
				std::cout << "rm dir: " << path << std::endl;
		} else if (file_exists(path)){
			if (removeFile(path) != 0){
				throw std::runtime_error("can't remove " + std::string(path));
			}
		}
	}



};

#endif //WEB_SERVER_FILESYSTEM_HPP
