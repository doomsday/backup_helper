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
#include <boost/algorithm/string.hpp>

#include "performer.hpp"

Performer::Performer(std::shared_ptr<Config> ptr, std::shared_ptr<Logger> lgr):
    pCnf(ptr),
    pLog(lgr)
{}

/* TODO: Add exceptions */
int Performer::transferBackups() const {
    // search for config values

    string str_backup_source_dir = pCnf->findConfigParamValue("BACKUP", "backup_source_dir");
    string str_backup_dest_host = pCnf->findConfigParamValue("BACKUP", "backup_dest_host");
    string str_backup_dest_host_dir = pCnf->findConfigParamValue("BACKUP", "backup_dest_host_dir");
    string str_backup_dest_user = pCnf->findConfigParamValue("BACKUP", "backup_dest_user");
    string str_backup_dest_host_port = pCnf->findConfigParamValue("BACKUP", "backup_dest_host_port");
    // composing string for execution
    string str_execute("/usr/bin/rsync");
    str_execute+=" -avzq -e \"ssh -p ";
    str_execute+=str_backup_dest_host_port+="\" ";
    str_execute+=str_backup_source_dir+=" ";
    str_execute+=str_backup_dest_user;
    str_execute+='@';
    str_execute+=str_backup_dest_host;
    str_execute+=":";
    str_execute+=str_backup_dest_host_dir;
    // writing to logfile
    *pLog << pLog->date() << "[INFO]: Starting backups transferring";
    // transforming string to c-string as system() call wants it, and executing
    shExecute(str_execute.c_str());

    return 0;
}

/* TODO: Add exceptions */
int Performer::cleanBackups() const {

    string str_backup_source_dir = pCnf->findConfigParamValue("BACKUP", "backup_source_dir");
    string str_execute("/usr/bin/find ");
    str_execute+=str_backup_source_dir;
    str_execute+=" -type d -ctime +7 -exec rm {} ;";

    *pLog << pLog->date() << "[INFO]: Starting cleaning backups";
    shExecuteExperimental(str_execute.c_str());
    return 0;
}

/* TODO: Add exceptions */
int Performer::sendMail() const {

    string str_email_from = pCnf->findConfigParamValue("NOTIFICATIONS", "email_from");
    string str_email_to = pCnf->findConfigParamValue("NOTIFICATIONS", "email_to");

    string str_execute("echo \"This e-mail was sent using HDSH (Help Desk Services Helper) at $(date).\" | mail -s \"E-mail from HDSH\" ");
    str_execute+=str_email_from;
    str_execute+=" -b ";
    str_execute+=str_email_to;

    *pLog << pLog->date() << "[INFO]: Starting send mail";
    shExecute(str_execute.c_str());
    return 0;
}

int Performer::shutdownSynergy() const {

    bool is_pid_from_pidfile;
    pid_t kpid;
    string pidfile_path = pCnf->findConfigParamValue("GENERAL", "synergy_pidfile");
    string hardkill_or_not = pCnf->findConfigParamValue("GENERAL", "term_if_cant_kill");
    const char* cc_pidfile_path = pidfile_path.c_str();

    *pLog << pLog->date() << "[INFO]: \"synergy_pidfile\" value is " << pidfile_path;
    *pLog << pLog->date() << "[INFO]: \"term_if_cant_kill\" value is " << hardkill_or_not;
    /* NEXT:
     * Get pid from pidfile or any other possible way
     */
    try {
        *pLog << pLog->date() << "[INFO]: Trying to find PID in PID-file";
        kpid = getIDFromPidfile(pidfile_path);
        is_pid_from_pidfile = 1;
        /* NEXT:
         * If pidfile is absent or unaccessible try other method(s)
         */
    } catch (std::exception& e) {
        /* TODO:
         * Procedure of searching synergy-specific java process
         */
        /* NOTE:
         * If getPIDByName fails it will throw and exception that will be caught
         * outside of shutdownSynergy, cause we have nothing to do
         */
        *pLog << pLog->date() << "[WARNING]: Unable to find PID in PID-file";
        *pLog << pLog->date() << "[INFO]: Trying to find PID by process name";
        kpid = getPIDByName("java");
        /* NEXT:
         * If PID found lets go to kill it
         */
        is_pid_from_pidfile = 0;
    }
    /* NEXT:
     * Suppose we found PID of synergy
     */
    try {
        *pLog << pLog->date() << "[INFO]: PID was successfully found: [" << kpid << "]";
        *pLog << pLog->date() << "[INFO]: Trying to send SIGTERM to PID [" << kpid << "]";
        softKill(kpid, cc_pidfile_path);
    }
    catch (std::exception& e) {
        /* NEXT:
         * Failed to send SIGKILL or unable to stop it, don't exactly know why, but anyway lets try to SIGTERM it
         */
        *pLog << pLog->date() << "[WARNING]: Unable to SIGTERM process";
        *pLog << pLog->date() << "[INFO]: Trying to send SIGKILL to PID";
        if (hardkill_or_not == "1") {
            /* NEXT:
             * Run hardkill() and don't catch any exceptions, cause we can do
             * no more with it here
             */
            hardKill(kpid);
            if (is_pid_from_pidfile == 1) {
                *pLog << pLog->date() << "[INFO]: Removing pidfile";
                unlink(cc_pidfile_path);
            }
        } else if ( hardkill_or_not == "0" ) {
            throw std::runtime_error("Could not SIGKILL process, and according to the bh.conf did not tried to SIGTERM it");
        } else {
            throw std::runtime_error("Invalid \"term_if_cant_kill\" value inf bh.conf");
        }
    }
    return 0;
}

