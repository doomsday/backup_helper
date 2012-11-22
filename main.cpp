#include <performer.hpp>
#include <globalexceptions.hpp>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

    Performer maintenance;

    try {
        try {
            maintenance.shutdownSynergy();
        }
        catch (IOError& e) {                                                    // nothing to do
            FatalError e1(e.errorMessage);
            throw e1;
        }
        catch (processManagementError &e) {                                     // nothing to do
            FatalError e1(e.errorMessage);
            throw e1;
        }
        catch (ShellExecuteError &e){                                              // nothing to do
            FatalError e1(e.errorMessage);
            throw e1;
        }

        maintenance.transferBackups(argc, argv);
        maintenance.cleanBackups(argc, argv);
        maintenance.sendMail(argc, argv);
    }
    catch (FatalError& e){
        std::cout << e.errorMessage;
        std::cout << "\n2: Fatal Error. Unable to process\n";
        return 1;
    }
    return 0;
}
