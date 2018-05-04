#ifndef JBC_JSON_ITEMLOCATOR_H
#define JBC_JSON_ITEMLOCATOR_H

namespace jbc
{
namespace json
{

enum class item_locator_type {
    Invalid,
    Item,
    ObjectIterator,
    ArrayIterator
};

/**
 * @brief The item locator class gives all information needed to iterate through a json object, advance into it, as a textual output
 * representation (not pretty-printed). It thus makes it possible to get string representation of a partial json object, stopping at an arbitrary location,
 * making it suitable to write in a fixed size buffer, whatever the underlying data is.
 */
template<typename Item_>
struct item_locator
{
    item_locator();

    ~item_locator();

    /**
     * @brief locator types, used when iterating through the object
     */
    item_locator_type locatorType;

    /**
     * @brief currentItem is the current item.
     */
    Item_ const* currentItem;

    typename Item_::traits::array_const_iterator arrayit;
    typename Item_::traits::object_const_iterator objectit;
    /**
     * @brief currentOffset is the current offset in the current value in the output data. The meaning of this value depends on the current item_locator_type
     */
    int currentOffset = 0;

    /**
     * @brief currentOffsetInData is the offset in the data (only for string values). This is different than current offset, because it is only for
     * string values, and is an offset in the source data, not the output one.
     */
    int currentOffsetInData = 0;

    /**
     * @brief offsetInChar is the offset inside a value, for chars that needs to be outputted in more than one char, or for integer values
     */
    int offsetInValue = 0;

};

template<typename Item_>
item_locator<Item_>::item_locator() :
    currentItem(nullptr)
{
}

template<typename Item_>
item_locator<Item_>::~item_locator()
{
}

}
}

#endif // JBC_JSON_ITEMLOCATOR_H
