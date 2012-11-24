#include "configparser.hpp"

void add_section::operator()(char const* p, char const* q) const
{
    string s(p,q);
    boost::algorithm::trim(s);
    data_.push_back(Section(s, Entries()));
}

void add_key::operator()(char const* p, char const* q) const
{
    string s(p,q);
    boost::algorithm::trim(s);
    data_.back().second.push_back(Entry(s, string()));
}

void add_value::operator()(char const* p, char const* q) const
{
    data_.back().second.back().second.assign(p,q);
}

bool is_comment::operator()(string const& s) const
{
    return s[0] == '\n' || s[0] == ';';
}

bool find_value(IniData const& ini, string const& s, string const& p, string & res)
{
    IniData::const_iterator sit = find_if(ini.begin(), ini.end(), first_is(s));
    if (sit == ini.end())
        return false;

    Entries::const_iterator it = find_if(sit->second.begin(), sit->second.end(), first_is(p));
    if (it == sit->second.end())
        return false;

    res = it->second;
    return true;
}

