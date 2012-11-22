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
#include "performer.hpp"

int Performer::transferBackups(int argc_p, char *argv_p[]){
    /* TODO: 1. check fstab if there is mounted nfs directories
             2. perform non-recursive search to find if a directory is a synergy backup folder
             3. if not - use scp, but only for transferring no more than 5-6GB of data, it takes too much time otherwise
    */
    executeSh(argc_p, argv_p, "/usr/bin/find /var/backups/synergy/* -type d -ctime -1 -exec scp -r {} 192.168.10.195:/var/backups/synergy_reserve ';'");
    return 0;
}
int Performer::cleanBackups(int argc_p, char *argv_p[]){
    executeSh(argc_p, argv_p, "/usr/bin/find /var/backups/synergy/* -type d -ctime +7 -delete");
    return 0;
}
int Performer::sendMail(int argc_p, char *argv_p[]){
    executeSh(argc_p, argv_p, "echo \"Backups has been (hopefully) made at: $(date).\" | mail -s \"Medicare: Backups has just been made\" -r notificator_medicare@medicare.kz support@arta.kz");
    return 0;
}

void Performer::executeSh(int argc_p, char *argv_p[], char *stringToExecute){

    pid_t cpid, w;
    int status;

    cpid = system(stringToExecute);                                             // /usr/bin/find: `/var/backups/synergy/*': No such file or directory
    if (cpid == -1) {
        throw ShellExecuteError(strerror(errno));                                  // strerror: Get pointer to error message string
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
    // prepare to open
    int length;
    char* buffer(0);
    int killwait_counter;
    ifstream is;
    // open
    is.exceptions ( ifstream::failbit | ifstream::badbit );                     // exception mask
    try {
        is.open("/var/run/synergy/arta-synergy-jboss.pid", ios::binary);
    } catch (ifstream::failure) {
        IOError e("\n1: The following error has occured: Failed to open pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"\n");
        throw e;
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
        IOError e("\n1: The following error has occured: Failed to read pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"\n");
        throw e;
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
                processManagementError e("Couldn't force termination!\n");
                throw e;
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
