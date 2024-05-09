#pragma once
#include <memory>

template <typename T>
class non_owning_ptr {
  public:
    // Parameterized Constructor
    explicit non_owning_ptr(T* source = nullptr) : ptr(source) {}

    // Constructor from std::unique_ptr<T> without transferring ownership
    explicit non_owning_ptr(std::unique_ptr<T> const& source)
        : ptr(source.get()) {}    // Get the raw pointer without affecting ownership

    // Destructor - trivial since no ownership is assumed
    ~non_owning_ptr() = default;

    // Template Copy Constructor for converting from derived to base
    template <typename U>
    non_owning_ptr(const non_owning_ptr<U>& other) : ptr(static_cast<T*>(other.get())) {}

    // Template Copy Assignment for converting from derived to base
    template <typename U>
    non_owning_ptr& operator=(const non_owning_ptr<U>& other) {
        ptr = static_cast<T*>(other.get());
        return *this;
    }
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

    // TODO : temporary while not everyone is non_owning.
    bool operator==(T* other) const { return ptr == other; }
    bool operator!=(T* other) const { return ptr != other; }

    // Bool Conversion Operator
    explicit operator bool() const { return ptr != nullptr; }

    // Explicit conversion to T*
    explicit operator T*() const { return ptr; }

    void clear() noexcept { ptr = nullptr; }

    // Getter for the pointer (useful in template conversions)
    T* get() const { return ptr; }

  private:
    T* ptr;
};
