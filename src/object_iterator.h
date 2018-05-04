#ifndef JBC_JSON_OBJECT_ITERATOR
#define JBC_JSON_OBJECT_ITERATOR

namespace jbc
{
namespace json
{

template<typename Item_>
struct object_iterator
{
private:
    Item_& item_;
    object_iterator()=delete;
    object_iterator(object_iterator const&)=delete;
    object_iterator& operator=(object_iterator const&)=delete;
public:
    object_iterator(Item_& item) : item_(item) {};
    typename Item_::traits::object_iterator begin() { return item_.begin_object(); }
    typename Item_::traits::object_iterator end() { return item_.end_object(); }
};

template<typename Item_>
struct object_const_iterator
{
private:
    Item_ const& item_;
    object_const_iterator()=delete;
    object_const_iterator(object_const_iterator const&)=delete;
    object_const_iterator& operator=(object_const_iterator const&)=delete;
public:
    object_const_iterator(Item_ const& item) : item_(item) {};
    typename Item_::traits::object_const_iterator begin() { return item_.begin_object(); }
    typename Item_::traits::object_const_iterator end() { return item_.end_object(); }
};

}

}

#endif
