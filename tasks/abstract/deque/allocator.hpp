#pragma once

#include <cstddef>

template<typename T>
class Allocator {

public:
    T* allocate(size_t count);
    void deallocate(T* ptr, size_t);

    template<typename U, typename... Args>
    void construct(U* ptr, const Args&... args);

    template<typename U, typename... Args>
    void construct(U* ptr, Args&&... args);

    template<typename U>
    void destroy(U* ptr);

};


template <typename T>
T* Allocator<T>::allocate(size_t count) {
    return reinterpret_cast<T*>(new char[count * sizeof(T)]);
}

template <typename T>
void Allocator<T>::deallocate(T* ptr, size_t)  {
    delete[] reinterpret_cast<char*>(ptr);
}

template <typename T>
template <typename U, typename... Args>
void Allocator<T>::construct(U* ptr, const Args&... args) {
    new (ptr) U(args...);
}

template <typename T>
template <typename U, typename... Args>
void Allocator<T>::construct(U* ptr, Args&&... args) {
    new (ptr) U(std::forward<Args>(args)...);
}


template <typename T>
template <typename U>
void Allocator<T>::destroy(U* ptr) {
    if constexpr (std::is_same<U*, void*>::value) {
        free(ptr);
    } else {
        ptr->~U();
    }
}