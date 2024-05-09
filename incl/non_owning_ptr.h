#pragma once

template <typename T>
class non_owning_ptr {
  public:
    // Parameterized Constructor
    explicit non_owning_ptr(T* source = nullptr) : ptr(source) {}

    // Destructor - trivial since no ownership is assumed
    ~non_owning_ptr() = default;

    // Copy Constructor
    non_owning_ptr(const non_owning_ptr& other) : ptr(other.ptr) {}

    // Copy Assignment Operator
    non_owning_ptr& operator=(const non_owning_ptr& other) {
        if (this != &other) {
            ptr = other.ptr;
        }
        return *this;
    }

    // Move Constructor
    non_owning_ptr(non_owning_ptr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }

    // Move Assignment Operator
    non_owning_ptr& operator=(non_owning_ptr&& other) noexcept {
        if (this != &other) {
            ptr       = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // Access operators
    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }

    // Equality Operators
    bool operator==(const non_owning_ptr& other) const { return ptr == other.ptr; }
    bool operator!=(const non_owning_ptr& other) const { return ptr != other.ptr; }

    // Bool Conversion Operator
    explicit operator bool() const { return ptr != nullptr; }

    // Explicit conversion to T*
    explicit operator T*() const { return ptr; }

  private:
    T* ptr;
};
