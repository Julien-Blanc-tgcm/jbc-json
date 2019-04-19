//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef JBC_JSON_STATIC_JSON_H
#define JBC_JSON_STATIC_JSON_H

#include "libjson.h"
#include <boost/container/static_vector.hpp>

#ifndef JSON_MAX_DEPTH
#define JSON_MAX_DEPTH 30
#endif

#ifndef JSON_STRING_LENGTH
#define JSON_STRING_LENGTH 255
#endif

namespace jbc
{
namespace json
{

template<typename T>
using static_vector=boost::container::static_vector<T, JSON_MAX_DEPTH>;

struct static_types
{
//    typedef std::string string_type;
    using char_type = char;
    using buffer_type = boost::container::static_vector<char, JSON_STRING_LENGTH>;
//    typedef std::vector<basic_item<stl_types> > array_type;
//    typedef std::vector<std::pair<std::string, basic_item<stl_types> > > object_type;
//    typedef array_type::iterator array_iterator;
//    typedef object_type::iterator object_iterator;
//    typedef array_type::const_iterator array_const_iterator;
//    typedef object_type::const_iterator object_const_iterator;
/*    template<typename... args>
    static void array_emplace_back(array_type& container, args... arg)
    {
        container.emplace_back(std::forward<args>(arg)...);
    }
    template<typename... args>
    static void object_emplace_back(object_type& container, args&&... arg)
    {
        container.emplace_back(std::forward<args>(arg)...);
    }*/
/*    static std::string make_string(char const* str)
    {
        return std::string(str);
    }*/
    static int char_value(char_type c)
    {
        return static_cast<int>(static_cast<unsigned char>(c));
    }
};

//using stl_item=basic_item<stl_types>;
//using stl_item_builder = item_builder<stdvector, stl_item>;
//using stl_parser = parser_bits<stdvector,stl_item_builder, std::vector<char>,char>;
//using stl_printer = printer<stl_item>;

}
}

#endif // JBC_JSON_STL_JSON_H
