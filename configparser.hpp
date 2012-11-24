#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <fstream>
#include <functional>
#include <numeric>
#include <list>
#include <vector>
#include <string>

#include <boost/spirit.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::spirit;

typedef pair<string, string>    Entry;      // САМА ЗАПИСЬ
typedef list<Entry>             Entries;    // Список записей
typedef pair<string, Entries>   Section;    // Секция
typedef list<Section>           IniData;    // Все данные

struct add_section {
        IniData & data_;
        add_section (IniData& data) : data_(data) {}
        void operator()(char const* p, char const* q) const;
};

struct add_key {
        IniData & data_;
        add_key(IniData & data) : data_(data) {}
        void operator()(char const* p, char const* q) const;
};

struct add_value
{
        IniData & data_;
        add_value(IniData & data) : data_(data) {}
        void operator()(char const* p, char const* q) const;
};

struct inidata_parser : public grammar<inidata_parser>
{
        IniData & data_;

        inidata_parser(IniData & data) : data_(data) {}

        template <typename ScannerT>
        struct definition
        {
                rule<ScannerT> inidata, section, entry, ident, value, stringSpaces, spaces;

                rule<ScannerT> const& start() const { return inidata; }

                definition(inidata_parser const& self)
                {
                    inidata = *section;

                    section = ch_p('[')
                            >> ident[add_section(self.data_)]
                            >> ch_p(']')
                            >> stringSpaces
                            >> ch_p('\n')
                            >> spaces
                            >> *(entry);

                    entry = ident[add_key(self.data_)]
                            >> stringSpaces
                            >> ch_p('=')
                            >> stringSpaces
                            >> value[add_value(self.data_)]
                            >> spaces;

                    ident = +(alnum_p | chset<>("-_.,:(){}#@&*|"));

                    value = *(~ch_p('\n'));

                    stringSpaces = *blank_p;

                    spaces = *space_p;

                }
        };
};

struct is_comment
{
        bool operator()(string const& s) const;
};

struct first_is
{
        string const& s_;
        first_is(std::string const& s) : s_(s) {}
        template< class Pair>
        bool operator()(Pair const& p) const { return p.first == s_; }
};

bool find_value(IniData const& ini, string const& s, string const& p, string & res);

#endif // CONFIGPARSER_HPP

