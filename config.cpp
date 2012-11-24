#include "config.hpp"
#include "configparser.hpp"
#include "globalexceptions.hpp"

int Config::readConfig(int argc_p, char *argv_p[])
{
    using std::cout;
    using std::ifstream;
    using std::vector;
    using std::string;
    using std::getline;

    if ( argc_p != 4 )
    {
        FatalError e("Sick usage. Try: <file.ini> <section> <parameter>\n");
        throw e;
    }

    ifstream in(argv_p[1]);
    if( !in )
    {
        IOError e("Can't open requested configuration file\n");
        throw e;
    }

    vector<string> lns;
    string s;

    /* конфиг считывается строками до конца, заполняя вектор строк lns */
    while ( !in.eof() )
    {
        getline( in, s );
        boost::algorithm::trim(s);
        lns.push_back( s+='\n');
    }
    /* 1. # iterator erase( iterator start, iterator end )
     *    deletes the elements between start and end (including start but not including end). The return value is the element after the last element erased
     * 2. # iterator remove_if( iterator start, iterator end, Predicate p )
     *    removes all elements in the range [start,end] for which the predicate p returns true
     */
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
    if (find_value(data, argv_p[2], argv_p[3], res))
        cout << res; // TODO: Replace for our needs
    cout << endl;
    return 0; // TODO: Check
}
