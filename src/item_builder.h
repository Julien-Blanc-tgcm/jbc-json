//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef JBC_JSON_ITEMBUILDER_H
#define JBC_JSON_ITEMBUILDER_H

#include "parser.h"
#include "basic_item.h"

namespace jbc
{
namespace json
{

template<template<class> class container, typename Item_>
class item_builder
{
protected:
    Item_ item_;
    container<Item_*> current_item_;
    enum class State : std::uint8_t {
        Initial,
        InArray,
        InObject
    };

    State state_ = State::Initial;

    // ARRAY
    bool begin_array_handler();

    bool end_array_handler();

    // OBJECT
    bool begin_object_handler();

    bool end_object_handler();

    // BOOLEAN
    bool boolean_handler(bool value);

    // DOUBLE
    bool double_handler(double value);

#ifdef JSON_USE_LONG_INTEGERS
    // INTEGER
    bool integer_handler(int64_t value);
#endif
    // NULL
    bool null_handler();

    // STRING
    bool begin_string_handler();
    bool string_content_handler(typename Item_::traits::string_view value);
    bool end_string_handler();

    // KEY (same as string)
    bool begin_key_handler();
    bool key_content_handler(typename Item_::traits::string_view value);
    bool end_key_handler();

    // there is currently no way to rename an object key. So we need to store the whole key name
    // before creating the object, unlike for string values. This should not be an issue in real
    // life since key names should be short, but it still incurs a penalty...
    typename Item_::traits::string_type lastString_;

public:
    item_builder();
    item_builder(item_builder const&)=delete;
    item_builder& operator=(item_builder const&)=delete;
    item_builder(item_builder&&) = delete;
    item_builder& operator=(item_builder&&) = delete;
    ~item_builder() noexcept = default;

    /**
     * @brief item returns the item constructed. May be complete, or not, depending on the parsing
     * @return reference to the item. A default constructed (null) Item will be returned if parsing was not done at all,
     * but failure of parsing may lead to the return of a partially constructed object
     */
    Item_ const& item() const;

    /**
     * @brief moveTo moves the json item to the new item. The internal item is discarded.
     * @param dest the destination to move into.
     */
    void moveTo(Item_ & dest);
};

template<template<class> class container,typename Item_>
item_builder<container, Item_>::item_builder()
{
    current_item_.push_back(&item_);
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::begin_array_handler()
{
    if(item_.type() == ItemType::Null)
    {
        item_.morph_to(ItemType::Array);
        current_item_.push_back(&item_);
        return true;
    }
    if(current_item_.back()->type() == ItemType::Array)
    {
        current_item_.push_back(current_item_.back()->create_item(ItemType::Array));
        return true;
    }
    // ItemType::Object: // object value
    current_item_.back()->morph_to(ItemType::Array);
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::end_array_handler()
{
    current_item_.back()->end_current_object();
    current_item_.pop_back();
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::begin_object_handler()
{
    if(item_.type() == ItemType::Null)
    {
        item_.morph_to(ItemType::Object);
        current_item_.push_back(&item_);
        return true;
    }
    if(current_item_.back()->type() == ItemType::Array)
    {
        current_item_.push_back(current_item_.back()->add_item(Item_(ItemType::Object)));
        return true;
    }
    // else ItemType::Object: // object value
    current_item_.back()->morph_to(ItemType::Object);
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::end_object_handler()
{
    current_item_.back()->end_current_object();
    current_item_.pop_back();
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::boolean_handler(bool value)
{
    if(current_item_.back()->type() == ItemType::Array)
    {
        Item_* item = current_item_.back()->create_item(ItemType::Boolean);
        item->set_bool_value(value);
        return true;
    }
    // else ItemType::Object:
    current_item_.back()->morph_to(ItemType::Boolean);
    current_item_.back()->set_bool_value(value);
    current_item_.pop_back();
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::double_handler(double value)
{
    if(current_item_.back()->type() == ItemType::Array)
    {
        Item_* item = current_item_.back()->create_item(ItemType::Double);
        item->set_double_value(value);
        return true;
    }
    // else object
    current_item_.back()->morph_to(ItemType::Double);
    current_item_.back()->set_double_value(value);
    current_item_.pop_back();
    return true;
}

#ifdef JSON_USE_LONG_INTEGERS
template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::integer_handler(int64_t value)
{
    if(current_item_.back()->type() == ItemType::Array)
    {
        Item_* item = current_item_.back()->create_item(ItemType::Integer);
        item->setIntegerValue(value);
        return true;
    }
    // else ItemType::Object
    current_item_.back()->morph_to(ItemType::Integer);
    current_item_.back()->setIntegerValue(value);
    current_item_.pop_back();
    return true;
}

#endif

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::null_handler()
{
    if(current_item_.back()->type() == ItemType::Array)
    {
        current_item_.back()->create_item(ItemType::Null);
        return true;
    }
    // else ItemType::Object:
    current_item_.pop_back();
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::begin_string_handler()
{
    if(item_.type() == ItemType::Null)
    {
        using string = typename Item_::traits::string_type;
        string s;
        item_.morph_to_string(std::move(s));
        current_item_.push_back(&item_);
        return true;
    }
    if(current_item_.back()->type() == ItemType::Array)
    {
        Item_* item = current_item_.back()->create_item(ItemType::String);
        using string = typename Item_::traits::string_type;
        string s;
        item->set_string_value(std::move(s));
        current_item_.push_back(item);
        return true;
    }
    // else ItemType::Object:
    using string = typename Item_::traits::string_type;
    string s;
    current_item_.back()->morph_to_string(std::move(s));
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::end_string_handler()
{
    current_item_.pop_back();
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::string_content_handler(typename Item_::traits::string_view value)
{
    using namespace std;
    typename Item_::traits::string_type & s = current_item_.back()->string_value();
    helper_functions<typename Item_::traits::buffer_type, typename Item_::traits::char_type>::
            append(s, begin(value), end(value));
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::begin_key_handler()
{
    lastString_.clear();
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::key_content_handler(typename Item_::traits::string_view value)
{
    using namespace std;
    helper_functions<typename Item_::traits::buffer_type, typename Item_::traits::char_type>::
            append(lastString_, begin(value), end(value));
    return true;
}

template<template<class> class container,typename Item_>
bool item_builder<container, Item_>::end_key_handler()
{
    Item_* item = current_item_.back()->create_property(std::move(lastString_));
    current_item_.push_back(item);
    lastString_.clear();
    return true;
}

template<template<class> class container,typename Item_>
Item_ const& item_builder<container, Item_>::item() const
{
    return item_;
}

template<template<class> class container,typename Item_>
void item_builder<container, Item_>::moveTo(Item_ & destination)
{
    destination = std::move(item_);
}

}
}

#endif // JBC_JSON_ITEMBUILDER_H

