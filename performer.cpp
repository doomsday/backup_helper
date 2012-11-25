#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include "performer.hpp"

Performer::Performer(Config *ptr):
    pCnf(ptr)
{}

/* TODO: Add exceptions */
int Performer::transferBackups(int argc_p, char *argv_p[]){
    // search for config values
    string str_backup_source_dir = pCnf->findConfigParamValue("BACKUP", "backup_source_dir");
    string str_backup_dest_host = pCnf->findConfigParamValue("BACKUP", "backup_dest_host");
    string str_backup_dest_host_dir = pCnf->findConfigParamValue("BACKUP", "backup_dest_host_dir");
    // composing string for shell execution
    string str_execute("/usr/bin/find ");
    str_execute+=str_backup_source_dir;
    str_execute+="* -type d -ctime -1 -exec scp -r {} ";
    str_execute+=str_backup_dest_host;
    str_execute+=":";
    str_execute+=str_backup_dest_host_dir;
    str_execute+=" ';'";
    // transforming string to c-string as system() call wants it
    const char* cc_execute = str_execute.c_str();
    // execution...
    executeSh(argc_p, argv_p, cc_execute);
    return 0;
}

/* TODO: 1. Remove usage of argc-argv. It's not necessary
 * 2. Add exceptions */
int Performer::cleanBackups(int argc_p, char *argv_p[]){

    string str_backup_source_dir = pCnf->findConfigParamValue("BACKUP", "backup_source_dir");

    string str_execute("/usr/bin/find ");
    str_execute+="/var/backups/synergy/* -type d -ctime +7 -delete";

    const char* cc_execute = str_execute.c_str();

    executeSh(argc_p, argv_p, cc_execute);
    return 0;
}

/* TODO: Remove usage of argc-argv. It's not necessary
 * 2. Add exceptions */
int Performer::sendMail(int argc_p, char *argv_p[]){

    string str_email_from = pCnf->findConfigParamValue("NOTIFICATION", "email_from");
    string str_email_to = pCnf->findConfigParamValue("NOTIFICATION", "email_to");

    string str_execute("echo \"Backups has been (hopefully) made at: $(date).\" | mail -s \"Medicare: Backups has just been made\" -r ");
    str_execute+=str_email_from;
    str_execute+=" ";
    str_execute+=str_email_to;

    const char* cc_execute = str_execute.c_str();

    executeSh(argc_p, argv_p, cc_execute);
    return 0;
}

/* TODO: Remove usage of argc-argv. It's not necessary
 * 2. Add exceptions */
void Performer::executeSh(int argc_p, char *argv_p[], const char *stringToExecute){

    pid_t cpid, w;
    int status;

    cpid = system(stringToExecute);                                             // /usr/bin/find: `/var/backups/synergy/*': No such file or directory
    if (cpid == -1) {
        throw ShellExecuteError(strerror(errno));                               // strerror: Get pointer to error message string
    }
    if (cpid == 0) {
        std::cout << "Child PID is" << (long)getpid();
        if (argc_p == 1)
            pause();
        _exit(atoi(argv_p[1]));
    } else {                                                                    // got not error but "the return status of the command"
        do {
            w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);                 // waiting the child process to perform requested actions
            if (w == -1) {
                throw ShellExecuteError(strerror(errno));
            }
            if (WIFEXITED(status)) {                                            /* This macro queries the child termination status provided by the wait and waitpid functions,
                                                                                   and determines whether the child process ended normally */
                std::cout << "exited, status=" << WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                std::cout << "killed by signal " << WTERMSIG(status);
            } else if (WIFSTOPPED(status)) {
                std::cout << "stopped by signal" << WSTOPSIG(status);
            } else if (WIFCONTINUED(status)) {
                std::cout << "continued\n";
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        exit(EXIT_SUCCESS);
    }
}

int Performer::shutdownSynergy() {
    using std::ifstream;
    using std::ios;

    string str_pidfile;
    str_pidfile = pCnf->findConfigParamValue("GENERAL", "synergy_pidfile");
    const char* cc_pidfile = str_pidfile.c_str();

    // prepare to open
    int length;
    char* buffer(0);
    int killwait_counter;
    ifstream is;
    // open
    is.exceptions ( ifstream::failbit | ifstream::badbit );                     // exception mask
    try {
        is.open(cc_pidfile, ios::binary);
    } catch (ifstream::failure) {
        throw IOError ("\n1: The following error has occured: Failed to open pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"\n");
    }
    // prepare to read
    is.seekg(0, ios::end);
    length = is.tellg();
    is.seekg(0, ios::beg);
    buffer = new char[length];
    // read
    is.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );                     // exception mask
    try {
        is.read(buffer, length);
    } catch (ifstream::failure) {
        throw IOError ("\n1: The following error has occured: Failed to read pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"\n");
    }
    // kill anyway
    kill(*buffer, SIGKILL);
    do {    // polling
        sleep (1);
        ++killwait_counter;
        if (killwait_counter >= 120) {
//            std::cout << "\nCouldn't stop process with KILL signal. Trying to force termination.\n";
            kill(*buffer, SIGTERM);
            sleep (5);
            if (popen("/bin/pidof java", "r")) {
                throw ProcessManagementError ("Couldn't force termination!\n");
            }
        }
    } while (!access("/var/run/synergy/arta-synergy-jboss.pid", F_OK));

    is.close();
    delete[] buffer;
    return 0;
}
/*
Performs::~Performs(){
    delete stringToExecute;
}
*/
