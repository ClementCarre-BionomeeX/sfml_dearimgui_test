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

// TODO : check this implementation that can disconnect

// #include <functional>
// #include <mutex>
// #include <vector>
// #include <memory>

// template <typename... Args>
// class Signal {
// public:
//     using Slot = std::function<void(Args...)>;
//     using SlotPtr = std::shared_ptr<Slot>;

//     // Connect a slot to the signal
//     SlotPtr connect(Slot slot) {
//         std::lock_guard<std::mutex> lock(mutex);
//         auto slotPtr = std::make_shared<Slot>(std::move(slot));
//         slots.push_back(slotPtr);
//         return slotPtr;
//     }

//     // Disconnect a slot from the signal
//     void disconnect(SlotPtr slotPtr) {
//         std::lock_guard<std::mutex> lock(mutex);
//         slots.erase(std::remove(slots.begin(), slots.end(), slotPtr), slots.end());
//     }

//     // Emit the signal, calling all connected slots
//     void emit(Args... args) {
//         std::vector<SlotPtr> slotsCopy;
//         {
//             std::lock_guard<std::mutex> lock(mutex);
//             slotsCopy = slots;
//         }

//         for (auto& slotPtr : slotsCopy) {
//             if (slotPtr) {
//                 (*slotPtr)(std::forward<Args>(args)...);
//             }
//         }
//     }

// private:
//     std::vector<SlotPtr> slots;
//     mutable std::mutex mutex;
// };
