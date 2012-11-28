#ifndef PERFORMS_HPP
#define PERFORMS_HPP

#include "config.hpp"

class Performer
{
        Config* pCnf;
        int executeSh(const char *stringToExecute);
        pid_t getPIDByName(const char * name);
        pid_t getIDFromPidfile(string pidfile_path);
        bool getStatusFromPID(const pid_t process_id);
        int softKill(const pid_t process_id, const char *cc_pidfile_path);
        int hardKill(const pid_t process_id);
    public:
        Performer(Config *ptr);
        int transferBackups();
        int cleanBackups();
        int sendMail();
        int shutdownSynergy();
};

#endif // PERFORMS_HPP
