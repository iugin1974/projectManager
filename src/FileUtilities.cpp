#include <iostream>
#include <cstdlib>

#include "FileUtilities.h"

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
            return home()+separator()+"pm.xml";
    }

    std::string FileUtilities::pmTmpFile() {
            return tmp()+separator()+"pm.xml";
    }