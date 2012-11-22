#include <performs.hpp>
#include <checks.hpp>

using namespace std;

int main(int argc, char *argv[]) {

    Performs Maintenance;

    try {
        try {
            Maintenance.shutdownSnrg();
        }
        catch (ifstream::failure) {
            GeneralFatalError e("\n1: The following error has occured: Failed to open pidfile \"/var/run/synergy/arta-synergy-jboss.pid\"\n");
            throw e;
        }
        catch (processManagementError &e) {
            GeneralFatalError e1(e.errorMessage);
            throw e1;
        }
        Maintenance.transferBackups(argc, argv);
        Maintenance.cleanBackups(argc, argv);
        Maintenance.sendMail(argc, argv);
    }
    catch (shExecuteError &e){
        std::cout << "\n2: Fatal Error! Unable to process. The following error detected: " << e.errorMessage;                             // Interprets the value of errno as an error message, and prints it to stderr
        return 1;
    }
    catch (GeneralFatalError& e){
        std::cout << e.errorMessage;
        std::cout << "\n2: Fatal Error. Unable to process\n";
        return 1;
    }
    return 0;
}
