#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <auxiliary.hpp>

class Config : public Auxiliary
{
    public:
        Config();
        int checkConfig();
        int readConfig();
        int createConfig();
        int changeConfig();
};

#endif // CONFIG_HPP
