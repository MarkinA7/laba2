#include <cstddef>
#include <stdexcept>
#include <algorithm>

template <class T>
class vector {
private:
    T* data;
    size_t size;
    size_t capacity;

public:
    // ---------- Конструкторы / Деструктор ----------
    vector() : data(nullptr), size(0), capacity(0) {}

    vector(const vector& other) {
        if (other.data == nullptr) {
            data = nullptr;
            size = 0;
            capacity = 0;
            return;
        }
        size = other.size;
        capacity = other.capacity;
        data = new T[capacity];
        std::copy(other.data, other.data + size, data);
    }

    vector(size_t count, const T& value) : data(nullptr), size(0), capacity(0) {
        if (count == 0) return;
        size = count;
        capacity = count;
        data = new T[capacity];
        for (size_t i = 0; i < size; ++i) {
            data[i] = value;
        }
    }

    vector(size_t count, T* arr) : data(nullptr), size(0), capacity(0) {
        if (count == 0 || arr == nullptr) return;
        size = count;
        capacity = count;
        data = new T[capacity];
        std::copy(arr, arr + size, data);
    }

    ~vector() {
        delete[] data;
    }

    // ---------- Присваивание ----------
    vector& operator=(const vector& other) {
        if (this == &other) return *this;
        if (capacity >= other.size) {
            size = other.size;
            std::copy(other.data, other.data + other.size, data);
        } else {
            delete[] data;
            size = other.size;
            capacity = size;
            data = new T[capacity];
            std::copy(other.data, other.data + other.size, data);
        }
        return *this;
    }

    // ---------- Доступ к элементам ----------
    T& operator[](size_t pos) { return data[pos]; }

    T& at(size_t pos) {
        if (pos >= size)
            throw std::out_of_range("Index is out of range");
        return data[pos];
    }

    T& front() {
        if (size == 0)
            throw std::out_of_range("Vector is empty");
        return data[0];
    }

    T& back() {
        if (size == 0)
            throw std::out_of_range("Vector is empty");
        return data[size - 1];
    }

    T* Data() const { return data; }

    // ---------- Вместимость ----------
    bool empty() const { return size == 0; }
    size_t Size() const { return size; }
    size_t Capacity() const { return capacity; }

    void reserve(size_t new_cap) {
        if (new_cap <= capacity) return;
        T* arr = new T[size];
        std::copy(data, data + size, arr);
        delete[] data;
        capacity = new_cap;
        data = new T[capacity];
        std::copy(arr, arr + size, data);
        delete[] arr;
    }

    // ---------- Модификаторы ----------
    void clear() {
        size = 0;
    }

    void push_back(const T& value) {
        if (size < capacity) {
            data[size] = value;
            ++size;
        } else if (size == capacity) {
            if (size == 0) {
                size = 1;
                capacity = 1;
                data = new T[capacity];
                data[0] = value;
            } else {
                capacity = 2 * size;
                T* arr = new T[size];
                std::copy(data, data + size, arr);
                delete[] data;
                data = new T[capacity];
                std::copy(arr, arr + size, data);
                delete[] arr;
                data[size] = value;
                ++size;
            }
        }
    }

    void pop_back() {
        if (size == 0)
            throw std::out_of_range("Vector is empty");
        --size;
    }

    void swap(vector& other) noexcept {
        std::swap(data, other.data);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);
    }
};

// Специализация swap
namespace std {
    template <typename T>
    void swap(vector<T>& lhs, vector<T>& rhs) noexcept {
        lhs.swap(rhs);
    }
}


int main() {

    return 0;
}
