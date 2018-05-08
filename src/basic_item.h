//          Copyright Julien Blanc 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef JBC_JSON_BASICITEM_H
#define JBC_JSON_BASICITEM_H

#include <stdint.h>
#include <DBC/contracts.h>
#include <boost/variant.hpp>

namespace jbc
{
namespace json
{

template<int i1, int i2>
struct maxvalue {
    static constexpr int value = (i1 > i2)?i1:i2;
};

/**
 * @brief The ItemType enum lists the different type that a json::basic_item can be of
 */
enum class ItemType : uint8_t
{
    Null = 0 /**< null json value. Equivalent to null javascript value */,
    Boolean = 1 /**< boolean value, either true of false in javascript */,
#ifdef JSON_USE_LONG_INTEGERS
    Integer = 2 /**< integer value. 64bit signed integer */,
#endif
    Double = 3 /**< double value. 64bit double, unless double default to something else on architecture */,
    String = 4 /**< string value. equivalent to javascript "this is a string" */,
    Object = 5 /**< object value, as in javascript object */,
    Array = 6 /**< array value, as in javascript array */
};

/**
 * The basic_item class is the class that represents any json item. A json document is just another type
 * of basic_item (any json object or array is a document itself).
 */
template<typename traits_>
class basic_item
{
    template<typename Item, typename stream>
    friend class printer;
    template<typename Item, typename stream> friend class item_print_visitor;

    typedef boost::variant<
        boost::blank,
        bool,
        double,
        typename traits_::array_type,
        typename traits_::object_type,
        typename traits_::string_type
    > data_type;

    data_type data_;
//    ItemType itemType_ = ItemType::Null;
    bool complete_ = true;

public:

    typedef traits_ traits;
    /**
     * @brief basic_item default constructor. Constructs a new null basic_item, which can then be morphed into a new one
     */
    basic_item(bool async = false) noexcept;
    /**
     * @brief basic_item constructs a new basic_item of given type. It will not be initialized.
     * @param type
     */
    explicit basic_item(ItemType type, bool async = false) noexcept;
#ifdef COPYABLEITEM
    /**
     * @brief basic_item copy constructor.
     * @param other basic_item to copy
     */
    basic_item(basic_item const& other) noexcept;
    /**
     * @brief operator = copy operator.
     * @param other basic_item to copy
     * @return current object
     */
    basic_item& operator=(basic_item const& other) noexcept;
#endif
    static basic_item clone(basic_item const& other) noexcept;
public:
    /**
     * @brief basic_item move constructor
     * @param other object to move
     */
    basic_item(basic_item &&other) noexcept;
    /**
     * @brief operator = move operator= is deleted. Should not be needed.
     * @param other
     * @return current object
     */
    basic_item& operator=(basic_item&& other) noexcept;

    /**
     * @brief basic_item creates a new string item with the given string value
     * @param stringValue the value of the string
     */
    explicit basic_item(typename traits::string_type && stringValue) noexcept;

    /**
     * @brief basic_item creates a new string item with the given string value
     * @param stringValue the value of the string
     */
    explicit basic_item(typename traits::string_type const& stringValue);

    /**
     * @brief basic_item creates a new string item with the given string value
     * @param stringValue value of the string
     */
    explicit basic_item(char const* stringValue);

    /**
     * Frees up resources held by this object, including all children
     */
    ~basic_item() noexcept;

    /**
     * @brief type returns the type of the item
     * @return
     */
    ItemType type() const;
    /**
     * @brief complete returns true if the object is complete, ie : all of its children are available
     * @return true if complete, false otherwise
     */
    bool complete() const;
    /**
     * @brief Ends the creation of a complex json object, for example an array. It marks the object as complete
     */
    void end_current_object();

    /**
     * @brief Adds a property to an object
     * @param name name of the property
     * @param value value of the property
     * @return a pointer to the property added, which allows subsequent modifications of the property.
     * note that this pointer is invalidated if another property is added
     */
    basic_item<traits>* add_property(typename traits::string_type const& name, basic_item<traits> && value);

    /**
     * @copydoc addProperty
     */
    basic_item<traits>* add_property(char const* name, basic_item<traits> && value) {
        return add_property(traits::make_string(name), std::move(value));
    }

