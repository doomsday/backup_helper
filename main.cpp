#include "performer.hpp"
#include "config.hpp"
#include "logger.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    using std::cout;

    Config* cnf=0;
    Logger* lgr=0;

    try {
        /* NOTE:
         * Read configuration file to use it everywhere in the program later
         */
        cnf = new Config(argc, argv);
        lgr = new Logger(cnf);

        Performer maintenance(cnf, lgr);

        maintenance.shutdownSynergy();
//        maintenance.startSynergy();
//        maintenance.transferBackups(argc, argv);
//        maintenance.sendMail(argc, argv);
//        maintenance.cleanBackups();
        delete cnf;
    }
    catch (std::runtime_error& e){
        *lgr << lgr->date() << "SEVERITY [ERROR]: Runtime error: \"" << e.what() << "\"";
        cout << "\nFatal Error, unable to process. Exit.\n";
        return 1;
    }
    return 0;
}
