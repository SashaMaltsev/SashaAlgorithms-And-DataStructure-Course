#include "../deque/deque.hpp"

template <typename T, typename Container = Deque<T>>
class Queue : protected Container {
public:
    Queue() = default;

    void Pop();

    void Push(const T& value);
    void Push(T&& value);

    template <class... Args>
    void Emplace(Args&&... args);

    using Container::Front;
    using Container::IsEmpty;
    using Container::Size;
    using Container::Swap;

    ~Queue() = default;
};

template <typename T, typename Container>
inline void Queue<T, Container>::Push(const T& value) {
    Emplace(value);
}

template <typename T, typename Container>
inline void Queue<T, Container>::Push(T&& value) {
    Emplace(std::move(value));
}

template <typename T, typename Container>
template <class... Args>
inline void Queue<T, Container>::Emplace(Args&&... args) {
    Container::EmplaceBack(args...);
}

template <typename T, typename Container>
inline void Queue<T, Container>::Pop() {
    Container::PopFront();
}
