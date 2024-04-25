#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

#include "exceptions.hpp"

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {

    class Node;

public:
    class MapIterator {
        friend class Map;

    public:
        // NOLINTNEXTLINE
        using value_type = std::pair<const Key, Value>;
        // NOLINTNEXTLINE
        using reference_type = value_type&;
        // NOLINTNEXTLINE
        using pointer_type = value_type*;
        // NOLINTNEXTLINE
        using difference_type = std::ptrdiff_t;
        // NOLINTNEXTLINE
        using iterator_category = std::forward_iterator_tag;

        inline bool operator==(const MapIterator& other) const {
            return this->current_ == other.current_;
        };

        inline bool operator!=(const MapIterator& other) const {
            return this->current_ != other.current_;
        };

        inline reference_type operator*() const {
            return this->current_->data_;
        };

        MapIterator& operator++() {

            if (this->current_->is_right_link_) {
                this->current_ = this->current_->right_;
            } else {
                this->current_ = this->current_->right_;

                while (this->current_->left_ != nullptr) {
                    this->current_ = this->current_->left_;
                }
            }

            return *this;
        };

        MapIterator operator++(int) {
            MapIterator copy = *this;
            operator++();
            return copy;
        };

        inline pointer_type operator->() const {
            return &current_->data_;
        };

    private:
        explicit MapIterator(Node* current) : current_(current) {
        }

    private:
        Node* current_;
    };

    inline MapIterator Begin() const noexcept {
        if (IsEmpty()) {
            return MapIterator(this->fake_node_);
        }

        Node* temp = this->root_;

        while (temp->left_ != nullptr) {
            temp = temp->left_;
        }

        return MapIterator(temp);
    }

    inline MapIterator End() const noexcept {
        return MapIterator(this->fake_node_);
    }

    Map() : root_(nullptr), sz_(0) {
        this->fake_node_ = new Node();
    }

    Value& operator[](const Key& key) {
        if (IsEmpty()) {
            this->root_ = new Node(key);
            this->fake_node_->left_ = this->root_;
            this->root_->is_right_link_ = true;
            this->root_->right_ = this->fake_node_;
            ++sz_;
            return this->root_->data_.second;
        }

        Node* temp = this->root_;

        while (true) {
            if (comp_(key, temp->data_.first)) {
                if (temp->left_ == nullptr) {
                    temp->left_ = new Node(key);
                    temp->left_->is_right_link_ = true;
                    temp->left_->right_ = temp;
                    ++sz_;
                    return temp->left_->data_.second;
                }
                temp = temp->left_;

            } else if (key == temp->data_.first) {
                return temp->data_.second;
            } else {
                if (temp->is_right_link_) {
                    Node* ptr = new Node(key);
                    temp->is_right_link_ = false;
                    ptr->is_right_link_ = true;
                    ptr->right_ = temp->right_;
                    temp->right_ = ptr;
                    ++sz_;
                    return ptr->data_.second;
                }
                temp = temp->right_;
            }
        }
    }

    inline bool IsEmpty() const noexcept {
        return this->sz_ == 0;
    }

    inline size_t Size() const noexcept {
        return this->sz_;
    }

    void Swap(Map& a) {
        static_assert(std::is_same<decltype(this->comp_), decltype(a.comp_)>::value,
                      "The compare function types are different");

        std::swap(this->comp_, a.comp_);
        std::swap(this->root_, a.root_);
        std::swap(this->sz_, a.sz_);
        std::swap(this->fake_node_, a.fake_node_);
    }

    std::vector<std::pair<const Key, Value>> Values(bool is_increase = true) const noexcept {
        std::vector<std::pair<const Key, Value>> res;

        if (IsEmpty()) {
            return res;
        }

        if (is_increase) {
            LKP(this->root_, res);
        } else {
            PKL(this->root_, res);
        }
        return res;
    }

    void Insert(const std::pair<const Key, Value>& val) {
        if (IsEmpty()) {
            this->root_ = new Node(val.first, val.second);
            this->fake_node_->left_ = this->root_;
            this->root_->is_right_link_ = true;
            this->root_->right_ = this->fake_node_;
            ++sz_;
            return;
        }

        Node* temp = this->root_;

        while (true) {
            if (comp_(val.first, temp->data_.first)) {
                if (temp->left_ == nullptr) {
                    temp->left_ = new Node(val.first, val.second);
                    temp->left_->is_right_link_ = true;
                    temp->left_->right_ = temp;
                    ++sz_;
                    return;
                }
                temp = temp->left_;

            } else if (!comp_(val.first, temp->data_.first) and !comp_(temp->data_.first, val.first)) {
                temp->data_.second = val.second;
                return;
            } else {
                if (temp->is_right_link_) {

                    Node* ptr = new Node(val.first, val.second);
                    temp->is_right_link_ = false;
                    ptr->is_right_link_ = true;
                    ptr->right_ = temp->right_;
                    temp->right_ = ptr;

                    ++sz_;
                    return;
                }
                temp = temp->right_;
            }
        }
    }

    void Insert(const std::initializer_list<std::pair<const Key, Value>>& values) {
        for (auto it = values.begin(); it != values.end(); ++it) {
            Insert(*it);
        }
    }

    void Erase(const Key& key) {
        if (IsEmpty()) {
            throw TreeIsEmptyException("TreeIsEmpty");
        }

        Node* prev = this->root_;
        Node* curr = this->root_;

        while (curr->data_.first != key) {

            if (comp_(key, curr->data_.first)) {
                if (curr->left_ == nullptr) {
                    throw TreeIsEmptyException("TreeIsEmpty");
                }
                prev = curr;
                curr = curr->left_;

            } else {
                if (curr->is_right_link_) {
                    throw TreeIsEmptyException("TreeIsEmpty");
                }
                prev = curr;
                curr = curr->right_;
            }
        }

        --sz_;

        if ((curr->is_right_link_) and (curr->left_ == nullptr)) {

            if (prev->left_ == curr) {
                prev->left_ = nullptr;
            } else {
                prev->is_right_link_ = true;
                prev->right_ = curr->right_;
            }

            if (curr == this->root_) {
                this->fake_node_->left_ = nullptr;
                this->root_ = nullptr;
            }
            delete curr;
            return;

        } else if ((curr->is_right_link_) and (curr->left_ != nullptr)) {

            Node* temp = curr->left_;

            while (!temp->is_right_link_) {
                temp = temp->right_;
            }
            temp->right_ = this->fake_node_;

            if (curr == this->root_) {
                this->fake_node_->left_ = curr->left_;
                this->root_ = curr->left_;
            }

            if (prev->left_ == curr) {
                prev->left_ = curr->left_;
            } else {
                prev->right_ = curr->left_;
            }

            delete curr;
            return;

        } else if ((!curr->is_right_link_) and (curr->left_ != nullptr)) {

            Node* temp = curr->right_;
            Node* t_prev = curr;

            while (temp->left_ != nullptr) {
                t_prev = temp;
                temp = temp->left_;
            }

            Node* it = curr->left_;

            if ((temp->is_right_link_) and (temp->left_ == nullptr)) {
                temp->left_ = curr->left_;
                temp->right_ = (t_prev == curr) ? this->fake_node_ : curr->right_;
                temp->is_right_link_ = (t_prev == curr);
                t_prev->left_ = nullptr;
            } else if (!(temp->is_right_link_) and (temp->left_ == nullptr)) {
                temp->left_ = curr->left_;
                if (t_prev != curr) {
                    t_prev->left_ = temp->right_;
                    temp->right_ = curr->right_;
                }
            }

            while (!it->is_right_link_) {
                it = it->right_;
            }
            it->right_ = temp;

            if (curr == this->root_) {
                this->root_ = temp;
                this->fake_node_->left_ = temp;
            }

            if (prev->left_ == curr) {
                prev->left_ = curr;
            } else {
                prev->right_ = temp;
            }

            delete curr;
            return;

        } else if ((!curr->is_right_link_) and (curr->left_ == nullptr)) {

            if (curr == this->root_) {
                this->root_ = curr->right_;
                this->fake_node_->left_ = curr->right_;
            }

            if (prev->left_ == curr) {
                prev->left_ = curr->right_;
            } else {
                prev->is_right_link_ = curr->is_right_link_;
                prev->right_ = curr->right_;
            }

            delete curr;
            return;
        }
    }

    void Clear() noexcept {

        if (IsEmpty()) {
            return;
        }

        Del();
    }

    MapIterator Find(const Key& key) const {

        if (IsEmpty()) {
            return MapIterator(this->fake_node_);
        }

        Node* temp = this->root_;

        while (true) {

            if (temp->data_.first == key) {
                return MapIterator(temp);
            }

            if (comp_(temp->data_.first, temp->data_.first)) {
                if (temp->left_ == nullptr) {
                    return MapIterator(this->fake_node_);
                }
                temp = temp->left_;
            } else {

                if (temp->is_right_link_) {
                    return MapIterator(this->fake_node_);
                }
                temp = temp->right_;
            }
        }
    }

    ~Map() {
        Clear();
        delete fake_node_;
    }

private:
    class Node {
        friend class MapIterator;
        friend class Map;

    private:
        bool is_right_link_;
        Node* left_;
        Node* right_;
        std::pair<const Key, Value> data_;

    public:
        explicit Node(const Key& key, const Value& value)
            : is_right_link_(false), left_(nullptr), right_(nullptr), data_(std::make_pair(key, value)) {
        }
        explicit Node(const Key& key)
            : is_right_link_(false), left_(nullptr), right_(nullptr), data_(std::make_pair(key, Value{})) {
        }
        explicit Node() : is_right_link_(true), left_(nullptr), right_(this), data_() {
        }
    };

    void LKP(Node* nd, std::vector<std::pair<const Key, Value>>& res) const {

        if (nd->left_ != nullptr) {
            LKP(nd->left_, res);
        }

        res.push_back(std::make_pair(nd->data_.first, nd->data_.second));

        if (!nd->is_right_link_) {
            LKP(nd->right_, res);
        }
    }

    void PKL(Node* nd, std::vector<std::pair<const Key, Value>>& res) const {

        if (!nd->is_right_link_) {
            PKL(nd->right_, res);
        }

        res.push_back(std::make_pair(nd->data_.first, nd->data_.second));

        if (nd->left_ != nullptr) {
            PKL(nd->left_, res);
        }
    }

    void Del() {

        auto curr = Begin();
        auto prev = Begin();

        while (curr != End()) {
            prev = curr;
            ++curr;
            --sz_;
            delete prev.current_;
        }
    }

private:
    Compare comp_;
    Node* root_;
    size_t sz_;
    Node* fake_node_;
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std
