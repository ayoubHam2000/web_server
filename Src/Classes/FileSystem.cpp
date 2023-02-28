#include "FileSystem.hpp"

FileSystem::FileSystem(std::string &path, Mode mode, std::string extension) : path(path), mode(mode),extension(extension)
{}




bool FileSystem::open() {
	switch(mode)
	{
		case (READ):
			file.open(path, std::ios::in | std::ios::binary);
			break;
		case (WRITE):
			path = path + generateRandomString(6) + extension;
			file.open(path, std::ios::out | std::ios::trunc | std::ios::binary);
			//std::cout << "openedd\n";
			break;
		case (APPEND):
			file.open(path, std::ios::out | std::ios::app | std::ios::binary);
			break;
	}
	return (file.is_open());
}

void FileSystem::set_extension(std::string extension) {
	this->extension = extension;
}

void FileSystem::close() {
	file.close();
}

std::string FileSystem::Read(int max_bytes) {
	char buffer[max_bytes + 1];
	if(mode != READ || !file.is_open())
		return std::string();
	file.read(buffer, max_bytes);
	buffer[max_bytes] = 0;
	return std::string(buffer);
}

std::string FileSystem::readFile() {
	std::string fileContents;
	std::string serverConfigs = "";
	if (is_open())
	{
		std::string sa;
		while (getline(file, sa))
			fileContents += sa + "\n";
		close();
	}
	return (fileContents);
}

void FileSystem::Write_chunk(std::string &content, size_t size) {
	if (file.is_open()) {
		file.write(content.data(),size);
	}
}

void FileSystem::Write_chunk(const char *content, size_t size) {
	if (file.is_open()) {
		file.write(content,size);
	}
}

void FileSystem::Write(const std::string &contents) {
	if (file.is_open()) {
		file << contents;
	}
}

void FileSystem::Append(const std::string &contents) {
	Write(contents);
}

bool FileSystem::is_open() const {return file.is_open();}

std::string FileSystem::generateRandomString(int length) {
	std::string result;
	result.reserve(length);
	static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < length; ++i)
	{
		result.push_back(alphanum[std::rand() % (sizeof(alphanum) - 1)]);
	}
	return result;
}

std::string FileSystem::get_current_dir() {
	std::string current_working_dir;
	char buff[1024]; //create string buffer to hold path
	if (getcwd(buff, 1024 ) != NULL)
		current_working_dir = buff;
	return (current_working_dir);
}

std::string FileSystem::getExtention(std::string &path, bool withPoint) {
	std::string extention;
	size_t pos = path.rfind('.');
	if (pos != std::string::npos)
	{
		if (withPoint)
			pos++;
		extention = path.substr(pos, path.length());
	}
	return (extention);
}

bool FileSystem::file_exists(const char *path) {
	return (access(path, F_OK) == 0);
}

bool FileSystem::isDirectory(const char *path) {
	DIR* dir = opendir(path);
	if (dir != NULL)
	{
		closedir(dir);
		return true;
	}
	return false;
}

void FileSystem::getListOfFiles(const char *path, std::vector<std::string> &list) {
	DIR* dir = opendir(path);
	if (dir != NULL)
	{
		struct dirent* entry;
		entry = readdir(dir);
		while (entry)
		{
			if (entry->d_type == DT_REG || entry->d_type == DT_DIR)
			{
				list.push_back(entry->d_name);
			}
			entry = readdir(dir);
		}
		closedir(dir);
	}
}

int FileSystem::removeEmptyDir(const char *path) {
	return (rmdir(path));
}

int FileSystem::removeFile(const char *path) {
	return (unlink(path));
}

int FileSystem::remove(const char *path) {
	if (isDirectory(path))
		return removeEmptyDir(path);
	else
		return removeFile(path);
}

char *FileSystem::pathJoin(const char *path, const char *name) {
	size_t pathLen = std::strlen(path);
	size_t nameLen = std::strlen(name);
	char *newPath = new char[pathLen + nameLen + 2];
	std::strcpy(newPath, path);
	std::strncat(newPath, "/", 1);
	std::strncat(newPath, name, nameLen);
	return (newPath);
}

bool FileSystem::removeAll(const char *path) {
	DIR* dir = opendir(path);
	//TODO: remove
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
					return (false);
			}
			else if (entry->d_type == DT_DIR && std::strcmp(entry->d_name, ".") != 0 && std::strcmp(entry->d_name, "..") != 0){
				if (!removeAll(filePath))
					return (false);
			}
			delete filePath;
			entry = readdir(dir);
		}
		closedir(dir);
		if (removeEmptyDir(path) != 0)
			return (false);
	}
	else if (file_exists(path))
	{
		if (removeFile(path) != 0)
			return (false);
	}
	else
		return (false);
	return (true);
}

std::string FileSystem::getFullPath(std::string &path) {
	char cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL){
		return std::string(cwd) + "/" + path;
	} else {
		throw std::runtime_error("CWD Failed");
	}
}

const std::string FileSystem::getPath() {
	return (path);
}

FileSystem::FileSystem(const FileSystem &other)
{
	*this = other;
}

FileSystem &FileSystem::operator=(const FileSystem &other) {
	path = other.path;
	mode = other.mode;
	extension = other.extension;
	if(other.is_open())
	{
		mode = APPEND;
		open();
	}
	return (*this);
}
