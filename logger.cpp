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

void Logger::logWrite(const string record){
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    logfile << asctime(timeinfo) << record << std::endl;
}
