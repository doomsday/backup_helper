#include "performs.hpp"

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

    cpid = system(stringToExecute);                                                 // /usr/bin/find: `/var/backups/synergy/*': No such file or directory
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

int Performs::shutdownSnrg(){
    int length;
    char* buffer;

    using std::ifstream;
    using std::ios;

    ifstream is;
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

    is.close();

    delete[] buffer;
    return 0;
}

Performs::~Performs(){
    delete stringToExecute;
}
