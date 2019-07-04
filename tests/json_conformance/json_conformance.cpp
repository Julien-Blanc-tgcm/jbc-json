//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <stl_json.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE json_conformance
#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_CASE(pass0, *utf::description("Simple document"))
{
    std::string str = "[{\"toto\":\"tutu\"},{\"toto\":\"tutu\"}]";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass1, *utf::description("Sample json document"))
{
    std::string str = R"json([
                      "JSON Test Pattern pass1",
                      {"object with 1 member":["array with 1 element"]},
                      {},
                      [],
                      -42,
                      true,
                      false,
                      null,
                      {
                          "integer": 1234567890,
                          "real": -9876.543210,
                          "e": 0.123456789e-12,
                          "E": 1.234567890E+34,
                          "":  23456789012E66,
                          "zero": 0,
                          "one": 1,
                          "space": " ",
                          "quote": "\"",
                          "backslash": "\\",
                          "controls": "\b\f\n\r\t",
                          "slash": "/ & \/",
                          "alpha": "abcdefghijklmnopqrstuvwyz",
                          "ALPHA": "ABCDEFGHIJKLMNOPQRSTUVWYZ",
                          "digit": "0123456789",
                          "0123456789": "digit",
                          "special": "`1~!@#$%^&*()_+-={':[,]}|;.</>?",
                          "hex": "\u0123\u4567\u89AB\uCDEF\uabcd\uef4A",
                          "true": true,
                          "false": false,
                          "null": null,
                          "array":[  ],
                          "object":{  },
                          "address": "50 St. James Street",
                          "url": "http://www.JSON.org/",
                          "comment": "// /* <!-- --",
                          "# -- --> */": " ",
                          " s p a c e d " :[1,2 , 3

                  ,

                  4 , 5        ,          6           ,7        ],"compact":[1,2,3,4,5,6,7],
                          "jsontext": "{\"object with 1 member\":[\"array with 1 element\"]}",
                          "quotes": "&#34; \u0022 %22 0x22 034 &#x22;",
                          "\/\\\"\uCAFE\uBABE\uAB98\uFCDE\ubcda\uef4A\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?"
                  : "A key can be any string"
                      },
                      0.5 ,98.6
                  ,
                  99.44
                  ,

                  1066,
                  1e1,
                  0.1e1,
                  1e-1,
                  1e00,2e+00,2e-00
                  ,"rosebud"])json";
    jbc::json::stl_item i;
    std::istringstream s(str);
    bool res = parse_from_stream(s, i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass2, *utf::description("Nesting not an issue"))
{
    std::string str = R"([[[[[[[[[[[[[[[[[[["Not too deep"]]]]]]]]]]]]]]]]]]])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass3, *utf::description("Object outermost value is valid"))
{
    std::string str = R"json({
"JSON Test Pattern pass3": {
    "The outermost value": "can be an object.",
   "In this test": "It is an object."
    }
}
)json";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass4, *utf::description("Single string shall be valid"))
{
    std::string str = R"("A single JSON string is a valid document")";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass5, *utf::description("Array outermost value is valid"))
{
    std::string str = R"json([
"JSON Test Pattern pass5",
"The outermost value",
"can be an array.",
"In this test",
"It is an array."
]
)json";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(pass6, *utf::description("Extra spaces at end is valid"))
{
    std::string str = "{\"obj\":\"value\"}\n\t   \t  \n\r  ";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(fail1, *utf::description("empty document not valid"))
{
    std::string str;
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail2, *utf::description("Unclosed array"))
{
    std::string str=R"(["Unclosed array")";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail3, *utf::description("Unquoted key"))
{
    std::string str = R"({unquoted_key: "keys must be quoted"})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail4, *utf::description("extra comma"))
{
    std::string str = R"(["extra comma",])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}


BOOST_AUTO_TEST_CASE(fail5, *utf::description("double extra comma"))
{
    std::string str = R"(["double extra comma",,])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail6, *utf::description("missing value in array"))
{
    std::string str =R"([   , "<-- missing value"])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail7, *utf::description("comma after close"))
{
    std::string str = R"(["Comma after the close"],)";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail8, *utf::description("extra close"))
{
    std::string str = R"(["Extra close"]])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail9, *utf::description("extra comma"))
{
    std::string str = R"({"Extra comma": true,})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail10, *utf::description("extra value after close"))
{
    std::string str = R"({"Extra value after close": true} "misplaced quoted value")";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail11, *utf::description("Illegal expression"))
{
    std::string str = R"({"Illegal expression": 1 + 2})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail12, *utf::description("Illegal invocation"))
{
    std::string str = R"({"Illegal invocation": alert()})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail13, *utf::description("Invalid leading 0"))
{
    std::string str = R"({"Numbers cannot have leading zeroes": 013})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail14, *utf::description("Hex number invalid"))
{
    std::string str = R"({"Numbers cannot be hex": 0x14})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail15, *utf::description("Illegal backslash escape"))
{
    std::string str = R"(["Illegal backslash escape: \x15"])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail16, *utf::description("Invalid escape"))
{
    std::string str = R"([\naked])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail17, *utf::description("Illegal backslash escape \\017"))
{
    std::string str = R"(["Illegal backslash escape: \017"])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(notfail18, *utf::description("Depth has no limit"))
{
    std::string str = R"([[[[[[[[[[[[[[[[[[[["Too deep"]]]]]]]]]]]]]]]]]]]])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(res);
}

BOOST_AUTO_TEST_CASE(fail19, *utf::description("Missing colon"))
{
    std::string str = R"({"Missing colon" null})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail20, *utf::description("Double colon"))
{
    std::string str = R"({"Double colon":: null})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail21, *utf::description("Comma instead of colon"))
{
    std::string str = R"({"Comma instead of colon", null})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail22, *utf::description("Colon instead of comma"))
{
    std::string str = R"(["Colon instead of comma": false])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail23, *utf::description("Bad value"))
{
    std::string str = R"(["Bad value", truth])";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail24, *utf::description("single quote"))
{
    std::string str = "['single quote']";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail25, *utf::description("tab in string"))
{
    std::string str = "[\"\ttab\tcharacter\tin\tstring\t\"]";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail26, *utf::description("backslash in string"))
{
    std::string str = "[\"backslash\\ character\\\t       in\\  string\\  \"]";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail27, *utf::description("line break in string"))
{
    std::string str = "[\"line\nbreak\"]";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail28, *utf::description("line break in string (2)"))
{
    std::string str = "[\"line\\\nbreak\"]";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail29, *utf::description("Invalid number format"))
{
    std::string str = "[0e]";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail30, *utf::description("Invalid number format (2)"))
{
    std::string str = "[0e+]";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail31, *utf::description("Invalid number format (3)"))
{
    std::string str = "[0e+-1]";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail32, *utf::description("Comma instead of closing brace"))
{
    std::string str = R"("{"Comma instead if closing brace": true,)";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(fail33, *utf::description("mismatch"))
{
    std::string str = R"(["mismatch"})";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s,i);
    BOOST_TEST(!res);
}

BOOST_AUTO_TEST_CASE(largeunicodechar, *utf::description("Parsing a string with a large unicode char shall succeed"))
{
    std::string str = R"json("st\uD83D\uDCABau")json";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s, i);
    BOOST_TEST(res);
    BOOST_TEST(i.string_value() == u8"st💫au");
}

BOOST_AUTO_TEST_CASE(wrongunicodechar, *utf::description("Parsing a string with a large unicode char that is invalid shall fail"))
{
    std::string str = R"json("st\uD83D\uDCu2au")json";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_from_stream(s, i);
    BOOST_TEST(!res);
    bool rightType = i.type() == jbc::json::ItemType::Null;
    BOOST_TEST(rightType);
}

BOOST_AUTO_TEST_CASE(parse_single_document, *utf::description("Parsing a string containing multiple documents shall stop at end of first"))
{
    std::string str = R"json({"first" : [12, 64, true, 34, "test"]}{"second": [13, 64, false, 34, "test"]}")json";
    std::istringstream s(str);
    jbc::json::stl_item i;
    bool res = parse_one_document(s, i);
    BOOST_TEST(res);
    auto prop = i.property("first");
    BOOST_TEST(prop != nullptr);
    BOOST_TEST(prop->child_count() == 5);
    BOOST_TEST(prop->item(2)->bool_value());
    jbc::json::stl_item i2;
    res = parse_one_document(s, i2);
    BOOST_TEST(res);
    prop = i2.property("second");
    BOOST_TEST(prop != nullptr);
    BOOST_TEST(prop->child_count() == 5);
    BOOST_TEST(!prop->item(2)->bool_value());
}