int Performer::startSynergy() const {
    string start_synergy("/etc/init.d/arta-synergy-jboss start");

    *pLog << pLog->date() << "[INFO]: Starting Synergy";
    shExecuteExperimental(start_synergy.c_str());

    return 0;
}

int Performer::shExecute(const char* stringToExecute) const {
    pid_t child_pid, w;
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
    child_pid = system(stringToExecute);                                             // /usr/bin/find: `/var/backups/synergy/*': No such file or directory

    if ( child_pid == -1 ) {
        throw std::runtime_error(strerror(errno));                               // strerror: Get pointer to error message string
    }
    if ( child_pid == 0 ) {
        *pLog << pLog->date() << "[INFO]: Done. Child process PID is: " << (long)getpid();
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
            w = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);                 // waiting the child process to perform requested actions
            if (w == -1) {
                throw std::runtime_error(strerror(errno));
            }
            /* NOTE:
             * This macro queries the child termination status provided by the wait() and waitpid() functions, and determines whether the child process ended normally
             */
            if (WIFEXITED(status)) {
                *pLog << pLog->date() << "[INFO]: Exited, status=" << WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                *pLog << pLog->date() << "[WARNING]: Killed by signal " << WTERMSIG(status);
            } else if (WIFSTOPPED(status)) {
                *pLog << pLog->date() << "[WARNING]: Stopped by signal" << WSTOPSIG(status);
            } else if (WIFCONTINUED(status)) {
                *pLog << pLog->date() << "[INFO]: Continued";
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        *pLog << pLog->date() << "[INFO]: Process completed";
        return(EXIT_SUCCESS);
    }
}

