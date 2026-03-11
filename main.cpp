#include <cstddef>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <cassert>
#include <utility>

// ================== Класс vector с собственным итератором ==================
template <typename T>
class vector {
public:
    // Типы, необходимые для совместимости со STL
    using value_type        = T;
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using reference         = T&;
    using const_reference   = const T&;
    using pointer           = T*;
    using const_pointer     = const T*;

    // -------------------- Итератор произвольного доступа --------------------
    class iterator : public std::iterator<std::random_access_iterator_tag, T> {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

        iterator(pointer ptr = nullptr) : ptr_(ptr) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }

        iterator& operator++() { ++ptr_; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++ptr_; return tmp; }

        iterator& operator--() { --ptr_; return *this; }
        iterator operator--(int) { iterator tmp = *this; --ptr_; return tmp; }

        iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
        iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }

        // Добавляем operator+ для использования в выражениях it + n
        iterator operator+(difference_type n) const { return iterator(ptr_ + n); }
        // Добавляем operator- для it - n (используется реже, но добавим для симметрии)
        iterator operator-(difference_type n) const { return iterator(ptr_ - n); }

        reference operator[](difference_type n) const { return ptr_[n]; }

        difference_type operator-(const iterator& other) const { return ptr_ - other.ptr_; }

        bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }
        bool operator<(const iterator& other) const { return ptr_ < other.ptr_; }
        bool operator>(const iterator& other) const { return ptr_ > other.ptr_; }
        bool operator<=(const iterator& other) const { return ptr_ <= other.ptr_; }
        bool operator>=(const iterator& other) const { return ptr_ >= other.ptr_; }

    private:
        pointer ptr_;
    };

    // -------------------- Константный итератор --------------------
    class const_iterator : public std::iterator<std::random_access_iterator_tag, const T> {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = const T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;

        const_iterator(pointer ptr = nullptr) : ptr_(ptr) {}
        const_iterator(const iterator& it) : ptr_(it.operator->()) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }

        const_iterator& operator++() { ++ptr_; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; ++ptr_; return tmp; }

        const_iterator& operator--() { --ptr_; return *this; }
        const_iterator operator--(int) { const_iterator tmp = *this; --ptr_; return tmp; }

        const_iterator& operator+=(difference_type n) { ptr_ += n; return *this; }
        const_iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }

        const_iterator operator+(difference_type n) const { return const_iterator(ptr_ + n); }
        const_iterator operator-(difference_type n) const { return const_iterator(ptr_ - n); }

        reference operator[](difference_type n) const { return ptr_[n]; }

        difference_type operator-(const const_iterator& other) const { return ptr_ - other.ptr_; }

        bool operator==(const const_iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const const_iterator& other) const { return ptr_ != other.ptr_; }
        bool operator<(const const_iterator& other) const { return ptr_ < other.ptr_; }
        bool operator>(const const_iterator& other) const { return ptr_ > other.ptr_; }
        bool operator<=(const const_iterator& other) const { return ptr_ <= other.ptr_; }
        bool operator>=(const const_iterator& other) const { return ptr_ >= other.ptr_; }

    private:
        pointer ptr_;
    };

    // -------------------- Конструкторы / Деструктор / Присваивание --------------------
    vector() : data_(nullptr), size_(0), capacity_(0) {}

    explicit vector(size_type n, const T& value = T())
        : data_(nullptr), size_(0), capacity_(0) {
        if (n > 0) {
            reserve(n);
            for (size_type i = 0; i < n; ++i)
                push_back(value);
        }
    }

    vector(const vector& other)
        : data_(nullptr), size_(0), capacity_(0) {
        reserve(other.size_);
        for (size_type i = 0; i < other.size_; ++i)
            push_back(other.data_[i]);
    }

    ~vector() {
        clear();
        ::operator delete(data_);
    }

    vector& operator=(const vector& other) {
        if (this != &other) {
            vector tmp(other);
            swap(tmp);
        }
        return *this;
    }

    // -------------------- Доступ к элементам --------------------
    reference operator[](size_type pos) { return data_[pos]; }
    const_reference operator[](size_type pos) const { return data_[pos]; }

    reference at(size_type pos) {
        if (pos >= size_)
            throw std::out_of_range("vector::at");
        return data_[pos];
    }
    const_reference at(size_type pos) const {
        if (pos >= size_)
            throw std::out_of_range("vector::at");
        return data_[pos];
    }

    reference front() { return data_[0]; }
    const_reference front() const { return data_[0]; }

    reference back() { return data_[size_ - 1]; }
    const_reference back() const { return data_[size_ - 1]; }

    pointer data() { return data_; }
    const_pointer data() const { return data_; }

    // -------------------- Итераторы --------------------
    iterator begin() { return iterator(data_); }
    const_iterator begin() const { return const_iterator(data_); }
    const_iterator cbegin() const { return const_iterator(data_); }

    iterator end() { return iterator(data_ + size_); }
    const_iterator end() const { return const_iterator(data_ + size_); }
    const_iterator cend() const { return const_iterator(data_ + size_); }

    // -------------------- Вместимость --------------------
    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    size_type capacity() const { return capacity_; }

    void reserve(size_type new_cap) {
        if (new_cap <= capacity_)
            return;
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        size_type i = 0;
        try {
            for (; i < size_; ++i) {
                new (new_data + i) T(data_[i]);
            }
        } catch (...) {
            for (size_type j = 0; j < i; ++j)
                (new_data + j)->~T();
            ::operator delete(new_data);
            throw;
        }
        for (size_type i = 0; i < size_; ++i)
            (data_ + i)->~T();
        ::operator delete(data_);
        data_ = new_data;
        capacity_ = new_cap;
    }

    // -------------------- Модификаторы --------------------
    void clear() {
        for (size_type i = 0; i < size_; ++i)
            (data_ + i)->~T();
        size_ = 0;
    }

    iterator insert(iterator pos, const T& value) {
        difference_type index = pos - begin();
        if (size_ == capacity_) {
            size_type new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
            reserve(new_cap);
        }
        for (size_type i = size_; i > static_cast<size_type>(index); --i) {
            new (data_ + i) T(std::move(data_[i-1]));
            (data_ + i - 1)->~T();
        }
        new (data_ + index) T(value);
        ++size_;
        return iterator(data_ + index);
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            size_type new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
            reserve(new_cap);
        }
        new (data_ + size_) T(value);
        ++size_;
    }

    void pop_back() {
        if (size_ > 0) {
            (data_ + size_ - 1)->~T();
            --size_;
        }
    }

    void swap(vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

private:
    T* data_;
    size_type size_;
    size_type capacity_;
};