    /**
     * @brief Adds a property to an object, using move semantics for the key for efficiency
     * @param name name of the property
     * @param value value of the property
     * @return a pointer to the property added, which allows subsequent modifications of the property.
     * note that this pointer is invalidated if another property is added
     */
    basic_item<traits>* add_property(typename traits::string_type && name, basic_item<traits> && value);
    /**
     * @brief Creates a property with given name.
     * @param name name of the property
     * @param type type of the property
     * @return a pointer to the property added, which allows subsequent modifications of the property.
     * note that this pointer is invalidated if another property is added
     */
    basic_item<traits>* create_property(typename traits::string_type const& name, ItemType type = ItemType::Null);
    /**
     * @brief Creates a property with given name, using move semantics for the key for efficiency
     * @param name name of the property
     * @param type type of the property
     * @return a pointer to the property added, which allows subsequent modifications of the property.
     * note that this pointer is invalidated if another property is added
     */
    basic_item<traits>* create_property(typename traits::string_type && name, ItemType type = ItemType::Null);

    /**
     * @copydoc createProperty
     */
    basic_item<traits>* create_property(char const* name, ItemType type = ItemType::Null) {
        return create_property(traits::make_string(name), type);
    }

    /**
     * @brief Adds an item to an array item
     * @param value item to add
     * @return a pointer to the item added, which allows subsequent modifications of the item.
     * note that this pointer is invalidated if another item is added
     */
    basic_item<traits>* add_item(basic_item<traits> const& value);

    /**
     * @brief Adds an item to an array item
     * @param value item to add (moved)
     * @return a pointer to the item added.
     */
    basic_item<traits>* add_item(basic_item<traits> &&value);

    /**
     * @brief morphTo morph the objects into a new type. Must be a null basic_item.
     * @param newType new type to morph to
     */
    void morph_to(ItemType newType);

    /**
     * @brief Sets the value of a string item
     * @param value new value
     */
    void set_string_value(typename traits::string_type const& value);
    /**
     * @brief Sets the value of a string item using move semantics
     * @param value new value
     */
    void set_string_value(typename traits::string_type && value);

    /**
     * @copydoc set_string_value
     */
    void set_string_value(char const* value) { set_string_value(traits::make_string(value)); }

    /**
     * @brief Returns the current string value.
     * @warning Returns the string value, not the value as a string.
     * @return value of a string item
     */
    typename traits::string_type const& string_value() const;

#ifdef JSON_USE_LONG_INTEGERS

    /**
     * @brief set_integer_value sets the integer value of an integer item
     * @param value new value
     */
    void set_integer_value(int64_t value);

    /**
     * @brief Returns the integer value of an integer item
     * @return integer value
     */
    int64_t integer_value() const;

#endif

    /**
     * @brief Sets the double value of a double item
     * @param value new value
     */
    void set_double_value(double value);

    /**
     * @brief Returns the double value
     * @return double value
     */
    double double_value() const;

    /**
     * @brief Sets the bool value
     * @param value new value
     */
    void set_bool_value(bool value);

    /**
     * @brief bool_value returns the bool value of a bool item
     * @return value of item
     */
    bool bool_value() const;

    /**
     * @brief property returns a pointer to the item value of property of given name, or nullptr if not found
     * @param name of the property
     * @return pointer to property or nullptr
     */
    basic_item<traits> * property(typename traits::string_type const& name);
    /**
     * @brief property returns a const pointer to the item value of property of given name, or nullptr if not found
     * @param name of the property
     * @return const pointer to property or nullptr
     */
    basic_item<traits> const * property(typename traits::string_type const& name) const;

    basic_item<traits>* property(char const* name) { return property(traits::make_string(name)); }
    basic_item<traits> const* property(char const* name) const { return property(traits::make_string(name));}

    typename traits::array_iterator begin_array();
    typename traits::array_const_iterator begin_array() const;
    typename traits::array_iterator end_array();
    typename traits::array_const_iterator end_array() const;

    typename traits::object_iterator begin_object();
    typename traits::object_const_iterator begin_object() const;
    typename traits::object_iterator end_object();
    typename traits::object_const_iterator end_object() const;

    /**
     * @brief Removes the property of the given name from the object
     * @param name
     */
    void remove_property(typename traits::string_type const& name);

    void remove_property(char const* name) {
        return removeProperty(traits::make_string(name));
    }

    /**
     * @brief Sets the object property to the given item, taking ownership
     * @param name name of the property
     * @param item the item to set
     * @return the item inside the object
     */
    basic_item<traits>* set_property(typename traits::string_type const& name, basic_item<traits>&& item);

