#ifndef MYSTL_H
#define MYSTL_H

#include <memory>
#include <stdexcept>

template<class T>
class DArray {
    private:
        size_t _capacity;
        size_t _size;
        T* data;

        void resize(const size_t new_capacity) {
            T* new_data = new T[new_capacity];

            for (size_t i = 0; i < _size; i++) {
                new_data[i] = data[i];
            }
            
            // free old buffer and take ownership of new buffer
            delete[] data;
            data = new_data;
            _capacity = new_capacity;
        }

    public:
        DArray() : _capacity(0), _size(0), data(nullptr) {}

        // Copy constructor (deep copy)
        DArray(const DArray& other)
            : _capacity(other._capacity),
              _size(other._size),
              data(other._capacity ? new T[other._capacity] : nullptr)
        {
            for (size_t i = 0; i < _size; ++i)
                data[i] = other.data[i];
        }

        // Copy assignment (deep copy, strong exception safety)
        DArray& operator=(const DArray& other) {
            if (this != &other) {
                // Make a temporary deep copy first
                DArray temp(other);
                // Swap with *this* (strong guarantee)
                std::swap(_capacity, temp._capacity);
                std::swap(_size, temp._size);
                std::swap(data, temp.data);
            }
            return *this;
        }

        // Move constructor - take ownership of other's buffer
        DArray(DArray&& other) noexcept
            : _capacity(other._capacity),
              _size(other._size),
              data(other.data)
        {
            // leave other in a valid empty state
            other._capacity = 0;
            other._size = 0;
            other.data = nullptr;
        }

        // Move assignment - free current storage, take ownership of other's storage
        DArray& operator=(DArray&& other) noexcept {
                if (this != &other) {
                    delete[] data;
                    _capacity = other._capacity;
                    _size = other._size;
                    data = other.data;

                    other._capacity = 0;
                    other._size = 0;
                    other.data = nullptr;
                }
                return *this;
            }

        void erase(const size_t index) {
                if (index >= _size) {
                    throw std::out_of_range("erase index out of range");
                }

                // Move elements left by one
                for (size_t i = index; i < _size - 1; ++i) {
                    data[i] = data[i + 1];
                }

                --_size;
            }


        void push_back(const T& value) {
            if (_size == _capacity) {
                size_t new_capacity = 10;
                if ( _capacity > 0 ) {
                    new_capacity = _capacity * 2;
                }
                resize(new_capacity);
            }

            data[_size++] = value;
        }

        void pop_back() {
            if (_size > 0) {
                --_size;
            }
        }

        T* begin() { return data; }
        T* end()   { return data + _size; }
        const T* begin() const { return data; }
        const T* end()   const { return data + _size; }

        T* addr(size_t index) {
            return &data[index];
        }

        size_t size() const { return _size; }

        T& operator[](size_t index) {
            if (index >= _size) {
                throw std::out_of_range("index out of range");
            }
            return data[index];
        }

        ~DArray() {
            delete[] data;
        }
        
};

#endif