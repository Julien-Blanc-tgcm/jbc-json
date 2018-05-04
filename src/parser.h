#ifndef JBC_JSON_PARSER_H
#define JBC_JSON_PARSER_H


#include "helper_functions.h"
#include "parser_bits.h"
#include "customizable_parser_policy.h"

namespace jbc
{
namespace json
{

template<template<class> class container, typename string_type, typename char_type= typename string_type::value_type>
class parser :
        public parser_bits<container,customizable_parser_policy<string_type>,
            string_type, char_type>
{

};

}
}

#endif // JBC_JSON_PARSER_H
