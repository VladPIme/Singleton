#ifndef SYNC_STRATEGY_HPP
#define SYNC_STRATEGY_HPP

#include <mutex>
#include <atomic>
#include <thread>

namespace pattern {

/**
 * No synchronization strategy for single-threaded environments
 * 
 * This strategy doesn't provide any synchronization mechanism and
 * is suitable only for single-threaded applications.
 */
template <typename T>
class NoSynchronizationStrategy {
public:
    /**
     * A guard class that does nothing in single-threaded context
     */
    class SyncGuard {
    public:
        SyncGuard() {}
        ~SyncGuard() {}
    };
};

/**
 * Mutex-based synchronization strategy for multi-threaded environments
 * 
 * This strategy uses a standard mutex to ensure thread-safe singleton
 * access and initialization.
 */
template <typename T>
class MutexSynchronizationStrategy {
public:
    /**
     * A guard class that locks/unlocks the mutex on construction/destruction
     */
    class SyncGuard {
    public:
        SyncGuard() { mutex_.lock(); }
        ~SyncGuard() { mutex_.unlock(); }
    
    private:
        static std::mutex mutex_;
    };
};

// Static mutex initialization
template <typename T>
std::mutex MutexSynchronizationStrategy<T>::SyncGuard::mutex_;

/**
 * Atomic flag synchronization strategy for multi-threaded environments
 * 
 * This strategy uses atomic operations for potentially better performance
 * in certain scenarios compared to mutex-based locking.
 */
template <typename T>
class AtomicSynchronizationStrategy {
public:
    /**
     * A guard class that uses atomic flag for synchronization
     */
    class SyncGuard {
    public:
        SyncGuard() {
            // Spin until we can acquire the lock
            while (lock_flag_.test_and_set(std::memory_order_acquire)) {
                // Optional: yield to avoid excessive CPU usage in contended scenarios
                std::this_thread::yield();
            }
        }
        
        ~SyncGuard() {
            // Release the lock
            lock_flag_.clear(std::memory_order_release);
        }
    
    private:
        static std::atomic_flag lock_flag_;
    };
};

// Static atomic flag initialization
template <typename T>
std::atomic_flag AtomicSynchronizationStrategy<T>::SyncGuard::lock_flag_ = ATOMIC_FLAG_INIT;

/**
 * Thread-local storage based strategy for providing thread-specific instances
 * 
 * This strategy gives each thread its own independent singleton instance.
 */
template <typename T>
class ThreadLocalStrategy {
public:
    /**
     * A guard class that does nothing as each thread has its own instance
     */
    class SyncGuard {
    public:
        SyncGuard() {}
        ~SyncGuard() {}
    };
    
    /**
     * Get thread-local instance
     */
    static T& GetThreadLocalInstance() {
        static thread_local T* local_instance = new T();
        return *local_instance;
    }
};

} // namespace pattern

#endif // SYNC_STRATEGY_HPP