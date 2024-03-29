#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "config.hpp"
#include <iostream>
#include <fstream>
#include <string>

using std::string;
using std::ofstream;

class Logger {
        std::shared_ptr<Config> pCnf;
        ofstream logfile;
        string logfile_path;
    public:
        Logger(std::shared_ptr<Config> ptr);
        ~Logger();

        Logger& operator<< (const string& record);
        Logger& operator<< (const long& record);

        string date() const;
};

#endif // LOGGER_HPP
