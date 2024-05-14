#pragma once
#include <functional>
#include <mutex>
#include <vector>

template <typename... Args>
class Signal {
  public:
    using Slot = std::function<void(Args...)>;

    // Connect a slot to the signal
    void connect(Slot slot) {
        std::lock_guard<std::mutex> lock(mutex);    // Lock the mutex for thread safety
        slots.push_back(std::move(slot));
    }

    // Emit the signal, calling all connected slots
    void emit(Args... args) {
        std::vector<Slot> slotsCopy;
        {
            std::lock_guard<std::mutex> lock(mutex);    // Lock the mutex to copy the slots
            slotsCopy = slots;                          // Copy the slots to call outside the lock
        }

        // Call all slots without holding the mutex to avoid deadlocks and allow reentrancy
        for (auto& slot : slotsCopy) {
            slot(std::forward<Args>(args)...);
        }
    }

    // Disconnect all slots
    void disconnect_all() {
        std::lock_guard<std::mutex> lock(mutex);    // Lock the mutex for thread safety
        slots.clear();                              // Clear all slots
    }

  private:
    std::vector<Slot>  slots;
    mutable std::mutex mutex;    // Mutex to protect the slots container
};
