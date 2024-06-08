#include "vector.hpp"

template <typename T, typename Allocator>
void Vector<T, Allocator>::Swap(Vector& l) {
    std::swap(this->data_, l.data_);
    std::swap(this->size_, l.size_);
    std::swap(this->capacity_, l.capacity_);
    std::swap(this->allocator_, l.allocator_);
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector() : data_(nullptr), size_(0), capacity_(0), allocator_() {
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(size_t count, const T& value) : size_(count), capacity_(count), allocator_() {

    this->data_ = allocator_.allocate(count);

    for (T* it = this->data_; it != this->data_ + this->size_; ++it) {
        allocator_.construct(it, value);
    }
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(const Vector& other) : size_(other.Size()), capacity_(other.Capacity()), allocator_() {

    this->data_ = allocator_.allocate(this->capacity_);

    for (size_t i = 0; i < other.Size(); ++i) {
        allocator_.construct(this->data_ + i, *(other.data_ + i));
    }
}

template <typename T, typename Allocator>
Vector<T, Allocator>& Vector<T, Allocator>::operator=(const Vector& other) {
    Vector new_vector{other};
    Swap(new_vector);
    return *this;
}

template <typename T, typename Allocator>
inline Vector<T, Allocator>::Vector(Vector&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_), allocator_() {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

template <typename T, typename Allocator>
inline Vector<T, Allocator>& Vector<T, Allocator>::operator=(Vector&& other) {
    Vector new_vector{std::move(other)};
    Swap(new_vector);
    return *this;
}

template <typename T, typename Allocator>
Vector<T, Allocator>::Vector(std::initializer_list<T> init) : size_(init.size()), capacity_(init.size()), allocator_() {

    this->data_ = allocator_.allocate(this->capacity_);
    T* it = this->data_;

    for (auto& i : init) {
        allocator_.construct(it, i);
        ++it;
    }
}

template <typename T, typename Allocator>
T& Vector<T, Allocator>::operator[](size_t pos) {
    return *(this->data_ + pos);
}

template <typename T, typename Allocator>
T& Vector<T, Allocator>::Front() const noexcept {
    return *(this->data_);
}

template <typename T, typename Allocator>
bool Vector<T, Allocator>::IsEmpty() const noexcept {
    return this->size_ == 0;
}

template <typename T, typename Allocator>
T& Vector<T, Allocator>::Back() const noexcept {
    return *(this->data_ + this->size_ - 1);
}

template <typename T, typename Allocator>
T* Vector<T, Allocator>::Data() const noexcept {
    return this->data_;
}

template <typename T, typename Allocator>
size_t Vector<T, Allocator>::Size() const noexcept {
    return this->size_;
}

template <typename T, typename Allocator>
size_t Vector<T, Allocator>::Capacity() const noexcept {
    return this->capacity_;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Reserve(size_t new_cap) {

    if (this->capacity_ >= new_cap) {
        new_cap += this->capacity_;
    }

    T* new_place = this->allocator_.allocate(new_cap);

    for (size_t i = 0; i < this->size_; ++i) {
        this->allocator_.construct(new_place + i, std::move(this->data_[i]));
        this->allocator_.destroy(this->data_ + i);
    }

    this->allocator_.deallocate(this->data_, this->size_);

    this->data_ = new_place;
    this->capacity_ = new_cap;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Clear() noexcept {

    for (size_t i = 0; i < this->size_; ++i) {
        this->allocator_.destroy(this->data_ + i);
    }

    this->size_ = 0;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Insert(size_t pos, T value) {

    if (this->capacity_ >= this->size_ + 1) {

        T next;

        for (size_t i = pos; i < this->size_ + 1; ++i) {
            next = *(this->data_ + i);
            this->allocator_.destroy(this->data_ + i);
            this->allocator_.construct(this->data_ + i, value);
            value = next;
        }

        this->allocator_.construct(this->data_ + this->size_ + 1, value);

    } else {

        T* new_place = this->allocator_.allocate(this->size_ + 1);

        for (size_t i = 0; i < pos; ++i) {
            this->allocator_.construct(new_place + i, this->data_[i]);
        }

        this->allocator_.construct(new_place + pos, value);

        for (size_t i = pos + 1; i < this->size_ + 1; ++i) {
            this->allocator_.construct(new_place + i, this->data_[i - 1]);
        }

        this->allocator_.deallocate(this->data_, this->size_);

        this->data_ = new_place;
        this->capacity_ = this->size_ + 1;
    }

    ++size_;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Erase(size_t begin_pos, size_t end_pos) {

    if (begin_pos >= this->size_) {
        return;
    }

    end_pos -= 1;

    size_t j = end_pos + 1;
    size_t i = begin_pos;
    size_t begin_pos1 = begin_pos;
    size_t end_pos1 = end_pos + 1;

    while (j < this->size_) {

        while (i < end_pos) {
            this->allocator_.destroy(this->data_ + i);
            this->allocator_.construct(this->data_ + i, this->data_[j]);
            ++i;
            ++j;
        }

        end_pos += (end_pos1 - begin_pos1);
    }

    while (i < this->size_) {
        this->allocator_.destroy(this->data_ + i);
        ++i;
    }

    size_ -= (end_pos1 - begin_pos1);
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::PushBack(const T& value) {

    if (this->capacity_ < this->size_ + 1) {
        Reserve((this->size_ + 1) * 2);
    }

    this->allocator_.construct(this->data_ + size_, std::move(value));
    ++size_;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::PushBack(T&& value) {
    if (this->capacity_ < this->size_ + 1) {
        Reserve((this->size_ + 1) * 2);
    }
    this->allocator_.construct(this->data_ + size_, std::move(value));
    ++size_;
}

template <typename T, typename Allocator>
template <class... Args>
void Vector<T, Allocator>::EmplaceBack(Args&&... args) {
    Reserve(this->size_ + 1);
    this->allocator_.construct(this->data_ + size_, std::forward<Args>(args)...);
    ++size_;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::PopBack() {

    if (this->size_ == 0) {
        return;
    }

    this->allocator_.destroy(this->data_ + size_ - 1);
    --size_;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Resize(size_t count, const T& value) {

    if (count <= this->size_) {
        for (size_t i = 0; i < count - this->size_; ++i) {
            PopBack();
        }
        return;
    }

    Reserve(count);

    for (T* it = this->data_ + this->size_; it != this->data_ + count; ++it) {
        allocator_.construct(it, value);
    }

    this->size_ = count;
}

template <typename T, typename Allocator>
void Vector<T, Allocator>::Destroy() {

    if constexpr (!std::is_same<T, void*>::value) {
        for (T* it = this->data_; it != this->data_ + this->size_; ++it) {
            allocator_.destroy(it);
        }
    } else {
        for (size_t i = 0; i < this->size_; ++i) {
            allocator_.destroy(this->data_[i]);
        }
    }

    this->allocator_.deallocate(this->data_, this->capacity_);
}

template <typename T, typename Allocator>
Vector<T, Allocator>::~Vector() {
    Destroy();
}
