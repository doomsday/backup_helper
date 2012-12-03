#include "performer.hpp"
#include "config.hpp"
#include "logger.hpp"
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {

    try {
        /* NOTE:
         * Read configuration file to use it everywhere in the program later
         */
        std::shared_ptr<Config> cnf(new Config(argc, argv));
        std::shared_ptr<Logger> lgr(new Logger(cnf));
        Performer maintenance(cnf, lgr);

        maintenance.shutdownSynergy();
        maintenance.transferBackups();
        maintenance.cleanBackups();
        maintenance.startSynergy();
        maintenance.sendMail();
    }
    catch (std::runtime_error& e) {
        //*lgr << lgr->date() << "SEVERITY [ERROR]: Runtime error: \"" << e.what() << "\"";
        std::cout << "SEVERITY [ERROR]: Runtime error: \"" << e.what() << "\"";
        std::cout << "\nFatal Error, unable to process. Exit.\n";
        return 1;
    }
    return 0;
}
