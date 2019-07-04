//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <libjson.h>
#include <stl_json.h>
#include <fstream>
#include <static_json.h>


using namespace jbc;
using namespace json;

struct ItemBuilderPrinter {
    std::array<char, 65536> buf;
    using o=output<char, basic_locator>;
    int offset = 0;
    bool previous_is_key = true;

    void flushbuffer()
    {
        std::cout.write(buf.data(), offset);
        offset = 0;
    }

    // ARRAY
    bool begin_array_handler() {
        if(!previous_is_key)
        {
            while(!o::array_separator(buf, offset))
                flushbuffer();
        }
        previous_is_key = true;
        while(!o::array_start(buf, offset))
        {
            flushbuffer();
        }
        return true;
    }

    bool end_array_handler() {
        previous_is_key = false;
        while(!o::array_end(buf, offset))
        {
            flushbuffer();
        }
        return true;
    }

    // OBJECT
    bool begin_object_handler() {
        if(!previous_is_key)
        {
            while(!o::array_separator(buf, offset))
                flushbuffer();
        }
        previous_is_key = true;
        while(!o::object_start(buf, offset))
            flushbuffer();
        return true;
    }

    bool end_object_handler() {
        previous_is_key = false;
        while(!o::object_end(buf, offset))
        {
            flushbuffer();
        }
        return true;
    }

    // BOOLEAN
    bool boolean_handler(bool value)
    {
        if(!previous_is_key)
        {
            while(!o::array_separator(buf, offset))
                flushbuffer();
        }
        basic_locator loc;
        while(!o::boolean(value, loc, buf, offset))
        {
            flushbuffer();
        }
        previous_is_key = false;
        return true;
    }

    // DOUBLE
    bool double_handler(double value)
    {
        if(!previous_is_key)
        {
            while(!o::array_separator(buf, offset))
                flushbuffer();
        }
        basic_locator loc;
        while(!o::number(value, 10, loc, buf, offset))
        {
            flushbuffer();
        }
        previous_is_key = false;
        return true;
    }

    bool integer_handler(int64_t /*value*/) { return true; }

    bool null_handler()
    {
        if(!previous_is_key)
        {
            while(!o::array_separator(buf, offset))
                flushbuffer();
        }
        basic_locator loc;
        while(!o::null(loc, buf, offset))
        {
            flushbuffer();
        }
        previous_is_key = false;
        return true;
    }

    bool begin_string_handler()
    {
        if(!previous_is_key)
        {
            while(!o::array_separator(buf, offset))
                flushbuffer();
        }
        while(!o::string_delimiter(buf, offset))
            flushbuffer();
        previous_is_key = false;
        return true;
    }

    // STRING
    bool string_content_handler(std::string_view value)
    {
        basic_locator loc;
        loc.position = 1;
        while(!o::string_content<stl_types, decltype(value)>(value, loc, buf, offset))
        {
            flushbuffer();
        }
        return true;
    }

    bool end_string_handler()
    {
        while(!o::string_delimiter(buf, offset))
            flushbuffer();
        return true;
    }

    bool begin_key_handler()
    {
        if(!previous_is_key)
        {
            while(!o::array_separator(buf, offset))
                flushbuffer();
        }
        while(!o::string_delimiter(buf, offset))
            flushbuffer();
        return true;
    }

    bool key_content_handler(std::string_view value)
    {
        basic_locator loc;
        loc.position = 1;
        while(!o::string_content<stl_types, decltype (value)>(value, loc, buf, offset))
        {
            flushbuffer();
        }
        return true;
    }
    bool end_key_handler()
    {
        while(!o::string_delimiter(buf, offset))
        {
            flushbuffer();
        }
        while(!o::object_keyvalueseparator(buf, offset))
        {
            flushbuffer();
        }
        previous_is_key = true;
        return true;
    }
};

int main(int argc, char** argv)
{
    std::string f;
    bool use_stream = false;
    bool use_doc = false;
    bool use_traversal = false;
    if(argc == 3)
    {
        use_stream = std::string{"stream"} == argv[2];
        use_doc = std::string{"doc"} == argv[2];
        use_traversal = std::string{"traversal"} == argv[2];
    }
    if(argc < 2)
    {
        std::cerr << "Usage : " << argv[0] << " <file>" << std::endl;
        return -1;
    }
    else
        f  = argv[1];
    //std::cerr << "Item size is " << sizeof(stl_item) << std::endl;
    stl_item i;
    bool res;
    if(use_stream)
    {
        std::cerr << "Parsing using stream\n";
        std::ifstream file(f, std::ios::binary | std::ios::in);
        res = parse_from_stream(file, i);
    }
    else if(use_doc)
    {
        std::cerr << "Parsing using doc\n";
        std::ifstream file(f, std::ios::binary | std::ios::in);
        res = parse_one_document(file, i);
    }
    else if(use_traversal)
    {
        std::cerr << "Parsing using traversal printing\n";
        parser_bits<static_vector, ItemBuilderPrinter, std::vector<char>, char> parser;
        constexpr const int buffersize = 65536;
        char buf[buffersize];
        bool good = true;
        FILE *file;
        file = fopen(f.c_str(), "rb");
        if(!file)
            return -1;
        while(good && !feof(file))
        {
            size_t ret = fread(buf, sizeof(char), buffersize, file);
            good = parser.consume(buf, buf + ret);
        }
        if(feof(file) && good)
        {
            if(parser.end())
            {
                parser.flushbuffer();
                return 0;
            }
            else
                res = false;
        }
        else
            res = false;
    }
    else
    {
        std::cerr << "Parsing using file\n";
        res = parse_from_file(f, i);
    }
    if(res)
    {
        //stl_printer::print(i, false, std::cout);
        output_json<char, stl_item, basic_locator>(std::cout, i);
        return 0;
    }
    else
    {
        std::cerr << "Invalid json\n";
        return -2;
    }
}
