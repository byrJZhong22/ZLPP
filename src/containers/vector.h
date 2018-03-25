/***************************************************
 * 
 * classes:
 *     VectorStore<T, PODType>        :动态数组存储复制算法
 *     VectorBase<T>                  :动态数组基类
 *     Vector<T>                      :动态数组
 *     SortedVector<T>                :有序动态数组
 * *************************************************/

#ifndef ZLPP_CONTAINERS_Vector_H_
#define ZLPP_CONTAINERS_Vector_H_

#include <cstring>
#include <utility>
#include "interfaces.h"
#include "src/base/basic.h"

namespace zh
{

    namespace containers
    {

/*************************************************************************
 * Memory Management
 * 
 *************************************************************************/

        template <typename T, bool PODType>
        class VectorStore
        {
        };

//partial specialization
        template <typename T>
        class VectorStore<T, false>
        {
        protected:
            static void InitializeItemByDefault(void *dst, int count)
            {
                T *ds = (T *)dst;

                //placement new construction
                for (int i = 0; i < count; i++)
                {
                    new (&ds[i]) T();
                }
            }

            static void InitializeItemByMove(void *dst, void *src, int count)
            {
                T *ds = (T *)dst;
                T *ss = (T *)src;

                for (int i = 0; i < count; i++)
                {
                    new (&ds[i]) T(std::move(ss[i]));
                }
            }

            static void InitializeItemByCopy(void *dst, void *src, int count)
            {
                T *ds = (T *)dst;
                T *ss = (T *)src;

                for (int i = 0; i < count; i++)
                {
                    new (&ds[i]) T(ss[i]);
                }
            }

            static void MoveItemsInTheSameBuffer(void *dst, void *src, int count)
            {
                T *ds = (T *)dst;
                T *ss = (T *)src;

                if (ds < ss)
                {
                    for (int i = 0; i < count; i++)
                    {
                        ds[i] = std::move(ss[i]);
                    }
                }
                else if (ds > ss)
                {
                    for (int i = count - 1; i >= 0; i--)
                    {
                        ds[i] = std::move(ss[i]);
                    }
                }
            }

            static void ReleaseItems(void *dst, int count)
            {
                T *ds = (T *)dst;

                for (int i = 0; i < count; i++)
                {
                    ds[i].~T();
                }
            }

            static void *AllocateBuffer(int size)
            {
                if (size <= 0)
                    return nullptr;
                return (void *)malloc(sizeof(T) * size);
            }

            static void DeallocateBuffer(void *buffer)
            {
                if (buffer == nullptr)
                    return;
                free(buffer);
            }

        public:
        };

//partial specialization
        template <typename T>
        class VectorStore<T, true>
        {
        protected:
            static void InitializeItemByDefault(void *dst, int count)
            {
            }

            static void InitializeItemByMove(void *dst, void *src, int count)
            {
                if (count > 0)
                {
                    memcpy(dst, src, sizeof(T) * count);
                }
            }

            static void InitializeItemByCopy(void *dst, void *src, int count)
            {
                if (count > 0)
                {
                    memcpy(dst, src, sizeof(T) * count);
                }
            }

            static void MoveItemsInTheSameBuffer(void *dst, void *src, int count)
            {
                if (count > 0)
                {
                    memmove(dst, src, sizeof(T) * count);
                }
            }

            static void ReleaseItems(void *dst, int count)
            {
            }

            static void *AllocateBuffer(int size)
            {
                if (size <= 0)
                    return nullptr;
                return (void *)malloc(sizeof(T) * size);
            }

            static void DeallocateBuffer(void *buffer)
            {
                if (buffer == nullptr)
                    return;
                free(buffer);
            }

        public:
        };

//ArrayBase
        template <typename T>
        class ArrayBase : public VectorStore<T, std::is_pod<T>::value>, virtual public Iterable<T>
        {
        protected:
            class IteratorImpl : public Iterator<T>
            {
            private:
                const ArrayBase<T> *container;
                int index;

            public:
                IteratorImpl(const ArrayBase<T> *_container, int _index = -1)
                        : container(_container),
                          index(_index)
                {
                }

                Iterator<T> *Clone() const
                {
                    return new IteratorImpl(container, index);
                }

                const T &Current() const
                {
                    return container->Get(index);
                }

                int Index() const
                {
                    return index;
                }

                bool Next()
                {
                    index++;
                    return index >= 0 && index < container->Count();
                }

