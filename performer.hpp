#ifndef PERFORMS_HPP
#define PERFORMS_HPP

#include "config.hpp"

class Performer
{
        Config* pCnf;
        int executeSh(const char *stringToExecute);
        pid_t getPIDByName(const char * name);
        pid_t getIDFromPidfile(string pidfile_path);
    public:
        bool getStatusFromPID(const pid_t process_id);
        Performer(Config *ptr);
        int transferBackups();
        int cleanBackups();
        int sendMail();
        int shutdownSynergy();
};

#endif // PERFORMS_HPP
