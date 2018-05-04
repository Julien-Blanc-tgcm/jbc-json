#include <iostream>
#include <libjson.h>
#include <stl_json.h>
#include <fstream>

using namespace jbc;
using namespace json;

struct ItemBuilderPrinter {
    static std::array<char, 65536> buf;
    using o=output<char>;
    static int offset;
    static locator l;

    bool array_start = false;

    static void flushbuffer()
    {
        std::cout.write(buf.data(), offset);
        offset = 0;
    }

    // ARRAY
    static bool begin_array_handler_begin() {
        if(o::array_start(buf, offset))
            return true;
        else
        {
            flushbuffer();
            return begin_array_handler_begin();
        }
    }
    static bool begin_array_handler_array() { return begin_array_handler_begin(); }
    static bool begin_array_handler_object_value() { return begin_array_handler_begin(); }

    static bool end_array_handler() {
        if(o::array_end(buf, offset))
            return true;
        else
        {
            flushbuffer();
            return begin_array_handler_begin();
        }
    }

    // OBJECT
    static bool begin_object_handler_begin() {
        if(o::object_start(buf, offset))
            return true;
        else
        {
            flushbuffer();
            return begin_object_handler_begin();
        }
    }

    static bool begin_object_handler_array() { return begin_object_handler_begin(); }
    static bool begin_object_handler_object_value() { return begin_object_handler_begin(); }

    static bool end_object_handler() {
        if(o::object_end(buf, offset))
            return true;
        else
        {
            flushbuffer();
            return begin_object_handler_begin();
        }
    }

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
    static bool string_handler_initial(std::vector<char> const& value) {
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
        // todo : implement
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
        output_json<char>(std::cout, i);
        return 0;
    }
    else
    {
        std::cerr << "Invalid json\n";
        return -2;
    }
}
