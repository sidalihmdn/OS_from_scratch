#ifndef KERNEL_CPP_TYPES_HPP
#define KERNEL_CPP_TYPES_HPP

#include <unit_types.h>

namespace kernel {

// Size type
using size_t = uint32_t;

// nullptr_t type
using nullptr_t = decltype(nullptr);

// Move semantics helper
template<typename T>
struct remove_reference { using type = T; };

template<typename T>
struct remove_reference<T&> { using type = T; };

template<typename T>
struct remove_reference<T&&> { using type = T; };

template<typename T>
typename remove_reference<T>::type&& move(T&& arg) {
    return static_cast<typename remove_reference<T>::type&&>(arg);
}

// Forward helper
template<typename T>
T&& forward(typename remove_reference<T>::type& arg) {
    return static_cast<T&&>(arg);
}

template<typename T>
T&& forward(typename remove_reference<T>::type&& arg) {
    return static_cast<T&&>(arg);
}

// Swap
template<typename T>
void swap(T& a, T& b) {
    T temp = kernel::move(a);
    a = kernel::move(b);
    b = kernel::move(temp);
}

} // namespace kernel

#endif