#ifndef PERFORMS_HPP
#define PERFORMS_HPP

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>

#include <globalexceptions.hpp>

class Performer
{
        void executeSh(int argc_p, char* argv_p[]);
        char* stringToExecute;
    public:
//        ~Performs();
        int transferBackups(int argc_p, char* argv_p[]);
        int cleanBackups(int argc_p, char* argv_p[]);
        int sendMail(int argc_p, char* argv_p[]);
        int shutdownSynergy();
};

#endif // PERFORMS_HPP
