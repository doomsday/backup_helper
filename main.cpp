#include <performs.hpp>
#include <checks.hpp>

using namespace std;

int main(int argc, char *argv[]) {

    Performs Maintenance;

    try {
        try {
            Maintenance.shutdownSnrg();
        }
        catch (IOError& e) {                                                    // nothing to do
            FatalError e1(e.errorMessage);
            throw e1;
        }
        catch (processManagementError &e) {                                     // nothing to do
            FatalError e1(e.errorMessage);
            throw e1;
        }
        catch (shExecuteError &e){                                              // nothing to do
            FatalError e1(e.errorMessage);
            throw e1;
        }

        Maintenance.transferBackups(argc, argv);
        Maintenance.cleanBackups(argc, argv);
        Maintenance.sendMail(argc, argv);
    }
    catch (FatalError& e){
        std::cout << e.errorMessage;
        std::cout << "\n2: Fatal Error. Unable to process\n";
        return 1;
    }
    return 0;
}