    /**
     * @brief Creates a sub item (requires to be an array) and return it
     * @param type the Type of the item to create
     */
    basic_item<traits>* create_item(ItemType type);

    basic_item<traits>* item(int idx);
    basic_item<traits> const * item(int idx) const;

    void morph_to_string(typename traits::string_type&& newValue);

    int child_count() const;

    template<typename visitor>
    typename visitor::result_type
    apply_visitor(visitor& v)
    {
        return boost::apply_visitor(v, data_);
    }

    template<typename visitor>
    typename visitor::result_type
    apply_visitor(visitor& v) const
    {
        return boost::apply_visitor(v, data_);
    }
};


template<typename traits>
basic_item<traits>::basic_item(bool async) noexcept :
    data_{boost::blank{}},
    complete_{!async}
{
/*    for(size_t i = 0; i < sizeof(dataptr_); ++i)
        dataptr_[i] = 0; */
}

template<typename traits>
basic_item<traits>::basic_item(ItemType type, bool async) noexcept :
    data_{boost::blank{}},
    complete_(!async)
{
    morph_to(type);
}

template<typename traits>
basic_item<traits> basic_item<traits>::clone(basic_item<traits> const& other) noexcept
{
    basic_item<traits> ret;
    ret.complete_ = other.complete_;
    ret.data_ = other.data_;
    return ret;
}

template<typename traits>
basic_item<traits>::basic_item(basic_item<traits> &&other) noexcept :
    data_{boost::blank{}},
    complete_(other.complete_)
{
    data_ = std::move(other.data_);
    other.data_ = boost::blank{};
}

template<typename traits>
basic_item<traits> & basic_item<traits>::operator =(basic_item<traits> && other) noexcept
{
    if(this != &other)
    {
        data_ = std::move(other.data_);
        other.data_ = boost::blank{};
        complete_ = other.complete_;
    }
    return *this;
}

template<typename traits>
basic_item<traits>::basic_item(typename traits::string_type &&string_value) noexcept :
    complete_{true}
{
    data_ = std::move(string_value);
}

template<typename traits>
basic_item<traits>::basic_item(typename traits::string_type const& stringValue) :
    data_{stringValue},
    complete_{true}
{
}

template<typename traits>
basic_item<traits>::basic_item(char const* stringValue) :
    basic_item(traits::make_string(stringValue))
{}

template<typename traits>
basic_item<traits>::~basic_item() noexcept
{
}

template<typename traits>
class type_visitor : public boost::static_visitor<ItemType>
{
public:
    ItemType operator()(boost::blank)const { return ItemType::Null; }
    ItemType operator()(bool)const { return ItemType::Boolean; }
    ItemType operator()(double)const { return ItemType::Double; }
#ifdef JSON_USE_LONG_INTEGERS
    ItemType operator()(std::int64_t)const { return ItemType::Integer; }
#endif
    ItemType operator()(typename traits::string_type const&)const { return ItemType::String; }
    ItemType operator()(typename traits::array_type const&)const { return ItemType::Array; }
    ItemType operator()(typename traits::object_type const&)const { return ItemType::Object; }
};

template<typename traits>
ItemType basic_item<traits>::type() const
{
    return boost::apply_visitor(type_visitor<traits>{}, data_);
}

template<typename traits>
bool basic_item<traits>::complete() const
{
    switch(type())
    {
        case ItemType::Null:
        case ItemType::Boolean:
#ifdef JSON_USE_LONG_INTEGERS
        case ItemType::Integer:
#endif
        case ItemType::Double:
        case ItemType::String:
            return true;
        case ItemType::Object:
        case ItemType::Array:
            return complete_;
    }
    return false; // should not happen
}

template<typename traits>
void basic_item<traits>::end_current_object()
{
    REQUIRE(type() == ItemType::Object || type() == ItemType::Array, "Item must be object or array");
    complete_ = true;
}

template<typename traits>
basic_item<traits>* basic_item<traits>::add_item(basic_item<traits> const& value)
{
    REQUIRE(type() == ItemType::Array, "Must be an array");
    auto& arr = boost::get<typename traits::array_type>(data_);
    traits::array_emplace_back(arr, basic_item<traits>::clone(value));
    return &arr.back();
}

template<typename traits>
basic_item<traits>* basic_item<traits>::add_item(basic_item<traits>&& value)
{
    REQUIRE(type() == ItemType::Array, "Must be an array");
    auto& arr = boost::get<typename traits::array_type>(data_);
    traits::array_emplace_back(arr, std::move(value));
    return &arr.back();
}

template<typename traits>
basic_item<traits>* basic_item<traits>::add_property(typename traits::string_type const& name, basic_item<traits> && item)
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    auto& obj = boost::get<typename traits::object_type>(data_);
    traits::object_emplace_back(obj, name, std::move(item));
    return &obj.back().second;
}

template<typename traits>
basic_item<traits>* basic_item<traits>::add_property(typename traits::string_type && name, basic_item<traits> && item)
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    auto& obj = boost::get<typename traits::object_type>(data_);
    traits::object_emplace_back(obj, std::move(name), std::move(item));
    return &obj.back().second;
}

