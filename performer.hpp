#ifndef PERFORMS_HPP
#define PERFORMS_HPP

#include "config.hpp"
#include "logger.hpp"

class Performer
{
        Config* pCnf;
        Logger* pLog;
        int shExecute(const char *stringToExecute) const;
        pid_t getPIDByName(const char * name) const;
        pid_t getIDFromPidfile(string pidfile_path) const;
        bool getStatusFromPID(const pid_t process_id) const;
        int softKill(const pid_t process_id, const char *cc_pidfile_path) const;
        int hardKill(const pid_t process_id) const;
    public:
        Performer(Config *ptr, Logger *lgr);
        int transferBackups() const;
        int cleanBackups() const;
        int sendMail() const;
        int shutdownSynergy() const;
        int startSynergy() const;
};

#endif // PERFORMS_HPP
