#ifndef FILE_UTILITIES_H
#define FILE_UTILITIES_H

#include <iostream>
#include <cstdlib>

class FileUtilities
{
public:
    static std::string home();
    static std::string tmp();
    static std::string separator();
    static std::string configFile();
    static std::string pmFile();
    static std::string pmTmpFile();
};
#endif