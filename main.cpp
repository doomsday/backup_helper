#include "performer.hpp"
#include "config.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    using std::cout;

    try {
        /* NOTE:
         * Read configuration file to use it everywhere in the program later
         */
        Config* cnf = new Config(argc, argv);
        Performer maintenance(cnf);


        maintenance.shutdownSynergy();
//        maintenance.transferBackups(argc, argv);
//        maintenance.sendMail(argc, argv);
//        maintenance.cleanBackups();
        delete[] cnf;
    }
    catch (std::runtime_error& e){
        cout << e.what();
        cout << "\nFatal Error, unable to process. Exit.\n";
        return 1;
    }
    return 0;
}
