#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class ForwardList {
private:
    class Node {
        friend class ForwardListIterator;
        friend class ForwardList;

    private:
        T data_;
        Node* next_;

    public:
        explicit Node(const T& data) : data_(data), next_(nullptr) {
        }
        Node() : data_(), next_(nullptr) {
        }
    };

public:
    class ForwardListIterator {
        friend class ForwardList;

    public:
        // NOLINTNEXTLINE
        using value_type = T;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::forward_iterator_tag;

        inline bool operator==(const ForwardListIterator& other) const {
            return this->current_ == other.current_;
        };

        inline bool operator!=(const ForwardListIterator& other) const {
            return this->current_ != other.current_;
        };

        inline reference_type operator*() const {
            return this->current_->data_;
        };

        ForwardListIterator& operator++() {
            this->current_ = this->current_->next_;
            return *this;
        };

        ForwardListIterator operator++(int) {
            ForwardListIterator copy = *this;
            this->current_ = this->current_->next_;
            return copy;
        };

        inline pointer_type operator->() const {
            return current_->data_;
        };

    private:
        explicit ForwardListIterator(Node* current) : current_(current) {
        }

    private:
        Node* current_;
    };

public:
    ForwardList() : sz_(0) {
        this->head_ = new Node();
        this->last_ = head_;
    }

    explicit ForwardList(size_t sz) : sz_(sz) {

        this->head_ = new Node();
        this->last_ = head_;

        if (sz == 0) {
            return;
        }

        Node* temp = this->head_;

        for (size_t i = 0; i < sz; ++i) {
            temp->next_ = new Node();
            temp = temp->next_;
        }

        this->last_ = temp;
    }

    ForwardList(const std::initializer_list<T>& values) {

        this->sz_ = values.size();

        if (this->sz_ == 0) {
            this->head_ = new Node();
            this->last_ = head_;
            return;
        }

        auto it = values.begin();

        this->head_ = new Node(*it);
        Node* temp = this->head_;
        ++it;

        for (; it != values.end(); ++it) {
            temp->next_ = new Node(*it);
            temp = temp->next_;
        }

        temp->next_ = new Node();
        this->last_ = temp->next_;
    }

    ForwardList(const ForwardList& other) : sz_(0) {

        if (other.sz_ == 0) {
            this->head_ = new Node();
            this->last_ = head_;
            return;
        }

        ForwardListIterator it = other.Begin();
        this->head_ = new Node(*it);
        ++it;
        Node* temp = this->head_;

        while (it != other.End()) {
            temp->next_ = new Node(*it);
            temp = temp->next_;
            ++it;
        }

        this->sz_ = other.sz_;
        temp->next_ = new Node();
        this->last_ = temp->next_;
    }

    ForwardList& operator=(const ForwardList& other) {
        ForwardList new_list(other);
        Swap(new_list);
        return *this;
    }

    ForwardListIterator Begin() const noexcept {
        return ForwardListIterator(this->head_);
    }

    ForwardListIterator End() const noexcept {
        return ForwardListIterator(this->last_);
    }

    inline T& Front() const {

        if (IsEmpty()) {
            throw ListIsEmptyException("ListIsEmpty");
        }

        return this->head_->data_;
    }

    inline bool IsEmpty() const noexcept {
        return this->head_ == this->last_;
    }

    inline size_t Size() const noexcept {
        return this->sz_;
    }

    void Swap(ForwardList& l) {
        std::swap(this->head_, l.head_);
        std::swap(this->last_, l.last_);
        std::swap(this->sz_, l.sz_);
    }

    void EraseAfter(ForwardListIterator pos) {
        Node* temp = pos.current_->next_;
        pos.current_->next_ = temp->next_;
        delete temp;
        --sz_;
    }

    void InsertAfter(ForwardListIterator pos, const T& value) {
        Node* temp = pos.current_->next_;
        pos.current_->next_ = new Node(value);
        pos.current_->next_->next_ = temp;
        ++sz_;
    }

    ForwardListIterator Find(const T& value) const {

        ForwardListIterator it(this->head_);

        while (it != End()) {
            if (it.current_->data_ == value) {
                return it;
            }
            ++it;
        }

        return it;
    }

    void Clear() noexcept {
        while (this->sz_) {
            PopFront();
        }
    }

    void PushFront(const T& value) {
        Node* prev = new Node(value);
        prev->next_ = this->head_;
        this->head_ = prev;
        ++sz_;
    }

    void PopFront() {

        if (IsEmpty()) {
            throw ListIsEmptyException("ListIsEmpty");
        }

        Node* temp = this->head_;
        this->head_ = this->head_->next_;
        delete temp;
        --sz_;
    }

    ~ForwardList() {
        Clear();
        delete this->last_;
    }

private:
    size_t sz_;
    Node* head_;
    Node* last_;
};

namespace std {
// Global swap overloading
template <typename T>
// NOLINTNEXTLINE
void swap(ForwardList<T>& a, ForwardList<T>& b) {
    a.Swap(b);
}
}  // namespace std
