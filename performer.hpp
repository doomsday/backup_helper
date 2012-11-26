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
        int transferBackups(int argc_p, char* argv_p[]);
        int cleanBackups();
        int sendMail(int argc_p, char* argv_p[]);
        int shutdownSynergy();
};

#endif // PERFORMS_HPP
