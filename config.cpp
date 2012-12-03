#include "config.hpp"

Config::Config(int argc_p, char* argv_p[]){
    readConfig(argc_p, argv_p);
}

void Config::readConfig(int argc_p, char *argv_p[])
{
    using std::cout;
    using std::ifstream;
    using std::vector;
    using std::string;
    using std::getline;

    const char* pConfLocation=0;

    if ( argc_p == 1 ){
        pConfLocation = "./bh.conf";
    } else if ( argc_p == 2 ){
        pConfLocation = argv_p[1];
    } else {
        throw std::runtime_error("Sick usage. Try: <file.conf>\n");
    }

    ifstream in;
    try {
        /* NOTE:
         * input - reading
         * output - writing
         * ifstream	default mode parameter is "ios::in"
         */
        in.exceptions ( ifstream::failbit | ifstream::badbit );
        in.open(pConfLocation);
    } catch ( ifstream::failure e ) {
        throw std::runtime_error("Can't open configuration file");
    }

    vector<string> lns;
    string s;
    /* NOTE:
     * Конфиг считывается строками до конца, заполняя вектор строк lns
     */
    in.exceptions ( ifstream::failbit | ifstream::badbit | ifstream::eofbit );
    /* NOTE:
     * iostream::eof will only be set after reading the end of the stream. It does not indicate, that the
     * next read will be the end of the stream.
     * http://stackoverflow.com/questions/5605125/why-is-iostreameof-inside-a-loop-condition-considered-wrong
     *
     * eofbit is only set when the last read reached EOF, not if the next read will read only EOF. When the
     * latter happens, failbit is set at the same time
     * http://stackoverflow.com/questions/13651054/why-getline-throws-stdios-basefailure-when-exception-mask-is-not-set-to
     *
     * eof() returns true if the eofbit stream's error flag has been set by a previous i/o operation
     * http://www.cplusplus.com/reference/ios/ios/eof/
     */
    try {
        for (;;) {
            getline( in, s );
            boost::algorithm::trim(s);
            lns.push_back( s+='\n');
        }
    } catch (ifstream::failure)
    {}
    /* NOTE:
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
        throw std::runtime_error("Error has been detected in configuration file. Please double check it.");
    }
}

string Config::findConfigParamValue(string section, string param) const
{
    string res;
    if (find_value(conf_data, section, param, res)) {
        return res;
    } else {
        throw std::runtime_error(string("Unable to find \"") += res += string("\" parameter in configuration file"));
    }
}
