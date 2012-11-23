#ifndef CONFIG_HPP
#define CONFIG_HPP

class Config
{
    public:
        int checkConfig();
        int readConfig(int argc_p, char* argv_p[]);
        int createConfig();
        int changeConfig();
};

#endif // CONFIG_HPP
