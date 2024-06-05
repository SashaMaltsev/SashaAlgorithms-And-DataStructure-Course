#pragma once

#include "vector.hpp"
#include "allocator.hpp"


template<typename T, typename Allocator = Allocator<T>>
class Array : protected Vector<T, Allocator> {

private:
    size_t start_;
    size_t end_;

public:

    Array();
    Array(const Array& other);
    Array(Array&& other);

    Array& operator=(const Array& other);
    Array& operator=(Array&& other);

    void Reserve(size_t new_cap);

    inline T& Front() const noexcept;
    inline T& Back() const noexcept;

    void Destroy() noexcept;

    inline void PushFront(const T& value);
    inline void PushFront(T&& value);
    inline void PushBack(const T& value);
    inline void PushBack(T&& value);

    template<typename... Args>
    void EmplaceBack(Args&&... args);

    template<typename... Args>
    void EmplaceFront(Args&&... args);

    void PopFront();
    void PopBack();

    void Swap(Array& l);

    inline size_t Size() const noexcept;
    inline T& operator[](size_t pos) const noexcept;

    ~Array();
};

template<typename T, typename Allocator>
Array<T, Allocator>::Array() : start_(0), end_(0) {
}

template<typename T, typename Allocator>
Array<T, Allocator>::Array(const Array& other) : Vector<T, Allocator>(other), start_(other.start_), end_(other.end_) {
    if (this->end_ < this->start_) {
        for (size_t i = start_; i < this->Vector<T, Allocator>::capacity_; ++i) {
            this->Vector<T, Allocator>::allocator_.construct(this->data_ + i, *(other.data_ + i));
        }
    }
}

template<typename T, typename Allocator>
Array<T, Allocator>::Array(Array&& other) : Vector<T, Allocator>(std::move(other)), start_(other.start_), end_(other.end_) {
    other.start_ = 0;
    other.end_ = 0;
}

template<typename T, typename Allocator>
Array<T, Allocator>& Array<T, Allocator>::operator=(const Array& other) {
    Array new_array(other);
    Swap(new_array);
    return *this;
}

template<typename T, typename Allocator>
Array<T, Allocator>& Array<T, Allocator>::operator=(Array&& other) {
    Array new_array(std::move(other));
    Swap(new_array);
    return *this;
}

template<typename T, typename Allocator>
void Array<T, Allocator>::Reserve(size_t new_cap) {

    if (this->Vector<T, Allocator>::capacity_ >= new_cap) {
        new_cap += this->capacity_;
    }

    T* new_place = this->allocator_.allocate(new_cap);

    if (this->size_ > 0) {
        if (this->start_ > this->end_) {
            for (size_t i = start_; i < this->capacity_; ++i) {
                this->allocator_.construct(new_place + i - start_, std::move(this->data_[i]));
                this->allocator_.destroy(this->data_ + i);
            }
            for (size_t i = 0; i <= this->end_; ++i) {
                this->allocator_.construct(new_place + i - start_ + this->Vector<T, Allocator>::capacity_, std::move(this->data_[i]));
                this->allocator_.destroy(this->data_ + i);
            }
        } else {
            for (size_t i = start_; i <= this->end_; ++i) {
                this->allocator_.construct(new_place + i - start_, std::move(this->data_[i]));
                this->allocator_.destroy(this->data_ + i);
            }
        }
    }

    this->start_ = 0;
    this->end_ = this->size_ - 1;
    this->Vector<T, Allocator>::allocator_.deallocate(this->data_, this->size_);
    this->Vector<T, Allocator>::data_ = new_place;
    this->Vector<T, Allocator>::capacity_ = new_cap;
}

template<typename T, typename Allocator>
inline T& Array<T, Allocator>::Front() const noexcept {
    return *(this->data_ + this->size_);
}

template<typename T, typename Allocator>
inline T& Array<T, Allocator>::Back() const noexcept {
    return *(this->data_ + this->end_);
}

