#ifndef PERFORMS_HPP
#define PERFORMS_HPP

#include <globalexceptions.hpp>

class Performer
{
        void executeSh(int argc_p, char* argv_p[], char* stringToExecute);
    public:
//        ~Performs();
        int transferBackups(int argc_p, char* argv_p[]);
        int cleanBackups(int argc_p, char* argv_p[]);
        int sendMail(int argc_p, char* argv_p[]);
        int shutdownSynergy();
};

#endif // PERFORMS_HPP
