#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

#include "config.hpp"
#include "globalexceptions.hpp"
#include "configparser.hpp"

int Config::readConfig(int argc_p, char *argv_p[])
{
    using std::cout;
    using std::ifstream;
    using std::vector;
    using std::string;
    using std::getline;

    if ( argc_p != 4 )
    {
        FatalError e("Usage: <file.ini> <section> <parameter>\n");
        throw e;
    }

    ifstream in(argv_p[1]);
    if( !in )
    {
        IOError e("Can't open file\n");
        throw e;
    }

    vector<string> lns;

    string s;
    while ( !in.eof() )
    {
        getline( in, s );
        boost::algorithm::trim(s);
        lns.push_back( s+='\n');
    }

    lns.erase( remove_if(lns.begin(), lns.end(), is_comment()), lns.end() );
    string text = accumulate( lns.begin(), lns.end(), string() );

    IniData data;
    inidata_parser parser(data); // Our parser
    BOOST_SPIRIT_DEBUG_NODE(parser);

    parse_info<> info = parse(text.c_str(), parser, nothing_p);
    if ( !info.hit )
    {
        FatalError e("Error has been detected in configuration file\n");
        throw e;
    }

    string res;
    if (find_value(data, argv[2], argv[3], res))
        cout << res; // TODO: Replace for our needs
    cout << endl;
}
