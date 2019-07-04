//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <fstream>

#include "libjson.h"
#include "stl_json.h"

using namespace jbc;
using namespace json;

/**
 * @brief The FakeItemBuilder struct does nothing, but thus can be used
 * to only parse the json document without doing anything
 */
struct FakeItemBuilder {
    // ARRAY
    static bool begin_array_handler() { return true; }
    static bool begin_array_handler_array() { return true; }
    static bool begin_array_handler_object_value() { return true; }

    static bool end_array_handler() { return true; }

    // OBJECT
    static bool begin_object_handler() { return true; }
    static bool begin_object_handler_array() { return true; }
    static bool begin_object_handler_object_value() { return true; }

    static bool end_object_handler() { return true; }

    // BOOLEAN
    static bool boolean_handler(bool /*value*/) { return true; }

    // DOUBLE
    static bool double_handler(double /*value*/) { return true; }

    static bool integer_handler(int64_t /*value*/) { return true; }

    static bool null_handler() { return true; }

    // STRING
    static bool begin_string_handler() { return true;}
    static bool string_content_handler(std::string_view /*value*/) { return true; }
    static bool end_string_handler() { return true; }

    // KEY
    static bool begin_key_handler() { return true; }
    static bool key_content_handler(std::string_view /*value*/) { return true; }
    static bool end_key_handler() { return true; }
};
//	template<typename T> using stdvector=std::vector<T>;

void lint(std::istream& i, std::string const& name)
{
    bool good = true;
    char buf[32768];
    int currentLine = 0;
    int currentChar = 0;
    parser_bits<stdvector,FakeItemBuilder,std::vector<char>, char> parser;
    while(i.good() && good)
    {
        i.read(buf, sizeof(buf));
        int count = i.gcount();
        for(int i = 0; i < count && good; ++i)
        {
            good = parser.consume_(buf + i);
            if(buf[i] == '\n')
            {
                currentLine += 1;
                currentChar = 0;
            }
            else
                currentChar += 1;
        }
        parser.consume_(nullptr);
    }
    if(i.eof())
        good = good && parser.end();
    if(good)
        std::cout << name << " parsed successful";
    else
    {
        std::cout << name << " has error ";
        if(parser.error_message())
            std::cout << parser.error_message() << " at line " << currentLine << ", position " << currentChar;
    }
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    static_assert(sizeof(std::string) == 32,"Size shall be ok");

    static_assert(sizeof(stl_item) <= 40,"Size shall be ok");
    if(argc != 2)
    {
        std::cerr << "usage : " << argv[0] << " <file>" << std::endl;
        return -1;
    }
    std::string s{argv[1]};
    std::ifstream f{s};
    if(!f)
    {
        std::cerr << "Cannot read " << argv[1] << std::endl;
        return -1;
    }
    lint(f,s);
    return 0;
}
