#include "config.hpp"

Config::Config(int argc_p, char* argv_p[]){
    readConfig(argc_p, argv_p);
}

int Config::readConfig(int argc_p, char *argv_p[])
{
    using std::cout;
    using std::ifstream;
    using std::vector;
    using std::string;
    using std::getline;

    char* pConfLocation = new char[sizeof("./bh.conf")+1];

    if ( argc_p == 1 ){
        pConfLocation = "./bh.conf";
    } else if ( argc_p == 2 ){
        pConfLocation = argv_p[1];
    } else {
        throw std::runtime_error("Sick usage. Try: <file.ini>\n");
    }

    ifstream in(pConfLocation);
    if( !in ){
        throw std::runtime_error("Can't open requested configuration file\n");
    }

    vector<string> lns;
    string s;
    /* NOTE:
     * Конфиг считывается строками до конца, заполняя вектор строк lns
     */
    while ( !in.eof() ){
        getline( in, s );
        boost::algorithm::trim(s);
        lns.push_back( s+='\n');
    }
    /* REFERENCE:
     * 1. # iterator erase( iterator start, iterator end )
     *    deletes the elements between start and end (including start but not including end). The return value is the element after the last element erased
     *
     * 2. # iterator remove_if( iterator start, iterator end, Predicate p )
     *    removes all elements in the range [start,end] for which the predicate p returns true. The return value of this function is an iterator to the
     *    last element of the pruned range
     *
     * 3. # TYPE accumulate( iterator start, iterator end, TYPE val );
     *    The accummulate() function computes the sum of val and all of the elements in the range [start,end]
     */
    lns.erase( remove_if(lns.begin(), lns.end(), is_comment()), lns.end() );
    string text = accumulate( lns.begin(), lns.end(), string() );
    /* NOTE:
     * Create and initialize our parser
     */
    inidata_parser parser(conf_data);
    BOOST_SPIRIT_DEBUG_NODE(parser);
    /* NOTE:
     * Теперь запускаем парсер — для этого используется функция parse, которая принимает на вход сам текст, парсер и специальный парсер
     * для пропускаемых символов (скажем, мы хотели бы пропускать все пробелы). В нашем случае парсер для пропускаемых символов будет
     * пустым — nothing_p (т.е. ничего не парсящий). Результатом функции parse является структура parse_info<>
     */
    parse_info<> info = parse(text.c_str(), parser, nothing_p);
    if ( !info.hit ){
        throw std::runtime_error("Error has been detected in configuration file\n");
    }
    return 0; // TODO: Check
}

string Config::findConfigParamValue(string section, string param)
{
    string res;
    if (find_value(conf_data, section, param, res))
        return res;
    return 0;
}
