#ifndef JBC_JSON_ASYNCITEMPRINTER_H
#define JBC_JSON_ASYNCITEMPRINTER_H

#include "basic_item.h"
#include "item_locator.h"
#include <cstdlib>
#include <cassert>
#include <vector>

namespace jbc
{
namespace json
{

template<template<class> class container, typename Item_>
class async_item_printer
{
    container<item_locator<Item_> > currentLocationStack;
    Item_ const& baseItem;
    typedef typename Item_::traits::char_type char_type;
    typedef typename Item_::traits::string_type string_type;
public:
    async_item_printer(Item_ const& item);
    ~async_item_printer();

    /**
     * @brief write_into writes the object data in the buffer
     * @param buf buf to write into
     * @param size size (length) of the buffer
     * @return number of char_type written
     */
    int write_into(typename Item_::traits::char_type buf[], int size);

    bool finished() const { return currentLocationStack.size() == 0; }

    char_type tbuf[30];
};

template<template<class> class container, typename Item_>
async_item_printer<container, Item_>::async_item_printer(Item_ const& item) :
    baseItem(item)
{
    REQUIRE(item.type() == ItemType::Array || item.type() == ItemType::Object, "Item must be a valid json root object");
    item_locator<Item_> locator;
    locator.currentItem = &item;
    locator.currentOffset = 0;
    if(item.type() == ItemType::Array)
        locator.locatorType = item_locator_type::ArrayIterator;
    else
        locator.locatorType = item_locator_type::ObjectIterator;
    currentLocationStack.push_back(locator);
}

template<template<class> class container, typename Item_>
async_item_printer<container, Item_>::~async_item_printer()
{

}

template<template<class> class container, typename Item_>
int async_item_printer<container, Item_>::write_into(char_type* buf, int size)
{
    int i;
    bool exitLoop = false;
    for(i = 0; i < size && currentLocationStack.size() > 0 && !exitLoop; ++i)
    {
        item_locator<Item_>& currentLocation = currentLocationStack.back();

        if(currentLocation.currentOffset == 0)
        {
            switch(currentLocation.currentItem->type())
            {
                case ItemType::Array:
                    currentLocation.locatorType = item_locator_type::ArrayIterator;
                    break;
                case ItemType::Object:
                    currentLocation.locatorType = item_locator_type::ObjectIterator;
                    break;
                default:
                    currentLocation.locatorType = item_locator_type::Item;
                    break;
            }
            currentLocation.currentOffset = 1;
            currentLocation.currentOffsetInData = 0;
            currentLocation.offsetInValue = 0;
        }

        switch(currentLocation.locatorType)
        {
            case item_locator_type::ArrayIterator:
                switch(currentLocation.currentOffset)
                {
                    case 1: // start of object
                    {
                        currentLocation.arrayit = currentLocation.currentItem->beginArray();
                        if(currentLocation.arrayit != currentLocation.currentItem->endArray()) // not an empty array
                        {
                            buf[i] = char_type('[');
                            currentLocation.currentOffset = 2;
                            item_locator<Item_> loc;
                            loc.currentItem = &*currentLocation.arrayit;
                            currentLocationStack.push_back(loc);
                        }
                        else if(currentLocation.currentItem->complete()) // if array is incomplete, it may be not empty in fact !!!
                        {
                            buf[i] = char_type('[');
                            currentLocation.currentOffset = 3;
                        }
                        else
                            exitLoop = true;
                        break;
                    }
                    case 2:  // array content
                    {
                        auto nextit = currentLocation.arrayit; // don’t increment current to end of array, keep current position if nothing new
                        ++nextit;
                        if(nextit != currentLocation.currentItem->endArray())
                        {
                            currentLocation.arrayit = nextit;
                            buf[i] = char_type(',');
                            item_locator<Item_> loc;
                            loc.currentItem = &*currentLocation.arrayit;
                            currentLocationStack.push_back(loc);
                        }
                        else
                        {
                            if(currentLocation.currentItem->complete())
                            {
                                buf[i] = char_type(']');
                                currentLocationStack.pop_back();
                            }
                            else
                                exitLoop = true;
                        }
                        break;
                    }
                    case 3: // empty array
                        buf[i] = char_type(']');
                        currentLocationStack.pop_back();
                        break;
                }
                break;
            case item_locator_type::ObjectIterator:
                switch(currentLocation.currentOffset)
                {
                    case 1: // start of object
                    {
                        buf[i] = char_type('{');
                        currentLocation.objectit = currentLocation.currentItem->beginObject();
                        if(currentLocation.objectit == currentLocation.currentItem->endObject())
                            currentLocation.currentOffset = 6; // empty object
                        else
                            currentLocation.currentOffset = 2;
                        break;
                    }
                    case 2: // 2 is the start of a key
                    {
                        buf[i] = char_type('"');
                        currentLocation.currentOffset = 3;
                        currentLocation.currentOffsetInData = 0;
                        break;
                    }
                    case 3: // 3 is inside the key
                        if(currentLocation.currentOffsetInData < (int)currentLocation.objectit->first.size())
                        {
                            char_type c = currentLocation.objectit->first[currentLocation.currentOffsetInData];
                            if(c == char_type('"') && currentLocation.offsetInValue == 0)
                            {
                                buf[i] = char_type('\\');
                                currentLocation.offsetInValue = 1;
                            }
                            else
                            {
                                buf[i] = c;
                                currentLocation.offsetInValue = 0;
                                currentLocation.currentOffsetInData += 1;
                            }
                        }
                        else
                        {
                            buf[i] = char_type('"');
                            currentLocation.currentOffset = 4;
                        }
                        break;
                    case 4: // 4 is the :
                    {
                        buf[i] = char_type(':');
                        currentLocation.currentOffset = 5;
                        item_locator<Item_> loc;
                        loc.currentItem = &currentLocation.objectit->second;
                        currentLocationStack.push_back(loc);
                        break;
                    }
                    case 5:
                        currentLocation.objectit++; // increment to move to next object
                        if(currentLocation.objectit != currentLocation.currentItem->endObject())
                        {
                            buf[i] = char_type(',');
                            currentLocation.currentOffset = 2;
                        }
                        else
                        {
                            buf[i] = char_type('}');
                            currentLocationStack.pop_back();
                        }
                        break;
                    case 6: // empty object
                        buf[i] = char_type('}');
                        currentLocationStack.pop_back();
                        break;
                }
                break;
            case item_locator_type::Item:
            {
                switch(currentLocation.currentItem->type())
                {
                    case ItemType::Boolean:
                        currentLocation.currentOffsetInData += 1;
                        switch(currentLocation.currentOffsetInData)
                        {
                            case 1:
                                buf[i] = currentLocation.currentItem->boolValue()?char_type('t'):char_type('f');
                                break;
                            case 2:
                                buf[i] = currentLocation.currentItem->boolValue()?char_type('r'):char_type('a');
                                break;
                            case 3:
                                buf[i] = currentLocation.currentItem->boolValue()?char_type('u'):char_type('l');
                                break;
                            case 4:
                                if(currentLocation.currentItem->boolValue())
                                {
                                    buf[i] = char_type('e');
                                    currentLocationStack.pop_back();
                                }
                                else
                                    buf[i] = char_type('s');
                                break;
                            case 5:
                                buf[i] = char_type('e');
                                currentLocationStack.pop_back();
                        }
                        break;
                    case ItemType::Null:
                        currentLocation.currentOffsetInData += 1;
                        switch(currentLocation.currentOffsetInData)
                        {
                            case 1:
                                buf[i] = char_type('n');
                                break;
                            case 2:
                                buf[i] = char_type('u');
                                break;
                            case 3:
                                buf[i] = char_type('l');
                                break;
                            case 4:
                                buf[i] = char_type('l');
                                currentLocationStack.pop_back();
                                break;
                        }
                        break;
#ifdef JSON_USE_LONG_INTEGERS
                    case ItemType::Integer:
                        if(currentLocation.currentOffsetInData == 0)
                            std::snprintf(tbuf, sizeof(tbuf), "%ld", currentLocation.currentItem->integerValue());
                        if(tbuf[currentLocation.currentOffsetInData] == 0)
                        {
                            --i; // decrement i, we dont write anything
                            currentLocationStack.pop_back();
                        }
                        else
                        {
                            buf[i] = char_type(tbuf[currentLocation.currentOffsetInData]);
                            currentLocation.currentOffsetInData += 1;
                        }
                        break;
#endif
                    case ItemType::Double:
                        if(currentLocation.currentOffsetInData == 0)
                            std::snprintf(tbuf, sizeof(tbuf), "%g", currentLocation.currentItem->doubleValue());
                        if(tbuf[currentLocation.currentOffsetInData] == 0)
                        {
                            --i;
                            currentLocationStack.pop_back();
                        }
                        else
                        {
                            buf[i] = char_type(tbuf[currentLocation.currentOffsetInData]);
                            currentLocation.currentOffsetInData += 1;
                        }
                        break;
                    case ItemType::String:
                        if(currentLocation.currentOffset == 1)
                        {
                            buf[i] = char_type('"');
                            currentLocation.currentOffset = 2;
                        }
                        else if(currentLocation.currentOffset == 2)
                        {
                            string_type const& s = currentLocation.currentItem->stringValue();
                            if(currentLocation.offsetInValue < (int)s.size())
                            {
                                char_type c = s[currentLocation.offsetInValue];
                                if((c == char_type('"') || c == char_type('\\') || c == char_type('/'))
                                        && currentLocation.currentOffsetInData == 0) // characters escaped by \ followed by themselves
                                {
                                    buf[i] = char_type('\\');
                                    currentLocation.currentOffsetInData = 1;
                                }
                                else if(c == char_type('\n')) // new line
                                {
                                    if(currentLocation.currentOffsetInData == 0)
                                    {
                                        buf[i] = char_type('\\');
                                        currentLocation.currentOffsetInData = 1;
                                    }
                                    else
                                    {
                                        buf[i] = char_type('n');
                                        currentLocation.currentOffsetInData = 0;
                                        currentLocation.offsetInValue += 1;
                                    }
                                }
                                else if(c == char_type('\r')) // carriage return
                                {
                                    if(currentLocation.currentOffsetInData == 0)
                                    {
                                        buf[i] = char_type('\\');
                                        currentLocation.currentOffsetInData = 1;
                                    }
                                    else
                                    {
                                        buf[i] = char_type('r');
                                        currentLocation.currentOffsetInData = 0;
                                        currentLocation.offsetInValue += 1;
                                    }
                                }
                                else if(c == char_type('\b')) // backspace
                                {
                                    if(currentLocation.currentOffsetInData == 0)
                                    {
                                        buf[i] = char_type('\\');
                                        currentLocation.currentOffsetInData = 1;
                                    }
                                    else
                                    {
                                        buf[i] = char_type('b');
                                        currentLocation.currentOffsetInData = 0;
                                        currentLocation.offsetInValue += 1;
                                    }
                                }
                                else if(c == char_type('\f'))
                                {
                                    if(currentLocation.currentOffsetInData == 0)
                                    {
                                        buf[i] = char_type('\\');
                                        currentLocation.currentOffsetInData = 1;
                                    }
                                    else
                                    {
                                        buf[i] = char_type('f');
                                        currentLocation.currentOffsetInData = 0;
                                        currentLocation.offsetInValue += 1;
                                    }
                                }
                                else if(c == char_type('\t'))
                                {
                                    if(currentLocation.currentOffsetInData == 0)
                                    {
                                        buf[i] = char_type('\\');
                                        currentLocation.currentOffsetInData = 1;
                                    }
                                    else
                                    {
                                        buf[i] = char_type('t');
                                        currentLocation.currentOffsetInData = 0;
                                        currentLocation.offsetInValue += 1;
                                    }
                                }
                                else
                                {
                                    buf[i] = c;
                                    currentLocation.currentOffsetInData = 0;
                                    currentLocation.offsetInValue += 1;
                                }
                            }
                            else
                            {
                                buf[i] = char_type('"');
                                currentLocationStack.pop_back();
                            }
                        }
                        break;
                    case ItemType::Object:
                    case ItemType::Array:
                    default:
                        assert(false); // should not happen
                }
                break;
            }
            case item_locator_type::Invalid:
                assert(false); // should not happen;
                break;
        }
    }
    if(exitLoop)
        i -= 1; // nothing written in the last loop if exit loop is true;
    return i;

}

}
}

#endif // JBC_JSON_ASYNCITEMPRINTER_H
