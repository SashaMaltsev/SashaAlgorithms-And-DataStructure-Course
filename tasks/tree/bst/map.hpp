#pragma once

#include <fmt/core.h>

#include <cstdlib>
#include <functional>
#include <utility>
#include <vector>

#include "exceptions.hpp"

template <typename Key, typename Value, typename Compare = std::less<Key>>
class Map {
private:
    class Node {
        friend class Map;

    private:
        Node* left_;
        Node* right_;
        Key key_;
        Value value_;

    public:
        explicit Node(const Key& key, const Value& value) : left_(nullptr), right_(nullptr), key_(key), value_(value) {
        }

        explicit Node(const Key& key) : left_(nullptr), right_(nullptr), key_(key), value_() {
        }
    };

public:
    Map() : root_(nullptr), sz_(0) {
    }

    Value& operator[](const Key& key) {

        if (IsEmpty()) {
            this->root_ = new Node(key);
            ++sz_;
            return this->root_->value_;
        }

        Node* temp = this->root_;

        while (true) {
            if (comp_(key, temp->key_)) {
                if (temp->left_ == nullptr) {
                    Node* new_elem = new Node(key);
                    temp->left_ = new_elem;
                    ++sz_;
                    return new_elem->value_;
                }
                temp = temp->left_;

            } else if (key == temp->key_) {
                return temp->value_;
            } else {
                if (temp->right_ == nullptr) {
                    Node* new_elem = new Node(key);
                    temp->right_ = new_elem;
                    ++sz_;
                    return new_elem->value_;
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

    void LKP(Node* nd, std::vector<std::pair<const Key, Value>>& res) const {

        if (nd->left_ != nullptr) {
            LKP(nd->left_, res);
        }

        res.push_back(std::make_pair(nd->key_, nd->value_));

        if (nd->right_ != nullptr) {
            LKP(nd->right_, res);
        }
    }

    void PKL(Node* nd, std::vector<std::pair<const Key, Value>>& res) const {

        if (nd->right_ != nullptr) {
            PKL(nd->right_, res);
        }

        res.push_back(std::make_pair(nd->key_, nd->value_));

        if (nd->left_ != nullptr) {
            PKL(nd->left_, res);
        }
    }

    void Insert(const std::pair<const Key, Value>& val) {

        if (IsEmpty()) {
            this->root_ = new Node(val.first, val.second);
            ++sz_;
            return;
        }

        Node* temp = this->root_;

        while (true) {
            if (comp_(val.first, temp->key_)) {
                if (temp->left_ == nullptr) {
                    temp->left_ = new Node(val.first, val.second);
                    ++sz_;
                    return;
                }
                temp = temp->left_;

            } else if (val.first == temp->key_) {
                temp->value_ = val.second;
                return;
            } else {
                if (temp->right_ == nullptr) {
                    temp->right_ = new Node(val.first, val.second);
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
            throw TreeIsEmptyException("ListIsEmpty");
        }

        Node* temp = this->root_;
        Node* prev = this->root_;

        while (true) {
            if (temp->key_ == key) {
                break;
            }

            if (comp_(key, temp->key_)) {
                if (temp->left_ == nullptr) {
                    throw TreeIsEmptyException("ListIsEmpty");
                }
                prev = temp;
                temp = temp->left_;

            } else {
                if (temp->right_ == nullptr) {
                    throw TreeIsEmptyException("ListIsEmpty");
                }
                prev = temp;
                temp = temp->right_;
            }
        }

        --sz_;

        if ((temp->right_ == nullptr) or (temp->left_ == nullptr)) {

            auto t = (temp->right_ != nullptr) ? (temp->right_) : (temp->left_);

            if (prev == this->root_ and temp == this->root_) {
                this->root_ = t;
                delete prev;
                return;
            }
            if (prev->left_ == temp) {
                prev->left_ = t;
            } else {
                prev->right_ = t;
            }
            delete temp;
            return;
        }

        Node* t_prev = temp->right_;
        Node* t = temp->right_;

        while (t->left_ != nullptr) {
            t_prev = t;
            t = t->left_;
        }

        if (t == t_prev) {
            if (temp == this->root_) {
                this->root_ = t;
                t->left_ = temp->left_;
                if (temp->right_ != t) {
                    t->right_ = temp->right_;
                }
                delete temp;
                return;
            }
            if (prev->left_ == temp) {
                prev->left_ = t;
            } else {
                prev->right_ = t;
            }
            t->left_ = temp->left_;
            delete temp;
            return;
        }

        t_prev->left_ = nullptr;
        t->left_ = temp->left_;
        t->right_ = temp->right_;

        if (temp == this->root_) {
            this->root_ = t;
            delete temp;
            return;
        }

        if (prev->left_ == temp) {
            prev->left_ = t;
        } else {
            prev->right_ = t;
        }

        delete temp;
    }

    void Clear() {

        if (IsEmpty()) {
            return;
        }

        Del(this->root_, nullptr);
    }

    void Del(Node* nd, Node* prev) {

        if (IsEmpty()) {
            return;
        }

        if (nd->left_ != nullptr) {
            Del(nd->left_, nd);
        }

        if (nd->right_ != nullptr) {
            Del(nd->right_, nd);
        }

        if (prev != nullptr) {
            if (prev->left_ == nd) {
                prev->left_ = nullptr;
            } else {
                prev->right_ = nullptr;
            }
        }

        delete nd;
        --sz_;
    }

    bool Find(const Key& key) const {

        if (IsEmpty()) {
            return false;
        }

        Node* temp = this->root_;

        while (true) {

            if (temp->key_ == key) {
                return true;
            }

            if (comp_(temp->key_, temp->key_)) {
                if (temp->left_ == nullptr) {
                    return false;
                }
                temp = temp->left_;
            } else {
                if (temp->right_ == nullptr) {
                    return false;
                }
                temp = temp->right_;
            }
        }
    }

    ~Map() {
        Clear();
    }

private:
    Compare comp_;
    Node* root_;
    size_t sz_;
};

namespace std {
// Global swap overloading
template <typename Key, typename Value>
// NOLINTNEXTLINE
void swap(Map<Key, Value>& a, Map<Key, Value>& b) {
    a.Swap(b);
}
}  // namespace std
