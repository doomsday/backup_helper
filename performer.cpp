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
#include <dirent.h>
#include <string>
#include <boost/lexical_cast.hpp>

#include "performer.hpp"

Performer::Performer(Config *ptr):
    pCnf(ptr)
{}

/* TODO: Add exceptions */
int Performer::transferBackups(){
    // search for config values
    string str_backup_source_dir = pCnf->findConfigParamValue("BACKUP", "backup_source_dir");
    string str_backup_dest_host = pCnf->findConfigParamValue("BACKUP", "backup_dest_host");
    string str_backup_dest_host_dir = pCnf->findConfigParamValue("BACKUP", "backup_dest_host_dir");
    string str_backup_dest_user = pCnf->findConfigParamValue("BACKUP", "backup_dest_user");
    // composing string for shell execution
    string str_execute("/usr/bin/find ");
    str_execute+=str_backup_source_dir;
    str_execute+="* -type d -ctime -1 -exec scp -qr {} ";
    str_execute+=str_backup_dest_user;
    str_execute+='@';
    str_execute+=str_backup_dest_host;
    str_execute+=":";
    str_execute+=str_backup_dest_host_dir;
    str_execute+=" ';'";
    // transforming string to c-string as system() call wants it
    const char* cc_execute = str_execute.c_str();
    // execution...
    executeSh(cc_execute);
    return 0;
}

/* TODO: Add exceptions */
int Performer::cleanBackups(){

    string str_backup_source_dir = pCnf->findConfigParamValue("BACKUP", "backup_source_dir");

    string str_execute("/usr/bin/find ");
    str_execute+=str_backup_source_dir;
    str_execute+=" -type d -ctime +7 -delete";

    const char* cc_execute = str_execute.c_str();

    executeSh(cc_execute);
    return 0;
}

/* TODO: Add exceptions */
int Performer::sendMail(){

    string str_email_from = pCnf->findConfigParamValue("NOTIFICATIONS", "email_from");
    string str_email_to = pCnf->findConfigParamValue("NOTIFICATIONS", "email_to");

    string str_execute("echo \"This e-mail was sent using HDSH (Help Desk Services Helper) at $(date).\" | mail -s \"E-mail from HDSH\" ");
    str_execute+=str_email_from;
    str_execute+=" -b ";
    str_execute+=str_email_to;

    const char* cc_execute = str_execute.c_str();

    executeSh(cc_execute);
    return 0;
}

int Performer::shutdownSynergy(){

    pid_t cpid, kpid;
    string pidfile_path;

    pidfile_path = pCnf->findConfigParamValue("GENERAL", "synergy_pidfile");

    try {
        kpid = getIDFromPidfile(pidfile_path);
    } catch (std::runtime_error& e) {
        /* TODO:
         * 1. write to log [ FAIL: Failed to open or read pidfile. Suppose it doesn't exist. Trying to find process automatically ]
         * 2. Procedure of searching synergy java process
         */
    }

    const char* cc_pidfile_path = pidfile_path.c_str();

    cpid = kill(kpid, SIGKILL);
    /* NOTE:
     * В случае успеха, возвращается ноль. При ошибке, возвращается -1 и значение errno устанавливается соответствующим образом.
     */
    if ( cpid == -1 ) {
        throw std::runtime_error(strerror(errno));
    }
    if ( cpid == 0 ) {
        int killwait_counter(0);
        do {
            sleep (1);
            ++killwait_counter;
            /* NOTE:
             * If prosess isn't going to stop, or pidfile is not going to disappear for any reason,
             * and bh.conf says we shall try to SIGTERM it.
             * In this block we shall exit anyway, or infinite loop will happen.
             */
            if ( killwait_counter >= 120 ) {
                if ( pCnf->findConfigParamValue("GENERAL", "term_if_cant_kill") == "1" ) {
                    cpid = kill( kpid, SIGTERM );
                    /* NOTE:
                     * On success (at least one signal was sent), zero is returned.
                     * On error, -1 is returned, and errno is set appropriately.
                     */
                    if ( cpid == -1 ) {
                        throw std::runtime_error(strerror(errno));
                    } else if ( cpid == 0 ) {
                        sleep (5);
                        cpid = getPIDByName("java");
                        /* NOTE:
                         * The return value of procFind is -1 if no processed was found or can't open /proc
                         * directory. Otherwise the return value is processe's PID
                         */
                        if (cpid = -1) {
                            /* TODO:
                             * 1. It's possible to more than one Java processes to coexist
                             * if it is, we will think that we couldn't kill kill it. So it's necessary
                             * to check existence of process by it's pid that has previously been detected
                             * 2. Check unlink() results
                             */
                            /* NOTE:
                             * If killed successfully - go away
                             */
                            unlink(cc_pidfile_path);
                            return 0;
                        } else {
                            throw std::runtime_error("\nIt seems that even after SIGTERM the Java VM process is still alive\n");
                        }
                    } else {
                        throw std::runtime_error("\nkill() returned impossible value\n");
                    }
                } else if ( pCnf->findConfigParamValue("GENERAL", "term_if_cant_kill") == "0" ) {
                    throw std::runtime_error("\nCould not SIGKILL process, and according to the bh.conf did not tried to SIGTERM it\n");
                } else {
                    throw std::runtime_error("\nInvalid \"term_if_cant_kill\" value inf bh.conf\n");
                }
            }
        } while (!access(cc_pidfile_path, F_OK));
    }

    return 0;
}

