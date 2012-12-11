#include "performer.hpp"
#include "config.hpp"
#include "logger.hpp"
#include <iostream>
#include <memory>

#define BH_EXPERIMENTAL

int main(int argc, char *argv[]) {

    std::shared_ptr<Config> cnf(0);
    std::shared_ptr<Logger> lgr(0);

    try {
        /* NOTE:
         * Read configuration file to use it everywhere in the program later
         */
        cnf = std::make_shared<Config>(argc, argv);
        lgr = std::make_shared<Logger>(cnf);

        Performer maintenance(cnf, lgr);

        maintenance.shutdownSynergy();
        maintenance.transferBackups();
        maintenance.cleanBackups();
        maintenance.startSynergy();
        maintenance.sendMail();
    }
    catch (std::runtime_error& e) {
        *lgr << lgr->date() << "SEVERITY [ERROR]: Runtime error: \"" << e.what() << "\"";
        std::cout << "SEVERITY [FATAL]: Runtime error: \"" << e.what() << "\"";
        return 1;
    }
    return 0;
}
