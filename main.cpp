#include "performer.hpp"
#include "config.hpp"
#include <stdexcept>

#include <iostream>

int main(int argc, char *argv[]) {

    /* INFO:
     * Read configuration file to use it everywhere in the program later
     */

    try {
        Config* cnf = new Config(argc, argv);
        Performer maintenance(cnf);
        maintenance.shutdownSynergy();
        maintenance.transferBackups(argc, argv);
        maintenance.cleanBackups(argc, argv);
        maintenance.sendMail(argc, argv);
        delete cnf;
    }
    catch (std::runtime_error& e){
        std::cout << e.what();
        std::cout << "\n2: Fatal Error. Unable to process\n";
        return 1;
    }

    /* INFO:
     * Do not forget to clean up everything, if you are not a khuesos
     */
    return 0;
}