                void Reset()
                {
                    index = -1;
                }
            };

            void *buffer = nullptr;
            int count = 0;

            static void *AddressOf(void *bufferOfTs, int index)
            {
                return (void *)((char *)bufferOfTs + sizeof(T) * index);
            }

            const T &ItemOf(int index) const
            {
                CHECK_ERROR(index >= 0 && index < count, L"const T& ArrayBase<T>::ItemOf(int index) const#index is out of range.");
                return *((const T *)AddressOf(buffer, index));
            }

            T &ItemOf(int index)
            {   
                CHECK_ERROR(index >= 0 && index < count, L"T& ArrayBase<T>::ItemOf(int index)#index is out of range.");
                return *((T *)AddressOf(buffer, index));
            }

        public:
            ArrayBase() {}

            Iterator<T> *CreateIterator() const
            {
                return new IteratorImpl(this);
            }

            int Count() const
            {
                return count;
            }

            const T &Get(int index) const
            {
                //should check the index before used
                return ItemOf(index);
            }

            const T &operator[](int index)
            {
                return ItemOf(index);
            }
        };

//VectorBase
        template <typename T>
        class VectorBase : public ArrayBase<T>
        {
        protected:
            int capacity = 0;
            bool lessMemoryMode = false;

            int CalculateCapacity(int expected)
            {
                int result = capacity;
                while (result < expected)
                {
                    result = result * 5 / 4 + 1;
                }
                return result;
            }

            void ExpandMemorySpace(int index, int _count, bool &uninitialized)
            {
                int newCount = this->count + _count;
                if (newCount > capacity)
                {
                    int newCapacity = CalculateCapacity(newCount);
                    void *newBuffer = this->AllocateBuffer(newCapacity);
                    this->InitializeItemByMove(this->AddressOf(newBuffer, 0),
                                               this->AddressOf(this->buffer, 0),
                                               index);
                    this->InitializeItemByMove(this->AddressOf(newBuffer, index + _count),
                                               this->AddressOf(this->buffer, index),
                                               this->count - index);
                    this->ReleaseItems(this->buffer, this->count);
                    this->DeallocateBuffer(this->buffer);
                    this->capacity = newCapacity;
                    this->buffer = newBuffer;
                    uninitialized = true;
                }
                else if (index >= this->count)
                {
                    uninitialized = true;
                }
                else if (this->count - index < _count)
                {
                    this->InitializeItemByMove(this->AddressOf(this->buffer, index + _count),
                                               this->AddressOf(this->buffer, index),
                                               this->count - index);
                    this->ReleaseItems(this->AddressOf(this->buffer, index), _count - (this->count - index));
                    uninitialized = true;
                }
                else
                {
                    this->InitializeItemByMove(this->AddressOf(this->buffer, this->count),
                                               this->AddressOf(this->buffer, this->count - _count),
                                               _count);
                    this->MoveItemsInTheSameBuffer(this->AddressOf(this->buffer, index + _count), this->AddressOf(this->buffer, index), this->count - index - _count);
                    uninitialized = false;
                }

                this->count = newCount;
            }

            void ReleaseUnusedBuffer(int previousCount)
            {
                if (this->buffer && this->count < previousCount) {
                    this->ReleaseItems(this->AddressOf(this->buffer, this->count), previousCount - this->count);
                }

                if (this->lessMemoryMode && this->count <= this->capacity/2) {
                    int newCapacity = this->capacity * 5 / 8;
                    if (this->count < newCapacity) {
                        void *newBuffer = this->AllocateBuffer(newCapacity);
                        this->InitializeItemByMove(this->AddressOf(newBuffer, 0), this->AddressOf(this->buffer, 0), this->count);
                        this->ReleaseItems(this->buffer, this->count);
                        this->DeallocateBuffer(this->buffer);
                        this->capacity = newCapacity;
                        this->buffer = newBuffer;
                    }
                }
            }

        public:
            VectorBase() {}

            ~VectorBase()
            {
                this->ReleaseItems(this->buffer, this->count);
                this->DeallocateBuffer(this->buffer);
            }

            void SetLessMemoryMode(bool mode)
            {
                this->lessMemoryMode = mode;
            }

