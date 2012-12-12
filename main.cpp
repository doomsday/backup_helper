#include "performer.hpp"
#include "config.hpp"
#include "logger.hpp"
#include <iostream>
#include <memory>

#define BH_EXPERIMENTAL

int main(int argc, char *argv[]) {

    std::shared_ptr<Config> cnf;
    std::shared_ptr<Logger> lgr;

    try {
        cnf = std::make_shared<Config>(argc, argv);
    } catch (std::runtime_error& e) {
        ofstream error;
        error.open("/tmp/hdsh.error.log", ofstream::out | ofstream::app);
        error << "SEVERITY [FATAL]: Runtime error: \"" << e.what() << "\"";
        error.close();
        std::cout << "SEVERITY [FATAL]: Initialization failed. See \"/tmp/hdsh.error.log\" for details" << std::endl;
        return 1;
    }

    try {
        lgr = std::make_shared<Logger>(cnf);
    } catch (std::runtime_error& e) {
        ofstream error;
        error.open("/tmp/hdsh.error.log", ofstream::out | ofstream::app);
        error << "SEVERITY [FATAL]: Runtime error: \"" << e.what() << "\"";
        error.close();
        std::cout << "SEVERITY [FATAL]: Initialization failed. See \"/tmp/hdsh.error.log\" for details" << std::endl;
        return 1;
    }

    try {
        /* NOTE:
         * Read configuration file to use it everywhere in the program later
         */
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
