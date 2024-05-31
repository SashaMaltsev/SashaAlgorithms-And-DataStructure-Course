#include "Deque/deque.hpp"

template<typename T, typename Container = Deque<T>>
class Stack : protected Container {
public:
    Stack() = default;

    void Pop();
    inline T& Top() const noexcept;

    void Push(const T& value);
    void Push(T&& value);

    template <class... Args>
    void Emplace(Args&& ... args);

    using Container::IsEmpty;
    using Container::Size;
    using Container::Swap;

    ~Stack() = default;
};


template <typename T, typename Container>
inline T &Stack<T, Container>::Top() const noexcept {
    return Container::Back();
}

template <typename T, typename Container>
inline void Stack<T, Container>::Push(const T& value) {
    Emplace(value);
}

template <typename T, typename Container>
inline void Stack<T, Container>::Push(T&& value) {
    Emplace(std::move(value));
}

template <typename T, typename Container>
template <class... Args>
inline void Stack<T, Container>::Emplace(Args&& ...args) {
    Container::EmplaceBack(args...);
}

template <typename T, typename Container>
inline void Stack<T, Container>::Pop() {
    Container::PopBack();
}
