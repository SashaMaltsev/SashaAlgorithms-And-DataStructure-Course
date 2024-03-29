#pragma once

#include <fmt/core.h>

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>

#include "exceptions.hpp"

template <typename T>
class List {
private:
    class Node {
        friend class ListIterator;
        friend class List;

    private:
        T data_;
        Node* next_;
        Node* prev_;

    public:
        explicit Node(const T& data) : data_(data), next_(nullptr), prev_(nullptr) {
        }
        Node() : data_(), next_(nullptr), prev_(nullptr) {
        }
    };

public:
    class ListIterator {
        friend class List;

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
        using iterator_category = std::bidirectional_iterator_tag;

        inline bool operator==(const ListIterator& other) const {
            return this->current_ == other.current_;
        };

        inline bool operator!=(const ListIterator& other) const {
            return this->current_ != other.current_;
        };

        inline reference_type operator*() const {
            return this->current_->data_;
        };

        ListIterator& operator++() {
            this->current_ = this->current_->next_;
            return *this;
        };

        ListIterator operator++(int) {
            ListIterator copy = *this;
            this->current_ = this->current_->next_;
            return copy;
        };

        ListIterator& operator--() {
            this->current_ = this->current_->prev_;
            return *this;
        };

        ListIterator operator--(int) {
            ListIterator copy = *this;
            this->current_ = this->current_->prev_;
            return copy;
        };

        inline pointer_type operator->() const {
            return current_->data_;
        };

    private:
        ListIterator(Node* current) : current_(current) {
        }

    private:
        Node* current_;
    };

public:
    List() : sz_(0) {
        this->head_ = new Node();
        this->last_ = this->head_;
    }

    explicit List(size_t sz) {

        this->head_ = new Node();
        this->last_ = this->head_;

        for (size_t i = 0; i < sz; ++i) {
            this->head_->prev_ = new Node();
            this->head_->prev_->next_ = this->head_;
            this->head_ = this->head_->prev_;
        }

        this->sz_ = sz;
    }

    List(const std::initializer_list<T>& values) : sz_(0) {

        this->head_ = new Node();
        this->last_ = this->head_;

        for (auto it = values.end(); it != values.begin();) {
            --it;
            this->head_->prev_ = new Node(*it);
            this->head_->prev_->next_ = this->head_;
            this->head_ = this->head_->prev_;
        }

        this->sz_ = values.size();
    }

    List(const List& other) : sz_(0) {

        this->head_ = new Node();
        this->last_ = this->head_;

        Node* t = other.head_;

        while (t != other.last_) {
            PushBack(t->data_);
            t = t->next_;
        }

        this->sz_ = other.sz_;
    }

    List& operator=(const List& other) {
        List new_list(other);
        Swap(new_list);
        return *this;
    }

    ListIterator Begin() const noexcept {
        return ListIterator(this->head_);
    }

    ListIterator End() const noexcept {
        return ListIterator(this->last_);
    }

    inline T& Front() const {
        return this->head_->data_;
    }

    inline T& Back() const {
        return this->last_->prev_->data_;
    }

    inline bool IsEmpty() const noexcept {
        return this->head_ == this->last_;
    }

    inline size_t Size() const noexcept {
        return this->sz_;
    }

    void Swap(List& l) {
        std::swap(this->head_, l.head_);
        std::swap(this->last_, l.last_);
        std::swap(this->sz_, l.sz_);
    }

    ListIterator Find(const T& value) const {

        ListIterator it(this->head_);

        while (it != End()) {
            if (it.current_->data_ == value) {
                return it;
            }
            ++it;
        }

        return it;
    }

    void Erase(ListIterator pos) {

        if (pos.current_ == this->head_) {
            this->head_ = this->head_->next_;
            this->head_->prev_ = nullptr;
            delete pos.current_;
            --sz_;
            return;
        }

        pos.current_->prev_->next_ = pos.current_->next_;
        pos.current_->next_->prev_ = pos.current_->prev_;
        delete pos.current_;
        --sz_;
    }

    void Insert(ListIterator pos, const T& value) {
        Node* nd = new Node(value);

        if (pos.current_ == this->head_) {
            this->head_->prev_ = nd;
            nd->next_ = this->head_;
            this->head_ = nd;
            ++sz_;
            return;
        }

        if (pos.current_ == this->last_) {
            this->last_->prev_ = nd;
            nd->next_ = this->last_;
            ++sz_;
            return;
        }

        pos.current_->prev_->next_ = nd;
        nd->prev_ = pos.current_->prev_;
        pos.current_->prev_ = nd;
        nd->next_ = pos.current_;

        ++sz_;
    }

    void Clear() noexcept {
        while (this->sz_) {
            PopFront();
        }
    }

    void PushBack(const T& value) {
        Node* next = new Node(value);

        if (this->sz_ == 0) {
            this->head_ = next;
            next->next_ = this->last_;
            this->last_->prev_ = this->head_;
            ++sz_;
            return;
        }

        next->prev_ = this->last_->prev_;
        next->next_ = this->last_;
        this->last_->prev_->next_ = next;
        this->last_->prev_ = next;

        ++sz_;
    }

    void PushFront(const T& value) {
        Node* prev = new Node(value);
        this->head_->prev_ = prev;
        this->head_->prev_->next_ = this->head_;
        this->head_ = this->head_->prev_;
        ++sz_;
    }

    void PopBack() {
        if (IsEmpty()) {
            throw ListIsEmptyException("ListIsEmpty");
        }

        --sz_;

        Node* prev_node = this->last_->prev_;
        prev_node->prev_->next_ = this->last_;
        this->last_->prev_ = prev_node->prev_;
        delete prev_node;
    }

    void PopFront() {

        if (IsEmpty()) {
            throw ListIsEmptyException("ListIsEmpty");
        }

        Node* temp = this->head_;

        this->head_ = this->head_->next_;
        delete temp;

        this->head_->prev_ = nullptr;

        --sz_;
    }

    ~List() {
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
void swap(List<T>& a, List<T>& b) {
    a.Swap(b);
}
}  // namespace std
