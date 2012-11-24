#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "configparser.hpp"
#include <string>

using::std::string;

class Config
{
        IniData data;
    public:
        Config(int argc, char *argv[]);
        int checkConfig();
        int readConfig(int argc_p, char* argv_p[]);
        string findConfigParamValue(string section, string param);
        int createConfig();
        int changeConfig();
};

#endif // CONFIG_HPP
