#ifndef PERFORMS_HPP
#define PERFORMS_HPP

#include "config.hpp"

class Performer
{
        Config* pCnf;
        int executeSh(const char *stringToExecute);
        pid_t procFind(const char * name);
    public:
        Performer(Config *ptr);
        int transferBackups();
        int cleanBackups();
        int sendMail();
        int shutdownSynergy();
};

#endif // PERFORMS_HPP
