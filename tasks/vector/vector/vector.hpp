#pragma once

#include <initializer_list>
#include <memory>
#include <utility>

#include "allocator.hpp"

template <typename T, typename Allocator = Allocator<T>>
class Vector {
private:
    void Swap(Vector& l);

public:
    Vector();

    Vector(size_t count, const T& value);

    Vector(const Vector& other);

    Vector& operator=(const Vector& other);

    Vector(Vector&& other) noexcept;

    Vector& operator=(Vector&& other);

    Vector(std::initializer_list<T> init);

    T& operator[](size_t pos);

    T& Front() const noexcept;

    T& Back() const noexcept;

    T* Data() const noexcept;

    bool IsEmpty() const noexcept;

    size_t Size() const noexcept;

    size_t Capacity() const noexcept;

    void Reserve(size_t new_cap);

    void Clear() noexcept;

    void Insert(size_t pos, T value);

    void Erase(size_t begin_pos, size_t end_pos);

    void PushBack(const T& value);

    void PushBack(T&& value);

    template <class... Args>
    void EmplaceBack(Args&&... args);

    void PopBack();

    void Resize(size_t count, const T& value);

    ~Vector();

private:
    T* data_;
    size_t size_;
    size_t capacity_;
    Allocator allocator_;
    void Destroy();
};
