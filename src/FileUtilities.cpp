#include <iostream>
#include <cstdlib>

#include "FileUtilities.h"

std::string FileUtilities::file = "";

    std::string FileUtilities::home()
    {
        return std::getenv("HOME");
    }

    std::string FileUtilities::tmp()
    {
        return std::getenv("TMPDIR");
    }

    std::string FileUtilities::separator()
    {
        return "/";
    }

    std::string FileUtilities::configFile()
    {
        return home()+separator()+".pmrc";
    }

    std::string FileUtilities::pmFile() {
if (file.empty())
            return home()+separator()+"pm.xml";
else return file;
    }

    std::string FileUtilities::pmTmpFile() {
            return tmp()+separator()+"pm.xml";
    }

void FileUtilities::setPmFile(const std::string& f) {
file = f;
}