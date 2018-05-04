#include "stl_json.h"
//#include <fstream>
#include <cstdlib>

namespace jbc
{
namespace json
{
namespace {
//const int buffersize = 131072;
const int buffersize = 65536;
//const int buffersize = 1048576;
//constexpr const int buffersize = 16 * 1024 * 1024;
}
void parse_from_file(const std::string& file, stl_item& destination)
{
    stl_parser parser;
    char buf[buffersize];
    bool good = true;
    FILE *f;
//    std::fstream f;
//    f.rdbuf()->pubsetbuf(0,0);
    f = fopen(file.c_str(), "rb");
    if(!f)
        return;
    while(good && !feof(f))
    {
        size_t ret = fread(buf, sizeof(char), buffersize, f);
        good = parser.consume(buf, buf + ret);
    }
    if(feof(f) && good)
    {
        if(parser.end())
            parser.moveTo(destination);
    }
    return;
}

}
}
