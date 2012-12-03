#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "configparser.hpp"
#include <string>

using::std::string;

class Config {
        IniData conf_data;
    public:
        /* NOTE:
         * Parses conf_data to find <value> of <section> <param>
         */
        Config (int argc_p, char* argv_p[]);
        void readConfig (int argc_p, char* argv_p[]);
        string findConfigParamValue (string section, string param) const;
};

#endif // CONFIG_HPP
