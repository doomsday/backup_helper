#define MEDICARE

#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

#ifdef MEDICARE
const char* scp_transfer = "/usr/bin/find /var/backups/synergy/* -type d -ctime -1 -exec scp -r {} 192.168.10.195:/var/backups/synergy_reserve ';'";
const char* delete_old = "/usr/bin/find /var/backups/synergy/* -type d -ctime +7 -delete";
//const char* send_notification = "echo "Backups has been (hopefully) made at: $(date)." | mail -s "Medicare: Backups has just been made" -r notificator_medicare@medicare.kz support@arta.kz";
#endif

void execute_sh(const char* exec_string, int argc_p, char* argv_p[]) {
    pid_t cpid, w;
    int status;

    cpid = system(exec_string);                                                 // /usr/bin/find: `/var/backups/synergy/*': No such file or directory
    if (cpid == -1) {
        perror("The following error has occured:");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {
        std::cout << "Child PID is" << (long)getpid();
        if (argc_p == 1)
            pause();
        _exit(atoi(argv_p[1]));
    } else {
        do {
            w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);
            if (w == -1) {
                perror("waitpid");                                              // waitpid: No child processes
                exit(EXIT_FAILURE);
            }
            if (WIFEXITED(status)) {
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

int main(int argc, char *argv[]) {

    execute_sh(scp_transfer, argc, argv);
    execute_sh(delete_old, argc, argv);

    delete scp_transfer;
    delete delete_old;
    return 0;
}