int Performer::executeSh(const char *stringToExecute){

    pid_t cpid, w;
    int status;

    /* INFO:
     *
     * DESCRIPTION
     * system()  executes  a command specified in command by calling /bin/sh -c command, and returns after the command has been completed.
     * During execution of the command, SIGCHLD will be blocked, and SIGINT and SIGQUIT will be ignored.
     *
     * RETURN VALUE
     * The value returned is -1 on error (e.g., fork(2) failed), and the return status of the command otherwise.  This latter return
     * status is in the format specified in wait(2).  Thus, the  exit  code of the command will be WEXITSTATUS(status).  In case
     * /bin/sh could not be executed, the exit status will be that of a command that does exit(127).
     *
     * system() does not affect the wait status of any other children.
     */
    cpid = system(stringToExecute);                                             // /usr/bin/find: `/var/backups/synergy/*': No such file or directory

    if ( cpid == -1 ) {
        throw std::runtime_error(strerror(errno));                               // strerror: Get pointer to error message string
    }
    if ( cpid == 0 ) {
        std::cout << "Done. Child process PID was: " << (long)getpid() << std::endl;
        return(EXIT_SUCCESS);
    } else {                                                                    // got not error but "the return status of the command"
        do {
            /* INFO:
             * The waitpid() system call suspends execution of the calling process until a child specified by pid argument has changed state.
             * By default, waitpid() waits only for terminated children, but this behavior is modifiable via the options argument.
             *
             * WUNTRACED   also return if a child has stopped (but not traced via ptrace(2))
             * WCONTINUED  also return if a stopped child has been resumed by delivery of SIGCONT
             *
             * The value of pid can be:
             * < -1   meaning wait for any child process whose process group ID is equal to the absolute value of pid.
             * -1     meaning wait for any child process.
             * 0      meaning wait for any child process whose process group ID is equal to that of the calling process.
             * > 0    meaning wait for the child whose process ID is equal to the value of pid.
             */
            w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);                 // waiting the child process to perform requested actions
            if (w == -1) {
                throw std::runtime_error(strerror(errno));
            }
            /* NOTE:
             * This macro queries the child termination status provided by the wait() and waitpid() functions, and determines whether the child process ended normally
             */
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
        return(EXIT_SUCCESS);
    }
}

pid_t Performer::getPIDByName(const char* name)
{
    /* INFO:
     * Return values:
     * "-1" if error or process not found
     * "PID" otherwise
     *
     * DECRIPTION:
     * Search process' PID using traversal around /proc directory. An alternative could bin using "pidof",
     * but it's not so interesting and it relies on the external program
     */
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];
    /* NOTE:
     * The opendir() function opens a directory stream corresponding to the directory name,
     * and returns a pointer to the directory stream. The stream is positioned at the first
     * entry in the directory
     */
    if ( !(dir = opendir("/proc")) ) {
        perror("can't open /proc");
        return -1;
    }
    /* NOTE:
     * The  readdir()  function returns a pointer to a dirent structure representing the next
     * directory entry in the directory stream pointed to by dirp.
     * It returns NULL on reaching the end of the directory stream or if an error occurred.
     */
    while( (ent = readdir(dir)) != NULL ) {
        /* INFO:
         * If endptr is not a null character, the directory is not entirely numeric, so ignore it.
         *
         * long int strtol(const char *nptr, char **endptr, int base)
         *
         * The strtol() function converts the initial part of the string in nptr to a long integer
         * value according to the given base, which must be between 2 and 36 inclusive, or be the
         * special value 0
         *
         * If endptr is not NULL, strtol() stores the address of the first invalid character in *endptr.
         * If there were no digits at all, strtol() stores the original  value  of  nptr  in  *endptr
         * (and returns 0).  In particular, if *nptr is not '\0' but **endptr is '\0' on return, the
         * entire string is valid
         */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if ( *endptr != '\0' ) {
            continue;
        }
        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if ( fp ) {
            if ( fgets(buf, sizeof(buf), fp ) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if ( !strcmp(first, name) ) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }
    }

    closedir(dir);
    return -1;
}

pid_t Performer::getIDFromPidfile(string pidfile_path){
    using std::ifstream;
    using std::ios;

    const char* cc_pidfile_path = pidfile_path.c_str();
    // prepare to open
    int length;
    char* buffer(0);
    ifstream is;
    // open
    is.exceptions ( ifstream::failbit | ifstream::badbit );                     // exception mask
    try {
        is.open(cc_pidfile_path, ios::binary);
    } catch (ifstream::failure) {
        throw std::runtime_error("\n1: The following error has occured: Failed to open pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"\n");
    }
    // prepare to read
    is.seekg(0, ios::end);
    length = is.tellg();
    is.seekg(0, ios::beg);
    buffer = new char[length];
    // read
    is.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );
    try {
        is.read(buffer, length);
    } catch (ifstream::failure) {
        throw std::runtime_error("\n1: The following error has occured: Failed to read pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"\n");
    }
    is.close();

    pid_t cpid = atoi(buffer);

    delete[] buffer;
    return cpid;
}

bool Performer::getStatusFromPID(const pid_t process_id){

    string proc_path("/proc/");
    string proc_pid = boost::lexical_cast<string>(process_id);

    proc_path+=proc_pid;

    if ( !(opendir(proc_path.c_str())) ) {
        perror("can't open /proc");
        return 0;
    } else {
        return 1;
    }
}
