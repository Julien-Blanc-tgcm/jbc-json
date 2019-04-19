# Overview

This is a C++ json parsing / producing library.

# Licence

Released under the Boost Software Licence, version 1.0. See LICENCE_1_0.txt for
details.

# Rationale

JBCJson is a JSON library for general use, designed with performance and 
memory usage in mind.

# Concepts

## Parser / item / producer separation

The library mainly consist of three independant parts, which are combined 
together to produce a fully featured JSON parser / producer :

 * parser : this template class is in charge of doing the actual parsing of
   the json data. Parsing is done from memory fed to the parser, so that it
   can come from any source. The parser has a sax-like API to allow retrieving
   of the JSON data. 
 * output : this class is in charge of producing json data. It is designed to
   write into fixed size buffers, handling a virtual object offset when data
   does not fit into the buffer, to allow easier use of asynchronous buffers.
 * basic_item : this template class represent json data, which, as per the
   specification, can either be null, boolean, double, string, array or
   object. It is a variant and allows visitation of the data, as well as
   accessors for the data in case you already know its underlying type.

There are also some glue parts :

 * item_builder makes the link between the parser and basic_item : it allows
   the creation of json basic items from the parser data
 * output_visitor makes the link between basic_item and output : it allows
   to output json items.
 * there is also, in program json_print, a sample item_builder_printer class
   which makes a direct link between parser and output.

## String and container agnosticity

The library tries not to enforce the use of a particular container or string
type. Such types are part of a traits class. The file stl_json.h serves as
both a standard implementation which should be sufficient for most people
and an example on how to implement its own traits to integrate the parser
with custom string or container types.

Another sample is the zero-allocating traits, which do not do a single
dynamic allocation. Note that this is incompatible with the use of basic_item,
which requires dynamic allocation (only parsing / outputting is supported with
these traits). These may be ideal in an embedded context, where json would be
used as an input / output format but not for internal data representation.

If needed, helper_functions can be customized. While they were designed to
work with most string and character types, there can be some corner cases
where customization is needed. The qt_json sample shows how to adapt the
library to work with different containers, which use multibyte characters
and a non utf-8 internal encoding.
