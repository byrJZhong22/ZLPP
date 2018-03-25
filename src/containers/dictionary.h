/***************************************************
 * 
 * classes:
 *     Dictionary<K, V>                :字典
 *     MultiDictionary<K, V>           :多重字典
 * *************************************************/

#ifndef ZLPP_CONTAINERS_VECTOR_H_
#define ZLPP_CONTAINERS_VECTOR_H_

#include "vector.h"
#include "interfaces.h"
#include "pair.h"

namespace zh
{
namespace containers
{
template <typename K, typename V>
class Dictionary : virtual public Iterable<Pair<K, V>>
{
  private:
    typedef SortedVector<K> KeyContainer;
    typedef Vector<V> ValueContainer;
    KeyContainer keys;
    ValueContainer values;

  public:
    class IteratorImpl : public Iterator<Pair<K, V>>
    {
      private:
        Dictionary<K, V> *container;
        int index;
        Pair<K, V> current;

        void UpdateCurrent()
        {
            if (index < container->Count())
            {
                current.key = container->Keys().Get(index);
                current.value = container->Values().Get(index);
            }
        }

      public:
        IteratorImpl(const Dictionary<K, V> *_container, int _index = -1) : container(_container), index(_index) {}

        Iterator<Pair<K, V>> *Clone() const
        {
            return new IteratorImpl(container, index);
        }

        const Pair<K, V> &Current() const
        {
            return current;
        }

        int Index() const
        {
            return index;
        }

        bool Next()
        {
            index++;
            UpdateCurrent();
            return index >= 0 && index < container->Count();
        }

        void Reset()
        {
            index = -1;
            UpdateCurrent();
        }
    };

    

    Dictionary() {}

    ~Dictionary() {Clear();}

    Iterator<Pair<K, V>> *CreateIterator() const
    {
        return new IteratorImpl(this);
    }

    void SetLessMemoryMode(bool mode)
    {
        keys.SetLessMemoryMode(mode);
        values.SetLessMemoryMode(mode);
    }

    int Count()
    {
        return keys.Size();
    }

    const KeyContainer &Keys()
    {
        return keys;
    }

    const ValueContainer &Values()
    {
        return values;
    }

    const V &Get(const K &key)
    {
        return values.Get(keys.IndexOf(key));
    }

    const V &operator[](const K &key)
    {
        return values.Get(keys.IndexOf(key));
    }

    bool Set(K &key, V &value)
    {
        int index = keys.IndexOf(key);
        if (index == -1)
        {
            index = keys.Add(key);
            values.Insert(index, value);
        }
        else
        {
            values[index] = value;
        }
        return true;
    }

    bool Add(K &key, V &value)
    {
        CHECK_ERROR(!keys.Contains(key), L"Dictionary<K, V>::Add(K& key, V& value)#key already exists.");
        int index = keys.Add(key);
        values.Insert(index, value);
    }

    bool Add(const Pair<K, V> &pair)
    {
        this->Add(pair.key, pair.value);
    }

    bool Remove(const K &key)
    {
        int index = keys.IndexOf(key);
        if (index != -1)
        {
            keys.RemoveAt(index);
            values.RemoveAt(index);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Clear()
    {
        keys.Clear();
        values.Clear();
        return true;
    }
};

template <typename K, typename V>
class MultiDictionary : virtual public Iterable<Pair<K, V>>
{   
  protected:
    typedef SortedVector<K> KeyContainer;
    typedef Vector<V> ValueContainer;
    KeyContainer keys;
    Vector<ValueContainer *> values;

  public:
    class IteratorImpl : public Iterator<Pair<K, V>>
    {
      public:
        IteratorImpl(const MultiDictionary<K, V> *_container, int _keyindex = -1, int _valueindex = -1) : 
                        container(_container), 
                        keyIndex(_keyindex),
                        valueIndex(_valueindex) {}

        Iterator<Pair<K, V>> *Clone() const
        {
            return new IteratorImpl(container, keyIndex, valueIndex);
        }

        const Pair<K, V> &Current() const
        {
            return current;
        }

        int Index() const
        {
            if (keyIndex>=0 && keyIndex<container->Count()) {
                int index = 0;
                for (int i = 0; i < keyIndex; i++) {
                    index += container->GetByIndex(i).Count();
                }
                return index+valueIndex;
            } else {
                return -1;
            }

        }

        bool Next()
        {
            if(keyIndex==-1)
            {
                keyIndex=0;
            }
            while(keyIndex<container->Count())
            {
                valueIndex++;
                const ValueContainer& values=container->GetByIndex(keyIndex);
                if(valueIndex<values.Count())
                {
                    UpdateCurrent();
                    return true;
                }
                else
                {
                    keyIndex++;
                    valueIndex=-1;
                }
            }
            return false;
        }

        void Reset()
        {
            keyIndex = -1;
            UpdateCurrent();
        }

      private:
        const MultiDictionary<K, V> *container;
        int keyIndex;
        int valueIndex;
        Pair<K, V> current;
        void UpdateCurrent() {
            if (keyIndex < container->Count()) {
                const ValueContainer& values = container->GetByIndex(keyIndex);
                if (valueIndex < values.Count())
                {
                    current.key = container->Keys().Get(keyIndex);
                    current.value = values.Get(valueIndex);
                }
            }
        }
    };

    MultiDictionary() {}

    ~MultiDictionary() {Clear();}
    
    Iterator<Pair<K, V>>* CreateIterator() const {
        return new IteratorImpl(this);
    }

    const KeyContainer& Keys() const {
        return keys;
    }

    int Count() const {
        return keys.Count();
    }

    const ValueContainer& Get(const K& key) const {
        return *values.Get(keys.IndexOf(key));
    }

    const ValueContainer& GetByIndex(int index) const {
        return *values.Get(index);
    }

    const ValueContainer& operator[](const K& key) const {
        return *values.Get(keys.IndexOf(key));
    }

    bool Contains(const K& key) const {
        return keys.Contains(key);
    }

    bool Contains(const K& key, const V& value) const {
        int index = keys.Contains(key);
        if (index != -1) {
            return values.Get(index)->Contains(value);
        } else {
            return false;
        }
    }

    bool Add(Pair<K, V>& value) {
        return Add(value.key, value.value);
    }

    bool Add(const K& key, const V&value) {
        int index = keys.IndexOf(key);
        ValueContainer *target = 0;
        if (index != -1) {
            target = values[index];
        } else {
            target = new ValueContainer;
            values.Insert(keys.Add(key), target);
        }
        target->Push_back(value);
        return true;
    }

    bool Remove(const K& key) {
        int index = keys.IndexOf(key);
        if (index != -1) {
            keys.RemoveAt(index);
            ValueContainer *target = values[index];
            values.RemoveAt(index);
            delete target;
            return true;
        } else {
            return false;
        }
    }

    bool Remove(const K& key, const V& value) {
        int index = keys.IndexOf(key);
        if (index != -1) {
            ValueContainer *target = values[index];
            target->Erase(value);
            if (target->Count() == 0) {
                keys.RemoveAt(index);
                values.RemoveAt(index);
                delete target;
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Clear() {
        for (int i = 0; i < values.Count(); i++) {
            delete values[i];
        }
        keys.Clear();
        values.Clear();
        return true;
    }
};
}
}
#endif