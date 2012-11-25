#ifndef PERFORMS_HPP
#define PERFORMS_HPP

#include "globalexceptions.hpp"
#include "config.hpp"

class Performer
{
        Config* pCnf;
        void executeSh(int argc_p, char* argv_p[], const char *stringToExecute);
    public:
//        ~Performs();
        Performer(Config *ptr);
        int transferBackups(int argc_p, char* argv_p[]);
        int cleanBackups(int argc_p, char* argv_p[]);
        int sendMail(int argc_p, char* argv_p[]);
        int shutdownSynergy();
};

#endif // PERFORMS_HPP
