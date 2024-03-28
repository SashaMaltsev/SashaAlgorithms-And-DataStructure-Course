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
        using ValueType = T;
        using ReferenceType = value_type&;
        using PointerType = value_type*;
        using DifferenceType = std::ptrdiff_t;
        using IteratorCategory = std::bidirectional_iterator_tag;

        inline bool operator==(const ListIterator& other) const {
            return this->current_ == other->current;
        };

        inline bool operator!=(const ListIterator& other) const {
            return this->current_ != other.current_;
        };

        inline reference_type operator*() const {
            return this->current_->data;
        };

        ListIterator& operator++() {
            this->current_ = this->current_->next;
            return *this;
        };

        ListIterator operator++(int) {
            ListIterator copy = *this;
            this->current_ = this->current_->next;
            return copy;
        };

        ListIterator& operator--() {
            this->current_ = this->current_->prev;
            return *this;
        };

        ListIterator operator--(int) {
            ListIterator copy = *this;
            this->current_ = this->current_->prev;
            return copy;
        };

        inline pointer_type operator->() const {
            return current_->data_;
        };

    private:
        explicit ListIterator(Node* current) : current_(current) {
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
            this->head_->prev = new Node();
            this->head_->prev->next = this->head_;
            this->head_ = this->head_->prev;
        }

        this->sz_ = sz;
    }

    List(const std::initializer_list<T>& values) : sz_(0) {

        this->head_ = new Node();
        this->last_ = this->head_;

        for (auto it = values.end(); it != values.begin();) {
            --it;
            this->head_->prev = new Node(*it);
            this->head_->prev->next = this->head_;
            this->head_ = this->head_->prev;
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
        return this->head_->data;
    }

    inline T& Back() const {
        return this->last_->prev->data;
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
            if (it.current_->data == value) {
                return it;
            }
            ++it;
        }

        return it;
    }

    void Erase(ListIterator pos) {

        if (pos.current_ == this->head_) {
            this->head_ = this->head_->next;
            this->head_->prev = nullptr;
            delete pos.current_;
            --sz_;
            return;
        }

        pos.current_->prev->next = pos.current_->next;
        pos.current_->next->prev = pos.current_->prev;
        delete pos.current_;
        --sz_;
    }

    void Insert(ListIterator pos, const T& value) {
        Node* nd = new Node(value);

        if (pos.current_ == this->head_) {
            this->head_->prev = nd;
            nd->next_ = this->head_;
            this->head_ = nd;
            ++sz_;
            return;
        }

        if (pos.current_ == this->last_) {
            this->last_->prev = nd;
            nd->next_ = this->last_;
            ++sz_;
            return;
        }

        pos.current_->prev->next = nd;
        nd->prev_ = pos.current_->prev;
        pos.current_->prev = nd;
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
            this->last_->prev = this->head_;
            ++sz_;
            return;
        }

        next->prev_ = this->last_->prev;
        next->next_ = this->last_;
        this->last_->prev->next = next;
        this->last_->prev = next;

        ++sz_;
    }

    void PushFront(const T& value) {
        Node* prev = new Node(value);
        this->head_->prev = prev;
        this->head_->prev->next = this->head_;
        this->head_ = this->head_->prev;
        ++sz_;
    }

    void PopBack() {
        if (IsEmpty()) {
            throw ListIsEmptyException("ListIsEmpty");
        }

        --sz_;

        Node* prev_node = this->last_->prev;
        prev_node->prev_->next = this->last_;
        this->last_->prev = prev_node->prev_;
        delete prev_node;
    }

    void PopFront() {

        if (IsEmpty()) {
            throw ListIsEmptyException("ListIsEmpty");
        }

        Node* temp = this->head_;

        this->head_ = this->head_->next;
        delete temp;

        this->head_->prev = nullptr;

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
void Swap(List<T>& a, List<T>& b) {
    a.Swap(b);
}
}  // namespace std
