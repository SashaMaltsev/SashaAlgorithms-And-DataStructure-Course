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
        T data;
        Node* next;
        Node* prev;

    public:
        Node(const T& data) : data(data), next(nullptr), prev(nullptr) {
        }
        Node() : data(), next(nullptr), prev(nullptr) {
        }
    };

public:
    class ListIterator {
        friend class List;

    public:
        using value_type = T;
        using reference_type = value_type&;
        using pointer_type = value_type*;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

        inline bool operator==(const ListIterator& other) const {
            return this->current == other->current;
        };

        inline bool operator!=(const ListIterator& other) const {
            return this->current != other.current;
        };

        inline reference_type operator*() const {
            return this->current->data;
        };

        ListIterator& operator++() {
            this->current = this->current->next;
            return *this;
        };

        ListIterator operator++(int) {
            ListIterator copy = *this;
            this->current = this->current->next;
            return copy;
        };

        ListIterator& operator--() {
            this->current = this->current->prev;
            return *this;
        };

        ListIterator operator--(int) {
            ListIterator copy = *this;
            this->current = this->current->prev;
            return copy;
        };

        inline pointer_type operator->() const {
            return current->data;
        };

    private:
        ListIterator(Node* current) : current(current) {
        }

    private:
        Node* current;
    };

public:
    List() : sz(0) {
        this->head = new Node();
        this->last = this->head;
    }

    explicit List(size_t sz) {

        this->head = new Node();
        this->last = this->head;

        for (size_t i = 0; i < sz; ++i) {
            this->head->prev = new Node();
            this->head->prev->next = this->head;
            this->head = this->head->prev;
        }

        this->sz = sz;
    }

    List(const std::initializer_list<T>& values) : sz(0) {

        this->head = new Node();
        this->last = this->head;

        for (auto it = values.end(); it != values.begin();) {
            --it;
            this->head->prev = new Node(*it);
            this->head->prev->next = this->head;
            this->head = this->head->prev;
        }

        this->sz = values.size();
    }

    List(const List& other) : sz(0) {

        this->head = new Node();
        this->last = this->head;

        Node* t = other.head;

        while (t != other.last) {
            PushBack(t->data);
            t = t->next;
        }

        this->sz = other.sz;
    }

    List& operator=(const List& other) {
        List new_list(other);
        Swap(new_list);
        return *this;
    }

    ListIterator Begin() const noexcept {
        return ListIterator(this->head);
    }

    ListIterator End() const noexcept {
        return ListIterator(this->last);
    }

    inline T& Front() const {
        return this->head->data;
    }

    inline T& Back() const {
        return this->last->prev->data;
    }

    inline bool IsEmpty() const noexcept {
        return this->head == this->last;
    }

    inline size_t Size() const noexcept {
        return this->sz;
    }

    void Swap(List& l) {
        std::swap(this->head, l.head);
        std::swap(this->last, l.last);
        std::swap(this->sz, l.sz);
    }

    ListIterator Find(const T& value) const {

        ListIterator it(this->head);

        while (it != End()) {
            if (it.current->data == value) {
                return it;
            }
            ++it;
        }

        return it;
    }

    void Erase(ListIterator pos) {

        if (pos.current == this->head) {
            this->head = this->head->next;
            this->head->prev = nullptr;
            delete pos.current;
            --sz;
            return;
        }

        pos.current->prev->next = pos.current->next;
        pos.current->next->prev = pos.current->prev;
        delete pos.current;
        --sz;
    }

    void Insert(ListIterator pos, const T& value) {
        Node* nd = new Node(value);

        if (pos.current == this->head) {
            this->head->prev = nd;
            nd->next = this->head;
            this->head = nd;
            ++sz;
            return;
        }

        if (pos.current == this->last) {
            this->last->prev = nd;
            nd->next = this->last;
            ++sz;
            return;
        }

        pos.current->prev->next = nd;
        nd->prev = pos.current->prev;
        pos.current->prev = nd;
        nd->next = pos.current;

        ++sz;
    }

    void Clear() noexcept {
        while (this->sz) {
            PopFront();
        }
    }

    void PushBack(const T& value) {
        Node* next = new Node(value);

        if (this->sz == 0) {
            this->head = next;
            next->next = this->last;
            this->last->prev = this->head;
            ++sz;
            return;
        }

        next->prev = this->last->prev;
        next->next = this->last;
        this->last->prev->next = next;
        this->last->prev = next;

        ++sz;
    }

    void PushFront(const T& value) {
        Node* prev = new Node(value);
        this->head->prev = prev;
        this->head->prev->next = this->head;
        this->head = this->head->prev;
        ++sz;
    }

    void PopBack() {
        if (IsEmpty()) {
            throw ListIsEmptyException("ListIsEmpty");
        }

        --sz;

        Node* prevNode = this->last->prev;
        prevNode->prev->next = this->last;
        this->last->prev = prevNode->prev;
        delete prevNode;
    }

    void PopFront() {

        if (IsEmpty()) {
            throw ListIsEmptyException("ListIsEmpty");
        }

        Node* temp = this->head;

        this->head = this->head->next;
        delete temp;

        this->head->prev = nullptr;

        --sz;
    }

    ~List() {
        Clear();
        delete this->last;
    }

private:
    size_t sz;
    Node* head;
    Node* last;
};

namespace std {
// Global swap overloading
template <typename T>
void swap(List<T>& a, List<T>& b) {
    a.Swap(b);
}
}  // namespace std