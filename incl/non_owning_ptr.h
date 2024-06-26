#pragma once
#include <memory>
#include <mutex>

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
    non_owning_ptr(const non_owning_ptr<U>& other) {
        std::lock_guard<std::mutex> lock(other.mtx);
        ptr = static_cast<T*>(other.get());
    }

    // Template Copy Assignment for converting from derived to base
    template <typename U>
    non_owning_ptr& operator=(const non_owning_ptr<U>& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(other.mtx);
            std::lock_guard<std::mutex> lock_this(mtx);
            ptr = static_cast<T*>(other.get());
        }
        return *this;
    }

    // Copy Constructor
    non_owning_ptr(const non_owning_ptr& other) {
        std::lock_guard<std::mutex> lock(other.mtx);
        ptr = other.ptr;
    }

    // Copy Assignment Operator
    non_owning_ptr& operator=(const non_owning_ptr& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(other.mtx);
            std::lock_guard<std::mutex> lock_this(mtx);
            ptr = other.ptr;
        }
        return *this;
    }

    // Move Constructor
    non_owning_ptr(non_owning_ptr&& other) noexcept {
        std::lock_guard<std::mutex> lock(other.mtx);
        ptr       = other.ptr;
        other.ptr = nullptr;
    }

    // Move Assignment Operator
    non_owning_ptr& operator=(non_owning_ptr&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(other.mtx);
            std::lock_guard<std::mutex> lock_this(mtx);
            ptr       = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // Access operators
    T* operator->() const {
        std::lock_guard<std::mutex> lock(mtx);
        return ptr;
    }
    T& operator*() const {
        std::lock_guard<std::mutex> lock(mtx);
        return *ptr;
    }

    // Equality Operators
    bool operator==(const non_owning_ptr& other) const {
        std::lock_guard<std::mutex> lock_this(mtx);
        std::lock_guard<std::mutex> lock_other(other.mtx);
        return ptr == other.ptr;
    }
    bool operator!=(const non_owning_ptr& other) const {
        std::lock_guard<std::mutex> lock_this(mtx);
        std::lock_guard<std::mutex> lock_other(other.mtx);
        return ptr != other.ptr;
    }

    // Bool Conversion Operator
    explicit operator bool() const {
        std::lock_guard<std::mutex> lock(mtx);
        return ptr != nullptr;
    }

    // Explicit conversion to T*
    explicit operator T*() const {
        std::lock_guard<std::mutex> lock(mtx);
        return ptr;
    }

    void clear() noexcept {
        std::lock_guard<std::mutex> lock(mtx);
        ptr = nullptr;
    }

    // Getter for the pointer (useful in template conversions)
    T* get() const {
        std::lock_guard<std::mutex> lock(mtx);
        return ptr;
    }

    // Safe dynamic casting to another non_owning_ptr type
    template <typename U>
    static non_owning_ptr<U> dynamic_cast_to(non_owning_ptr<T> const& source) {
        std::lock_guard<std::mutex> lock(source.mtx);
        if (U* casted = dynamic_cast<U*>(source.get())) {
            return non_owning_ptr<U>(casted);
        }
        return non_owning_ptr<U>(nullptr);
    }

    template <typename U>
    static non_owning_ptr<U> dynamic_cast_to(std::unique_ptr<T> const& source) {
        if (U* casted = dynamic_cast<U*>(source.get())) {
            return non_owning_ptr<U>(casted);
        }
        return non_owning_ptr<U>(nullptr);
    }

    template <typename U>
    static non_owning_ptr<U> dynamic_cast_to(std::shared_ptr<T> const& source) {
        if (U* casted = dynamic_cast<U*>(source.get())) {
            return non_owning_ptr<U>(casted);
        }
        return non_owning_ptr<U>(nullptr);
    }

  private:
    mutable std::mutex mtx;
    T*                 ptr;
};
