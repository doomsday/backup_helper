#include "performs.hpp"

/*
 * DESCRIBE ERROR CLASSES
 */

class FailedShExecute{
    public:
        const char* errorMessage;
        FailedShExecute(const char* pErrorMessage);
};
FailedShExecute::FailedShExecute(const char* pErrorMessage):
    errorMessage(pErrorMessage)
{}


int Performs::transferBackups(int argc_p, char *argv_p[]){
    stringToExecute = "/usr/bin/find /var/backups/synergy/* -type d -ctime -1 -exec scp -r {} 192.168.10.195:/var/backups/synergy_reserve ';'";
    executeSh(argc_p, argv_p);
    return 0;
}

int Performs::cleanBackups(int argc_p, char *argv_p[]){
    stringToExecute = "/usr/bin/find /var/backups/synergy/* -type d -ctime +7 -delete";
    executeSh(argc_p, argv_p);
    return 0;
}

int Performs::sendMail(int argc_p, char *argv_p[]){
    stringToExecute = "echo \"Backups has been (hopefully) made at: $(date).\" | mail -s \"Medicare: Backups has just been made\" -r notificator_medicare@medicare.kz support@arta.kz";
    executeSh(argc_p, argv_p);
    return 0;
}

void Performs::executeSh(int argc_p, char *argv_p[]){

    pid_t cpid, w;
    int status;

    try {
        cpid = system(stringToExecute);                                             // /usr/bin/find: `/var/backups/synergy/*': No such file or directory
        if (cpid == -1) {
            throw FailedShExecute(strerror(errno));
        }
        if (cpid == 0) {
            std::cout << "Child PID is" << (long)getpid();
            if (argc_p == 1)
                pause();
            _exit(atoi(argv_p[1]));
        } else {                                                                // got not error but "the return status of the command"
            do {
                w = waitpid(cpid, &status, WUNTRACED | WCONTINUED);             // waiting the child process to perform requested actions
                if (w == -1) {
                    throw FailedShExecute(strerror(errno));                     // strerror: Get pointer to error message string
                }
                if (WIFEXITED(status)) {                                        // This macro queries the child termination status provided by the wait and waitpid functions,
                    // and determines whether the child process ended normally
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
    catch (FailedShExecute &e){
        std::cout << "The following error has occured: " << e.errorMessage;                             // Interprets the value of errno as an error message, and prints it to stderr
        exit(EXIT_FAILURE);
    }

    catch (...){
        //...
    }
}

int Performs::shutdownSnrg(){
    int length;
    char* buffer;

    using std::ifstream;
    using std::ios;

    ifstream is;
    is.exceptions ( ifstream::failbit | ifstream::badbit );                     // exception mask

    try {
        is.open("/var/run/synergy/arta-synergy-jboss.pid", ios::binary);

        is.seekg(0, ios::end);
        length = is.tellg();
        is.seekg(0, ios::beg);

        buffer = new char[length];

        is.read(buffer, length);

        kill(*buffer, SIGKILL);
        // FILE* f = popen("/bin/pidof process_name", "r");
        do {
            sleep (1);
        } while (!access("/var/run/synergy/arta-synergy-jboss.pid", F_OK));
    }
    catch (ifstream::failure e) {
        std::cout << "Failed to open pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"";
        is.close();
    }
    delete[] buffer;
    return 0;
}

Performs::~Performs(){
    delete stringToExecute;
}