// Операторы для удобства (вне класса)
template <typename T>
typename vector<T>::iterator operator+(typename vector<T>::iterator::difference_type n,
                                       const typename vector<T>::iterator& it) {
    return it + n;  // используется метод iterator::operator+
}

template <typename T>
typename vector<T>::const_iterator operator+(typename vector<T>::const_iterator::difference_type n,
                                             const typename vector<T>::const_iterator& it) {
    return it + n;
}

// Специализация std::swap для нашего вектора
namespace std {
    template <typename T>
    void swap(vector<T>& lhs, vector<T>& rhs) noexcept {
        lhs.swap(rhs);
    }
}

// ================== Модульные тесты ==================

// Глобальная переменная для теста исключений
bool g_throw_on_copy = false;

void test_constructors() {
    vector<int> v1;
    assert(v1.empty());
    assert(v1.size() == 0);
    assert(v1.capacity() == 0);

    vector<int> v2(5, 42);
    assert(v2.size() == 5);
    for (size_t i = 0; i < v2.size(); ++i)
        assert(v2[i] == 42);

    vector<int> v3(v2);
    assert(v3.size() == 5);
    assert(v3[0] == 42);
}

void test_access() {
    vector<int> v(3, 10);
    assert(v[0] == 10 && v[1] == 10 && v[2] == 10);
    v[1] = 20;
    assert(v[1] == 20);

    try {
        v.at(3) = 5;
        assert(false);
    } catch (const std::out_of_range&) {}

    assert(v.front() == 10);
    assert(v.back() == 10);
    v.back() = 30;
    assert(v.back() == 30);
    assert(v.front() == 10);
}

