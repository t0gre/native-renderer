#ifndef MYSTL_H
#define MYSTL_H

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

        delete[] data;
        data = new_data;
        _capacity = new_capacity;
    }

    public:
        DArray() : _capacity(0), _size(0), data(nullptr) {}

        ~DArray() {
            delete[] data;
        } 

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

        size_t size() const { return _size; }

        T& operator[](size_t index) {
            if (index >= _size) {
                throw std::out_of_range("index out of range");
            }
            return data[index];
        }
        
};

#endif