template<typename traits>
basic_item<traits>* basic_item<traits>::create_property(typename traits::string_type const&name, ItemType itemType)
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    auto& obj = boost::get<typename traits::object_type>(data_);
    traits::object_emplace_back(obj, name, basic_item(itemType));
    return obj.back().second;
}

template<typename traits>
basic_item<traits>* basic_item<traits>::create_property(typename traits::string_type &&name, ItemType itemType)
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    auto& obj = boost::get<typename traits::object_type>(data_);
    traits::object_emplace_back(obj, std::move(name), basic_item(itemType));
    return &obj.back().second;
}

template<typename traits>
basic_item<traits> * basic_item<traits>::property(typename traits::string_type const& name)
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    auto& obj = boost::get<typename traits::object_type>(data_);
    for(auto it = obj.begin(); it != obj.end(); ++it)
    {
        if(it->first == name)
            return &it->second;
    }
    return nullptr;
}

template<typename traits>
basic_item<traits> const * basic_item<traits>::property(typename traits::string_type const& name) const
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    auto& obj = boost::get<typename traits::object_type>(data_);
    for(auto it = obj.begin(); it != obj.end(); ++it)
    {
        if(it->first == name)
            return &it->second;
    }
    return nullptr;
}

template<typename traits>
void basic_item<traits>::morph_to(ItemType newType)
{
    REQUIRE(type() == ItemType::Null, "Must be a null object");
    switch(newType)
    {
        case ItemType::Null:
            break;
        case ItemType::Boolean:
            data_ = bool{};
            break;
#ifdef JSON_USE_LONG_INTEGERS
        case ItemType::Integer:
            data_ = std::int64_t{};
            break;
#endif
        case ItemType::Double:
            data_ = double{};
            break;
        case ItemType::String:
        {
            data_ = std::string{};
            break;
        }
        case ItemType::Object:
        {
            data_ = typename traits::object_type{};
            break;
        }
        case ItemType::Array:
        {
            data_ = typename traits::array_type{};
            break;
        }
    }
}

template<typename traits>
void basic_item<traits>::set_string_value(typename traits::string_type const& value)
{
    REQUIRE(type() == ItemType::String, "Must be a string");
    data_ = value;
}

template<typename traits>
void basic_item<traits>::set_string_value(typename traits::string_type && value)
{
    REQUIRE(type() == ItemType::String, "Must be a string");
    data_ = std::move(value);
}

template<typename traits>
typename traits::string_type const& basic_item<traits>::string_value() const
{
    REQUIRE(type() == ItemType::String, "Must be a string");
    return boost::get<typename traits::string_type>(data_);
}

#ifdef JSON_USE_LONG_INTEGERS

template<typename traits>
void basic_item<traits>::set_integer_value(int64_t value)
{
    REQUIRE(type() == ItemType::Integer, "Must be an integer");
    data_ = value;
}

template<typename traits>
int64_t basic_item<traits>::integer_value() const
{
    REQUIRE(type() == ItemType::Integer, "Must be an integer");
    return boost::get<std::int64_t>(data_);
}

#endif

template<typename traits>
void basic_item<traits>::set_double_value(double value)
{
    REQUIRE(type() == ItemType::Double, "Must be a double");
    data_ = value;
}

template<typename traits>
double basic_item<traits>::double_value() const
{
    REQUIRE(type() == ItemType::Double, "Must be a double");
    return boost::get<double>(data_);
}

template<typename traits>
void basic_item<traits>::set_bool_value(bool value)
{
    REQUIRE(type() == ItemType::Boolean, "Must be a boolean");
    data_ =  value;
}

template<typename traits>
bool basic_item<traits>::bool_value() const
{
    REQUIRE(type() == ItemType::Boolean, "Must be a boolean");
    return boost::get<bool>(data_);
}

