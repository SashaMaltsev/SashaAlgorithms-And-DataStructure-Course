#pragma once

#include <initializer_list>

#include "files/array.hpp"
#include "files/exceptions.hpp"

template <typename T, typename Allocator = Allocator<T>>
class Deque {
protected:
    void Swap(Deque& l);

public:

    Deque();
    Deque(Deque&& other);
    Deque(const Deque& other);
    Deque(std::initializer_list<T> init);

    Deque& operator=(const Deque& other);
    Deque& operator=(Deque&& other);

    inline size_t Size() const noexcept;
    inline bool IsEmpty() const noexcept;
    inline T& Back() const noexcept;
    inline T& Front() const noexcept;
    inline T& operator[](size_t pos) const noexcept;

    inline void PushBack(const T& value);
    inline void PushBack(T&& value);
    inline void PushFront(const T& value);
    inline void PushFront(T&& value);
    void PopBack();
    void PopFront();
    void Clear();

    template <class... Args>
    void EmplaceBack(Args&&... args);

    template <class... Args>
    void EmplaceFront(Args&&... args);

    ~Deque();

protected:
    Array<T*> pointers_;
    size_t start_;
    size_t size_;
    size_t chunk_size_;
    Allocator allocator_;
};


template <typename T, typename Allocator>
void Deque<T, Allocator>::Swap(Deque& d) {
    this->pointers_.Swap(d.pointers_);
    std::swap(this->start_, d.start_);
    std::swap(this->size_, d.size_);
    std::swap(this->chunk_size_, d.chunk_size_);
    std::swap(this->allocator_, d.allocator_);
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque() : pointers_(), start_(0), size_(0), chunk_size_(5) {
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(Deque&& other) : pointers_(std::move(other.pointers_)), start_(other.start_), size_(other.size_), chunk_size_(other.chunk_size_), allocator_() {
    other.start_ = 0;
    other.size_ = 0;
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Deque& other) : pointers_(other.pointers_), start_(other.start_), size_(other.size_), chunk_size_(other.chunk_size_), allocator_() {
}

template <typename T, typename Allocator>
inline Deque<T, Allocator>::Deque(std::initializer_list<T> init) : pointers_(), start_(0), size_(0), chunk_size_(5), allocator_() {

    this->pointers_.Reserve((init.size() / this->chunk_size_) + 1);

    for (auto& i : init) {
        EmplaceBack(std::move(i));
    }
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(const Deque& other) {
    Deque newd(other);
    Swap(newd);
    return *this;
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(Deque&& other) {
    Deque newd{std::move(other)};
    Swap(newd);
    return *this;
}

template <typename T, typename Allocator>
inline size_t Deque<T, Allocator>::Size() const noexcept {
    return this->size_;
}

template <typename T, typename Allocator>
inline bool Deque<T, Allocator>::IsEmpty() const noexcept {
    return this->size_ == 0;
}

template <typename T, typename Allocator>
inline T& Deque<T, Allocator>::Back() const noexcept {

    if (this->size_ == 0) {
        throw DequeIsEmptyException("DequeIsEmpty");
    }

    return operator[](this->size_ - 1);
}

template <typename T, typename Allocator>
inline T& Deque<T, Allocator>::Front() const noexcept {

    if (this->size_ == 0) {
        throw DequeIsEmptyException("DequeIsEmpty");
    }

    return operator[](0);
}

template <typename T, typename Allocator>
inline T& Deque<T, Allocator>::operator[](size_t pos) const noexcept {

    if (this->size_ == 0) {
        throw DequeIsEmptyException("DequeIsEmpty");
    }

    return this->pointers_[(pos + this->start_) / this->chunk_size_][(pos + this->start_) % this->chunk_size_];
}

template <typename T, typename Allocator>
inline void Deque<T, Allocator>::PushBack(const T& value) {
    EmplaceBack(value);
}

template <typename T, typename Allocator>
inline void Deque<T, Allocator>::PushBack(T&& value) {
    EmplaceBack(std::move(value));
}

template <typename T, typename Allocator>
inline void Deque<T, Allocator>::PushFront(const T& value) {
    EmplaceFront(value);
}

template <typename T, typename Allocator>
inline void Deque<T, Allocator>::PushFront(T&& value) {
    EmplaceFront(std::move(value));
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::PopBack() {

    if (this->size_ == 0) {
        throw DequeIsEmptyException("DequeIsEmpty");
    }

    this->allocator_.destroy(this->pointers_[this->size_ / this->chunk_size_] + this->size_ % this->chunk_size_);
    --size_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::PopFront() {

    if (this->size_ == 0) {
        throw DequeIsEmptyException("DequeIsEmpty");
    }

    this->allocator_.destroy(this->pointers_[this->start_ / this->chunk_size_] + (this->start_ % this->chunk_size_));
    ++start_;
    --size_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::Clear() {
    for (size_t i = 0; i < this->size_; ++i) {
        this->allocator_.destroy(this->pointers_[(i + this->start_) / this->chunk_size_] + ((i + this->start_) % this->chunk_size_));
    }
    this->start_ = 0;
    this->size_ = 0;
    this->pointers_.Destroy();
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::EmplaceBack(Args&&... args) {

    if ((this->size_ + this->start_) == this->pointers_.Size() * this->chunk_size_) {
        this->pointers_.PushBack(this->allocator_.allocate(this->chunk_size_));
    }

    this->allocator_.construct(this->pointers_[(this->size_ + this->start_) / this->chunk_size_] + (this->size_ + this->start_) % this->chunk_size_, std::forward<Args>(args)...);
    ++size_;
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::EmplaceFront(Args&& ...args) {

    if (this->start_ == 0) {
        this->pointers_.PushFront(this->allocator_.allocate(this->chunk_size_));
        start_ = this->chunk_size_;
    }

    --start_;
    this->allocator_.construct(this->pointers_[(this->start_) / this->chunk_size_] + (this->start_) % this->chunk_size_, std::forward<Args>(args)...);
    ++size_;
}

template <typename T, typename Allocator>
Deque<T, Allocator>::~Deque() {
    Clear();
}
