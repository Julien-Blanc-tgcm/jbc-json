import qbs

Product
{
    type:"dynamiclibrary"
    Depends {name:"cpp"}
    name:"json"
    files: [
        "async_item_printer.h",
        "basic_item.h",
        "customizable_parser_policy.h",
        "helper_functions.h",
        "item_builder.h",
        "item_locator.h",
        "libjson.h",
        "object_iterator.h",
        "parser_bits.h",
        "parser.h",
        "printer.h",
//        "stl_json.cpp",
        "stl_json.h",
        "utf8_printer.h"
    ]
    cpp.cxxLanguageVersion : "c++14"
    cpp.dynamicLibraries : [ ]
    cpp.defines: ["CONTRACT_POLICY=CONTRACTS_USE_EXCEPTION"]
    Group {     // Properties for the produced executable
        fileTagsFilter: product.type
        qbs.install: true
    }
    Export {
        Depends { name:"cpp" }
        cpp.includePaths : [product.sourceDirectory]
    }
}
