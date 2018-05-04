#include <iostream>
#include <fstream>

#include "libjson.h"
#include "stl_json.h"

using namespace jbc;
using namespace json;

struct FakeItemBuilder {
    // ARRAY
    static bool begin_array_handler_begin() { return true; }
    static bool begin_array_handler_array() { return true; }
    static bool begin_array_handler_object_value() { return true; }

    static bool end_array_handler() { return true; }

    // OBJECT
    static bool begin_object_handler_begin() { return true; }
    static bool begin_object_handler_array() { return true; }
    static bool begin_object_handler_object_value() { return true; }

    static bool end_object_handler() { return true; }

    // BOOLEAN
    static bool boolean_handler_array(bool /*value*/) { return true; }
    static bool boolean_handler_object_value(bool /*value*/) { return true; }

    // DOUBLE
    static bool double_handler_array(double /*value*/) { return true; }
    static bool double_handler_object_value(double /*value*/) { return true; }

    static bool integer_handler_array(int64_t /*value*/) { return true; }
    static bool integer_handler_object_value(int64_t /*value*/) { return true; }

    static bool null_handler_array() { return true; }
    static bool null_handler_object_value() { return true; }

    // STRING
    static bool string_handler_array(std::vector<char> const& /*value*/) { return true; }
    static bool string_handler_object_value(std::vector<char> const& /*value*/) { return true; }
    static bool string_handler_object_key(std::vector<char> const& /*value*/) { return true; }
    static bool string_handler_initial(std::vector<char> const&) { return true;}

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
            good = parser.consume(buf[i]);
            if(buf[i] == '\n')
            {
                currentLine += 1;
                currentChar = 0;
            }
            else
                currentChar += 1;
        }
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
