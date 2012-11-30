#include "logger.hpp"
#include <time.h>
#include <ctime>
#include <string>
#include <fstream>

Logger::Logger(Config *ptr):
    pCnf(ptr)
{
    logfile_path = pCnf->findConfigParamValue("GENERAL", "logfile");
    logfile.exceptions ( ifstream::failbit | ifstream::badbit );
    try {
        logfile.open(logfile_path, fstream::in | fstream::out | fstream::app);
    } catch (fstream::failure) {
        throw std::logic_error("\nCannot open logfile\n");
    }
}

Logger::~Logger(){
    logfile.close();
//    delete pCnf;
}

Logger& Logger::operator<< (const string& record) {
    logfile << record;
    logfile.flush();

    return *this;
}

Logger& Logger::operator<< (const long& record) {
    logfile << record;
    logfile.flush();

    return *this;
}

string Logger::date(){
    /* INFO:
     * Returns string with newline character at the beginning and space at the end
     */
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    string str_time(asctime(timeinfo));
    str_time.erase(str_time.end()-1);

    return ( string("\n") += str_time += " " );
}
