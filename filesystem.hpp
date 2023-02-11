#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

//default var// enumeration



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

    std::string generateRandomString(int length)
    {
        std::string result;
        result.reserve(length);
        static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        std::srand(std::time(nullptr));
        for (int i = 0; i < length; ++i)
        {
            result.push_back(alphanum[std::rand() % (sizeof(alphanum) - 1)]);
        }
        return result;
    }

public:
    FileSystem(){};
    FileSystem(std::string& path, Mode mode ,std::string extension = "") : path(path), mode(mode),extension(extension)
    {};
    
    bool open()
    {
        switch(mode)
        {
            case (READ):
                file.open(path, std::ios::in | std::ios::binary);
                break;
            case (WRITE):
                path = path + generateRandomString(4) + extension;
                file.open(path, std::ios::out | std::ios::trunc | std::ios::binary);
                break;
            case (APPEND):
                file.open(path, std::ios::out | std::ios::app | std::ios::binary);
                break;
        }
        return (file.is_open());
    };
    
    void set_extension(std::string extension)
    {
        this->extension = extension;
    }
    void close()
    {
        file.close();
    }

    std::string Read(int max_bytes)
    {
        char buffer[max_bytes + 1];
        if(mode != READ || !file.is_open())
            return std::string();
        file.read(buffer, max_bytes);
        buffer[max_bytes] = 0;
        return std::string(buffer);
    }

    std::string readFile()
    {
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
    };

    void Write(const std::string& contents)  {
        if (file.is_open()) {
            file << contents;
        }
    }

    void Append(const std::string& contents) {
        Write(contents);
    }

    bool is_open(){return file.is_open();}
};

#endif
