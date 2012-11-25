#include "performer.hpp"
#include "config.hpp"
#include "globalexceptions.hpp"

#include <iostream>

int main(int argc, char *argv[]) {

    /* INFO:
     * Read configuration file to use it everywhere in the program later
     */

    try {
        Config* cnf = new Config(argc, argv);
        Performer maintenance(cnf);

        try {
            maintenance.shutdownSynergy();
        }
        catch (IOError& e) {                                                    // nothing to do
            throw FatalError (e.errorMessage);
        }
        catch (ProcessManagementError &e) {                                     // nothing to do
            throw FatalError (e.errorMessage);
        }
        catch (ShellExecuteError &e){                                           // nothing to do
            throw FatalError (e.errorMessage);
        }

        maintenance.transferBackups(argc, argv);
        maintenance.cleanBackups(argc, argv);
        maintenance.sendMail(argc, argv);
        delete cnf;
    }
    catch (FatalError& e){
        std::cout << e.errorMessage;
        std::cout << "\n2: Fatal Error. Unable to process\n";
        return 1;
    }

    /* INFO:
     * Do not forget to clean up everything, if you are not a khuesos
     */
    return 0;
}
