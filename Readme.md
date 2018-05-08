# Overview

This is a C++ json parsing / producing library.

# Licence

Released under the Boost Software Licence, version 1.0. See LICENCE_1_0.txt for
details.

# Rationale

JBCJson is a JSON library for general use, designed with performance and 
memory usage in mind.

# Concepts

The library mainly consist of three independant parts, which are combined 
together to produce a fully featured JSON parser / producer :

 * basic_item : this template class represent json data, which, as per the
   specification, can either be null, boolean, double, string, array or
   object. It is a variant and allows visitation of the data, as well as
   accessors for the data in case you already know its underlying type.
 * parser : this template class is in charge of doing the actual parsing of
   the json data. Parsing is done from memory fed to the parser, so that it
   can come from any source. The parser has a sax-like API to allow retrieving
   of the JSON data. 
 * output : this class is in charge of producing json data. It is designed to
   write into fixed size buffers, handling a virtual object offset when data
   does not fit into the buffer, to allow easier use of asynchronous buffers.

There are also some glue parts :

 * item_builder makes the link between the parser and basic_item : it allows
   the creation of json basic items from the parser data
 * output_visitor makes the link between basic_item and output : it allows
   to output json items.
 * there is also, in program json_print, a sample item_builder_printer class
   which makes a direct link between parser and output.



