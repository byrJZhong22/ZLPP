/******************************************
 * 
 * Interfaces:
 *     Iterator<T>             :枚举器
 *     Iterable<T>             ：可枚举对象
 * 
 * ****************************************/

#ifndef ZLPP_CONTAINERS_INTERFACES_H_
#define ZLPP_CONTAINERS_INTERFACES_H_
#include <string>

namespace zh
{
namespace containers
{

template <typename T>
class Iterator
{
public:
  typedef T ElementType;

  virtual Iterator<ElementType> *Clone() const = 0;

  virtual const ElementType &Current() const = 0;

  virtual int Index() const = 0;

  virtual bool Next() = 0;

  virtual void Reset() = 0;

  //   virtual bool Evaluated() const {return false;}
};

template <typename T>
class Iterable
{
public:
  typedef T ElementType;

  virtual Iterator<ElementType> *CreateIterator() const = 0;
};

namespace randomaccess_internal
{
template <typename T>
struct RandomAccessable
{
  static const bool can_read = false;
  static const bool can_resize = false;
};

template <typename T>
struct RandomAccess
{
  static size_t GetCount(const T &t)
  {
    return t.count();
  }

  static const typename T::ElementType &GetValue(const T &t, size_t index)
  {
    return t.Get(index);
  }

  static void SetCount(T &t, size_t count)
  {
    t.Resize(count);
  }

  static void SetValue(T &t, size_t index, const typename T::ElementType &value)
  {
    t.Set(index, value);
  }

  static void AppendValue(T &t, const typename T::ElementType value)
  {
    t.Add(value);
  }
};
}
}
}

#endif