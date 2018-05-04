#ifndef JBC_JSON_STL_JSON_H
#define JBC_JSON_STL_JSON_H

#include "libjson.h"
#include <vector>

namespace jbc
{
namespace json
{
template<typename T> using stdvector=std::vector<T>;


struct stl_types
{
    typedef std::string string_type;
    typedef std::string::value_type char_type;
    typedef std::vector<char> buffer_type;
    typedef std::vector<basic_item<stl_types> > array_type;
    typedef std::vector<std::pair<std::string, basic_item<stl_types> > > object_type;
    typedef array_type::iterator array_iterator;
    typedef object_type::iterator object_iterator;
    typedef array_type::const_iterator array_const_iterator;
    typedef object_type::const_iterator object_const_iterator;
    template<typename... args>
    static void array_emplace_back(array_type& container, args... arg)
    {
        container.emplace_back(std::forward<args>(arg)...);
    }
    template<typename... args>
    static void object_emplace_back(object_type& container, args&&... arg)
    {
        container.emplace_back(std::forward<args>(arg)...);
    }
    static std::string make_string(char const* str)
    {
        return std::string(str);
    }
    static int char_value(char c)
    {
        return static_cast<int>(static_cast<unsigned char>(c));
    }
};

using stl_item=basic_item<stl_types>;
using stl_item_builder = item_builder<stdvector, stl_item>;
using stl_parser = parser_bits<stdvector,stl_item_builder, std::vector<char>,char>;
using stl_printer = printer<stl_item>;

inline bool parse_from_file(std::string const& file, stl_item& destination)
{
    stl_parser parser;
    constexpr const int buffersize = 65536;
    char buf[buffersize];
    bool good = true;
    FILE *f;
//    std::fstream f;
//    f.rdbuf()->pubsetbuf(0,0);
    f = fopen(file.c_str(), "rb");
    if(!f)
        return false;
    while(good && !feof(f))
    {
        size_t ret = fread(buf, sizeof(char), buffersize, f);
        good = parser.consume(buf, buf + ret);
    }
    if(feof(f) && good)
    {
        if(parser.end())
        {
            parser.moveTo(destination);
            return true;
        }
    }
    return false;
}
template<typename stream>
inline bool parse_from_stream(stream & f, stl_item& destination)
{
    stl_parser parser;
    constexpr const int buffersize = 65536;
    char buf[buffersize];
    bool good = true;
    if(!f.good())
        return false;
    while(good && f.good() && !f.eof())
    {
        size_t ret = f.read(buf, buffersize).gcount();
        good = parser.consume(buf, buf + ret);
    }
    if(f.eof() && good)
    {
        if(parser.end())
        {
            parser.moveTo(destination);
            return true;
        }
    }
    return false;
}

template<typename stream>
inline bool parse_one_document(stream & f, stl_item& destination)
{
    stl_parser parser;
    bool good = true;
    if(!f.good())
        return false;
    while(good && f.good() && !parser.complete_without_error())
    {
        char c;
        f.get(c);
        if(f.good())
            good = parser.consume(c);
    }
    if(good && parser.end())
    {
        parser.moveTo(destination);
        return true;
    }
    return false;
}

}
}

#endif // JBC_JSON_STL_JSON_H
