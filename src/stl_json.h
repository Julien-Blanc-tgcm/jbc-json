//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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
    using string_type = std::string;
    using string_view = std::string_view;
    using char_type = std::string::value_type;
    using buffer_type = std::vector<char>;
    using array_type = std::vector<basic_item<stl_types> >;
    using object_type = std::vector<std::pair<std::string, basic_item<stl_types> > >;
    using array_iterator = array_type::iterator;
    using object_iterator = object_type::iterator;
    using array_const_iterator = array_type::const_iterator;
    using object_const_iterator = object_type::const_iterator;
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
    static std::string make_string(buffer_type::const_iterator begin, buffer_type::const_iterator end)
    {
        return std::string(begin, end);
    }
    static int char_value(char c)
    {
        return static_cast<int>(static_cast<unsigned char>(c));
    }
    static constexpr const bool is_utf8 = true;
    static void copy_basic_data(char const* first, char const* last, char * dest);
};

using stl_item=basic_item<stl_types>;
using stl_item_builder = item_builder<stdvector, stl_item>;
using stl_parser = parser_bits<stdvector,stl_item_builder, std::vector<char>,char>;
//using stl_printer = printer<stl_item>;

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

void stl_types::copy_basic_data(const char *first, const char *last, char *dest)
{
    std::copy(first, last, dest);
}

}
}

#endif // JBC_JSON_STL_JSON_H