            bool RemoveAt(int index)
            {
                int previousCount = this->count;
                CHECK_ERROR(index >= 0 && index < this->count, L"VectorBase<T, K>::RemoveAt(int index)#argument index is out of range.");
                this->MoveItemsInTheSameBuffer(this->AddressOf(this->buffer, index), this->AddressOf(this->buffer, index + 1), this->count - index - 1);
                this->count--;
                this->ReleaseUnusedBuffer(previousCount);
                return true;
            }

            bool RemoveRange(int index, int _count) {
                int previousCount = this->count;
                CHECK_ERROR(index >= 0 && index < this->count, L"VectorBase<T>::RemoveRange(int index, int _count)#argument index is out of range.");
                CHECK_ERROR(index + _count >= 0 && index + _count <= this->count, L"VectorBase<T>::RemoveRange(int index, int _count)#argument _count is out of range.");
                this->MoveItemsInTheSameBuffer(this->AddressOf(this->buffer, index), this->AddressOf(this->buffer, index + _count), this->count - index - _count);
                this->count -= _count;
                this->ReleaseUnusedBuffer(previousCount);
                return true;
            }

            bool Clear() {
                int previousCount = this->count;
                this->count = 0;
                if (this->buffer) {
                    if (lessMemoryMode) {
                        this->capacity = 0;
                        this->ReleaseItems(this->buffer, this->count);
                        this->DeallocateBuffer(this->buffer);
                        this->buffer = nullptr;
                    } else {
                        this->ReleaseUnusedBuffer(previousCount);
                    }
                }
                return true;
            }
        };

        template<typename T>
        class Vector : public VectorBase<T> {
        public:
            Vector(){}

            int IndexOf(const T& item) const {
                for (int i = 0; i < this->count; i++) {
                    if (this->ItemOf(i) == item) {
                        return i;
                    }
                }
                return -1;
            }

            bool Contains(const T& item) const {
                return IndexOf(item) != -1;
            }

            int Push_back(const T& item) {
                return Insert(this->count, item);
            }

            int Insert(int index, const T& item) {
                CHECK_ERROR(index >= 0 && index <= this->count, L"Vector<T>::Insert(vint, const T&)#Argument index not in range.");
                bool uninitialized = false;
                this->ExpandMemorySpace(index, 1, uninitialized);
                if (uninitialized) {
                    new(&this->ItemOf(index)) T(item);
                } else {
                    this->ItemOf(index) = item;
                }
                return index;
            }

            bool Erase(const T& item) {
                int index = IndexOf(item);
                if (index >= 0 && index < this->count) {
                    this->RemoveAt(index);
                    return true;
                } else {
                    return false;
                }
            }

            bool Set(int index, const T& item) {
                CHECK_ERROR(index >= 0 && index < this->count, L"Vector<T>::Set(vint, const T&)#Argument index not in range.");
                this->ItemOf(index) = item;
                return true;
            }

            int Size() {
                return this->count;
            }
        };

        //sorted list
        template<typename T>
        class SortedVector: public VectorBase<T> {
        protected:
          
          int IndexOfInternal(const T &item) const
          {
            int start = 0;
            int end = this->count-1;
            int index = -1;
            while(start <= end) {
              index = (end + start) / 2;
              if (this->ItemOf(index) == item) {
                return index;
              }
              else if (this->ItemOf(index) < item) {
                start = index + 1;
              } else {
                end = index - 1;
              }
            }
            return -1;
          }

          int Insert(int index, const T &item) {
            bool uninitialized = false;
            this->ExpandMemorySpace(index, 1, uninitialized);
            if (uninitialized) {
              new(&this->ItemOf(index)) T(item);
            } else {
              item->ItemOf(index) = item;
            }
            return index;
          }

        public:
          SortedVector() {}
          
          bool Contains(const T& item) const {
            return IndexOf(item) != -1;
          }

          int IndexOf(const T& item) const {
            return IndexOfInternal(item);
          }

          int Add(const T& item) {
            if (this->count == 0) {
              return Insert(0, item);
            } else {
              int index = IndexOfInternal(item);
              // CHECK_ERROR(index >= 0 && index < this->count, L"SortedVector<T>::Add(const T& item)#Internal error, index is not in the range.");
              if (index == -1 || this->ItemOf(index) < item) {
                index++;
              }
              return Insert(index, item);
            }
          }

          bool Erase(const T& item) {
            int index = IndexOf(item);
            if (index >= 0 && index < this->count) {
              this->RemoveAt(index);
              return true;
            } else {
              return false;
            }
          }

          int Size() {
            return this->count;
          }
        };
    }
}

#endif