void test_push_back_and_capacity() {
    vector<int> v;
    size_t prev_cap = v.capacity();
    for (int i = 0; i < 100; ++i) {
        v.push_back(i);
        if (v.capacity() > prev_cap) {
            assert(v.capacity() >= v.size());
            prev_cap = v.capacity();
        }
    }
    assert(v.size() == 100);
    for (int i = 0; i < 100; ++i)
        assert(v[i] == i);
}

void test_pop_back_and_clear() {
    vector<int> v(10, 5);
    v.pop_back();
    assert(v.size() == 9);
    assert(v.back() == 5);
    v.clear();
    assert(v.empty());
    v.pop_back(); // ничего не должно произойти
}

void test_reserve() {
    vector<int> v;
    v.reserve(50);
    assert(v.capacity() == 50);
    assert(v.size() == 0);
    v.push_back(1);
    assert(v.capacity() >= 50);
}

void test_insert() {
    vector<int> v(3, 1);
    auto it = v.insert(v.begin() + 1, 42); // теперь работает, т.к. есть operator+ для iterator
    assert(*it == 42);
    assert(v.size() == 4);
    assert(v[0] == 1 && v[1] == 42 && v[2] == 1 && v[3] == 1);
}

void test_swap() {
    vector<int> a(2, 10);
    vector<int> b(3, 20);
    a.swap(b);
    assert(a.size() == 3 && a[0] == 20);
    assert(b.size() == 2 && b[0] == 10);
    std::swap(a, b); // через std::swap
    assert(a.size() == 2 && a[0] == 10);
}

void test_stl_algorithms() {
    vector<int> v;
    for (int i = 0; i < 10; ++i)
        v.push_back(10 - i); // 10,9,8,...,1

    // sort - должен работать после добавления operator+
    std::sort(v.begin(), v.end());
    for (int i = 0; i < 10; ++i)
        assert(v[i] == i + 1);

    // copy_if
    vector<int> even;
    std::copy_if(v.begin(), v.end(), std::back_inserter(even),
                 [](int x) { return x % 2 == 0; });
    assert(even.size() == 5);
    for (size_t i = 0; i < even.size(); ++i)
        assert(even[i] % 2 == 0);

    // find_if
    auto it = std::find_if(v.begin(), v.end(), [](int x) { return x > 5; });
    assert(it != v.end() && *it == 6);
}

void test_copy_assign() {
    vector<int> a(5, 7);
    vector<int> b;
    b = a;
    assert(b.size() == 5);
    assert(b[0] == 7 && b[4] == 7);
    b[0] = 0;
    assert(a[0] == 7);
}

void test_exception_safety() {
    struct ThrowOnCopy {
        int id;
        ThrowOnCopy(int i = 0) : id(i) {}
        ThrowOnCopy(const ThrowOnCopy& other) {
            if (g_throw_on_copy) throw std::runtime_error("copy failed");
            id = other.id;
        }
    };

    vector<ThrowOnCopy> v(5, ThrowOnCopy(1));
    g_throw_on_copy = true;
    try {
        v.push_back(ThrowOnCopy(2));
        assert(false);
    } catch (const std::runtime_error&) {
        assert(v.size() == 5);
    }
    g_throw_on_copy = false;
}

int main() {
    test_constructors();
    test_access();
    test_push_back_and_capacity();
    test_pop_back_and_clear();
    test_reserve();
    test_insert();
    test_swap();
    test_stl_algorithms();
    test_copy_assign();
    test_exception_safety();
    std::cout << "All tests passed successfully!\n";
    return 0;
}