int Performer::shExecuteExperimental(const char* stringToExecute) const {

    namespace ba = boost::algorithm;

    string s(stringToExecute);
    vector<string> fields;
    ba::split(fields, s, ba::is_any_of(" "));

    const char* argumentsArray[fields.size()+1];

    for (unsigned int i = 0; i < fields.size(); ++i) {
        argumentsArray[i] = fields[i].c_str();
    }
    /* NEXT:
     * We need whitespace at the end of any argument ov execv(), but boost::split
     * erased it, of course, so lets restore */
    argumentsArray[fields.size()] = NULL;

    pid_t child_pid, w;
    int status;

    child_pid = fork();

    if ( child_pid == 0 ) {
        /* NOTE:
         * This is done by the child process because child_pid for it will be 0
         * but for parent child_pid will have the value of PID if the child
         */
        *pLog << pLog->date() << "[INFO]: Child process PID is: " << (long)getpid();
        execv(argumentsArray[0], const_cast<char** const>(argumentsArray));
        /* NOTE:
         * If execv returns, it must have failed */
        throw std::runtime_error(strerror(errno));
    } else {                                                                    // got not error but "the return status of the command"
        do {
            /* INFO:
             * This is run by the parent.  Wait for the child
               to terminate
             */
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
            w = waitpid(child_pid, &status, WUNTRACED | WCONTINUED);                 // waiting the child process to perform requested actions
            if (w == -1) {
                throw std::runtime_error(strerror(errno));
            }
            /* NOTE:
             * This macro queries the child termination status provided by the wait() and waitpid() functions, and determines whether the child process ended normally
             */
            if (WIFEXITED(status)) {
                *pLog << pLog->date() << "[INFO]: Child process Exited, status=" << WEXITSTATUS(status) << (long)getpid();
            } else if (WIFSIGNALED(status)) {
                *pLog << pLog->date() << "[WARNING]: Child process Killed by signal " << WTERMSIG(status);
            } else if (WIFSTOPPED(status)) {
                *pLog << pLog->date() << "[WARNING]: Child process Stopped by signal" << WSTOPSIG(status);
            } else if (WIFCONTINUED(status)) {
                *pLog << pLog->date() << "[INFO]: Child process Continued";
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        *pLog << pLog->date() << "[INFO]: Child process Completed";
        return(EXIT_SUCCESS);
    }
}

pid_t Performer::getPIDByName(const char* name) const {
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
    if ( (dir = opendir("/proc")) == 0 ) {
        throw std::runtime_error(strerror(errno));
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
        } else {
            throw std::runtime_error(strerror(errno));
        }
    }

    closedir(dir);
    throw std::runtime_error ("Unable to find the process ID(PID). Probably it is not exist");
}

pid_t Performer::getIDFromPidfile(string pidfile_path) const {
    using std::ifstream;
    using std::ios;

    const char* cc_pidfile_path = pidfile_path.c_str();
    // prepare to open
    int length;
    ifstream is;
    // open
    is.exceptions ( ifstream::failbit | ifstream::badbit );                     // exception mask
    try {
        is.open(cc_pidfile_path, ios::binary);
    } catch (ifstream::failure) {
        throw std::runtime_error("Failed to open pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"");
    }
    // prepare to read
    is.seekg(0, ios::end);
    length = is.tellg();
    is.seekg(0, ios::beg);

    std::shared_ptr<char> buffer(new char[length]);
    is.exceptions ( ifstream::eofbit | ifstream::failbit | ifstream::badbit );
    try {
        /* NOTE:
         * Smart pointers usually provide a way to access their raw pointer directly.
         * STL smart pointers have a get() member function for this purpose, and
         * CComPtr has a public p class member. By providing direct access to the
         * underlying pointer, you can use the smart pointer to manage memory in
         * your own code and still pass the raw pointer to code that does not support smart pointers
         */
        is.read(buffer.get(), length);
    } catch (ifstream::failure) {
        throw std::runtime_error("Failed to read pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"");
    }
    is.close();

    pid_t cpid = atoi(buffer.get());

    return cpid;
}

bool Performer::getStatusFromPID(const pid_t process_id) const {

    string proc_path("/proc/");
    string proc_pid = boost::lexical_cast<string>(process_id);

    proc_path+=proc_pid;

    if ( !(opendir(proc_path.c_str())) ) {
        //        perror("can't open /proc");
        return 0;
    } else {
        return 1;
    }
}

int Performer::softKill(const pid_t process_id, const char* cc_pidfile_path) const {
    pid_t cpid = kill(process_id, SIGTERM);
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
                throw std::runtime_error("Unable to stop the process");
            }
        } while (access(cc_pidfile_path, F_OK) == 0);
    }
    return 0;
}

int Performer::hardKill(const pid_t process_id) const {

    pid_t cpid = kill( process_id, SIGKILL );
    /* NOTE:
     * On success (at least one signal was sent), zero is returned.
     * On error, -1 is returned, and errno is set appropriately.
     */
    if ( cpid == -1 ) {
        throw std::runtime_error(strerror(errno));
    } else if ( cpid == 0 ) {
        sleep (5);
        cpid = getStatusFromPID(process_id);
        /* NOTE:
         * The return value of procFind is -1 if no processed was found or can't open /proc
         * directory. Otherwise the return value is processe's PID
         */
        if (cpid == 0) {
            /* TODO:
             * 1. It's possible to more than one Java processes to coexist
             * if it is, we will think that we couldn't kill kill it. So it's necessary
             * to check existence of process by it's pid that has previously been detected
             * 2. Check unlink() results
             */
            /* NOTE:
             * If killed successfully - go away
             */
            return 0;
        } else {
            throw std::runtime_error("It seems that even after SIGTERM the Java VM process is still alive");
        }
    } else {
        throw std::runtime_error("kill() returned impossible value");
    }
}