template<typename traits>
typename traits::array_iterator basic_item<traits>::begin_array()
{
    REQUIRE(type() == ItemType::Array, "Must be an array");
    return boost::get<typename traits::array_type>(data_).begin();
}

template<typename traits>
typename traits::array_const_iterator basic_item<traits>::begin_array() const
{
    REQUIRE(type() == ItemType::Array, "Must be an array");
    return boost::get<typename traits::array_type>(data_).begin();
}

template<typename traits>
typename traits::array_iterator basic_item<traits>::end_array()
{
    REQUIRE(type() == ItemType::Array, "Must be an array");
    return boost::get<typename traits::array_type>(data_).end();
}

template<typename traits>
typename traits::array_const_iterator basic_item<traits>::end_array() const
{
    REQUIRE(type() == ItemType::Array, "Must be an array");
    return boost::get<typename traits::array_type>(data_).end();
}

template<typename traits>
typename traits::object_iterator basic_item<traits>::begin_object()
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    return boost::get<typename traits::object_type>(data_).begin();
}

template<typename traits>
typename traits::object_const_iterator basic_item<traits>::begin_object() const
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    return boost::get<typename traits::object_type>(data_).begin();
}
template<typename traits>
typename traits::object_iterator basic_item<traits>::end_object()
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    return boost::get<typename traits::object_type>(data_).end();
}
template<typename traits>
typename traits::object_const_iterator basic_item<traits>::end_object() const
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    return boost::get<typename traits::object_type>(data_).end();
}

template<typename traits>
void basic_item<traits>::remove_property(typename traits::string_type const& name)
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    REQUIRE(property(name) != nullptr, "Property must be found in object");
    typename traits::object_type& obj = boost::get<typename traits::object_type>(data_);
    for(auto it = obj.begin(); it != obj.end(); ++it)
    {
        if(it->first == name)
        {
            obj.erase(it);
            return;
        }
    }
}

template<typename traits>
basic_item<traits>* basic_item<traits>::set_property(typename traits::string_type const& name, basic_item<traits> && item)
{
    REQUIRE(type() == ItemType::Object, "Must be an object");
    typename traits::object_type& obj = boost::get<typename traits::object_type>(data_);
    for(auto it = obj.begin(); it != obj.end(); ++it)
    {
        if(it->first == name)
        {
            it->second = std::move(item);
            return &it->second;
        }
    }
    traits::object_emplace_back(obj, std::move(name), std::move(item));
    return &obj.back().second;
}

template<typename traits>
basic_item<traits>* basic_item<traits>::create_item(ItemType type)
{
    REQUIRE(this->type() == ItemType::Array, "Must be an array");
    typename traits::array_type& arr = boost::get<typename traits::array_type>(data_);
    arr.emplace_back(type);
    return &arr.back();
}

template<typename traits>
void basic_item<traits>::morph_to_string(typename traits::string_type&& newValue)
{
    REQUIRE(this->type() == ItemType::Null, "Must be null item");
    data_ = std::move(newValue);
}
template<typename traits>
basic_item<traits>* basic_item<traits>::item(int idx)
{
    REQUIRE(this->type() == ItemType::Array, "Must be an array");
    typename traits::array_type& arr = boost::get<typename traits::array_type>(data_);
    return &arr[idx];
}

template<typename traits>
basic_item<traits> const * basic_item<traits>::item(int idx) const
{
    REQUIRE(this->type() == ItemType::Array, "Must be an array");
    typename traits::array_type& arr = boost::get<typename traits::array_type>(data_);
    return &arr[idx];
}

template<typename traits>
class size_visitor : public boost::static_visitor<size_t>
{
public:
    size_t operator()(bool)const { return 0; }
    size_t operator()(double)const { return 0; }
    size_t operator()(boost::blank)const { return 0; }
    size_t operator()(std::uint64_t)const { return 0; }
    size_t operator()(typename traits::string_type const& /*str*/)const { return 0; }
    size_t operator()(typename traits::array_type const& arr)const { return arr.size(); }
    size_t operator()(typename traits::object_type const& obj)const { return obj.size(); }
};

template<typename traits>
int basic_item<traits>::child_count() const
{
    REQUIRE(this->type() == ItemType::Array || this->type() == ItemType::Object, "Must be an array or an object");
    return boost::apply_visitor(size_visitor<traits>{}, data_);
}

}
}

#endif // JBC_JSON_BASIC_ITEM_H
