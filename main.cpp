#define MEDICARE

#include <performs.hpp>
#include <checks.hpp>

using namespace std;

int main(int argc, char *argv[]) {

    Performs Maintenance;

    Maintenance.transferBackups(argc, argv);
    Maintenance.cleanBackups(argc, argv);
    Maintenance.sendMail(argc, argv);

    return 0;
}
