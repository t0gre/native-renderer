#ifndef MYSTL_H
#define MYSTL_H

#include <memory>
#include <stdexcept>

template<class T>
class DArray {
    private:
        size_t _capacity;
        size_t _size;
        std::unique_ptr<T[]> data;;

    void resize(const size_t new_capacity) {
        std::unique_ptr<T[]> new_data = std::make_unique<T[]>(new_capacity);

        for (size_t i = 0; i < _size; i++) {
            new_data[i] = data[i];
        }

        data = std::move(new_data);
        _capacity = new_capacity;
    }

    public:
        DArray() : _capacity(0), _size(0), data(nullptr) {}

        // Move constructor
        DArray(DArray&& other) noexcept
            : _capacity(other._capacity),
              _size(other._size),
              data(std::move(other.data))
            {
                other._capacity = 0;
                other._size = 0;
            }

        // Move assignment
        DArray& operator=(DArray&& other) noexcept {
                if (this != &other) {
                    _capacity = other._capacity;
                    _size = other._size;
                    data = std::move(other.data);

                    other._capacity = 0;
                    other._size = 0;
                }
                return *this;
            }

        ~DArray() {}

        void push_back(const T& value) {
            if (_size == _capacity) {
                size_t new_capacity = 10;
                if ( _capacity > 0 ) {
                    new_capacity = _capacity * 2;;
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

        T* begin() { return data.get(); }
        T* end()   { return data.get() + _size; }

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
        
};

#endif