template<typename T, typename Allocator>
inline void Array<T, Allocator>::Destroy() noexcept {
    if (this->start_ > this->end_) {
        for (size_t i = this->start_; i < this->capacity_; ++i) {
            this->allocator_.destroy(this->data_ + i);
        }
        for (size_t i = 0; i < this->end_; ++i) {
            this->allocator_.destroy(this->data_ + i);
        }
    } else {
        for (size_t i = this->start_; i < this->end_; ++i) {
            this->allocator_.destroy(this->data_ + i);
        }
    }
    this->Vector<T, Allocator>::size_ = 0;
}


template<typename T, typename Allocator>
void Array<T, Allocator>::PopFront() {

    if (this->size_ == 0) {
        return;
    }

    this->allocator_.destroy(this->data_ + start_);
    --Vector<T, Allocator>::size_;
    ++start_;
}

template<typename T, typename Allocator>
void Array<T, Allocator>::PopBack() {
    if (this->size_ == 0) {
        return;
    }

    this->allocator_.destroy(this->data_ + end_);
    --Vector<T, Allocator>::size_;
    if (this->end_ != 0) {
        --end_;
    }
}

template<typename T, typename Allocator>
void Array<T, Allocator>::Swap(Array& l) {
    Vector<T, Allocator>::Swap(l);
    std::swap(this->start_, l.start_);
    std::swap(this->end_, l.end_);
}

template<typename T, typename Allocator>
inline size_t Array<T, Allocator>::Size() const noexcept {
    return this->Vector<T, Allocator>::size_;
}

template<typename T, typename Allocator>
inline T& Array<T, Allocator>::operator[](size_t pos)  const noexcept {
    return *(this->Vector<T, Allocator>::data_ + (pos + this->start_) % this->Vector<T, Allocator>::capacity_);
}

template<typename T, typename Allocator>
template<typename... Args>
void Array<T, Allocator>::EmplaceBack(Args &&...args) {
    if (this->Vector<T, Allocator>::size_ == 0) {
        Reserve(1);
    }

    if ((this->end_ == this->Vector<T, Allocator>::capacity_ - 1) or ((this->start_ > this->end_) and (this->start_ - this->end_ <= 1))){
        Reserve(this->capacity_ * 2);
    }

    this->allocator_.construct(this->data_ + this->end_ + 1, std::forward<Args>(args)...);
    ++Vector<T, Allocator>::size_;
    ++end_;
}

template<typename T, typename Allocator>
template<typename... Args>
void Array<T, Allocator>::EmplaceFront(Args &&...args) {

    if (this->Vector<T, Allocator>::size_ == 0) {
        EmplaceBack(args...);
        return;
    }

    if (this->start_ > this->end_) {
        if (this->start_ - this->end_ <= 1) {
            Reserve(this->capacity_ * 2);
            this->start_ = this->capacity_;
        }
    } else {
        if (this->start_ == 0) {
            if (this->end_ == this->capacity_ - 1) {
                Reserve(this->capacity_ * 2);
            }
            this->start_ = this->capacity_;
        }
    }

    this->allocator_.construct(this->data_ + start_ - 1, std::forward<Args>(args)...);
    ++Vector<T, Allocator>::size_;
    --start_;
}

template<typename T, typename Allocator>
inline void Array<T, Allocator>::PushFront(const T &value) {
    EmplaceFront(value);
}

template<typename T, typename Allocator>
inline void Array<T, Allocator>::PushFront(T &&value) {
    EmplaceFront(std::move(value));
}

template<typename T, typename Allocator>
inline void Array<T, Allocator>::PushBack(const T &value) {
    EmplaceBack(value);
}

template<typename T, typename Allocator>
inline void Array<T, Allocator>::PushBack(T &&value) {
    EmplaceBack(std::move(value));
}

template<typename T, typename Allocator>
inline Array<T, Allocator>::~Array() {
    Destroy();
}
