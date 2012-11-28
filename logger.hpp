#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "config.hpp";
#include <iostream>
#include <fstream>
#include <string>

using std::string;
using std::ofstream;

class Logger
{
        Config* pCnf;
        ofstream logfile;
        string logfile_path;
    public:
        Logger(Config* ptr);
        ~Logger();

        void logWrite(const string record);
};

#endif // LOGGER_HPP
