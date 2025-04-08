#ifndef CORE_SINGLETON_HPP
#define CORE_SINGLETON_HPP

#include "allocation_strategy.hpp"
#include "sync_strategy.hpp"
#include "disposal_strategy.hpp"

namespace pattern {

/**
 * CoreSingleton - A template class implementing the Singleton pattern with 
 * orthogonal strategies for allocation, synchronization, and disposal
 * 
 * @tparam T The type of object to be managed as a singleton
 * @tparam AllocStrat Strategy for object allocation and deallocation
 * @tparam DisposeStrat Strategy for managing object lifecycle
 * @tparam SyncStrat Strategy for thread synchronization
 */
template 
<
    typename T,
    template <typename> class AllocStrat = NewAllocationStrategy,
    template <typename> class DisposeStrat = StandardDisposalStrategy,
    template <typename> class SyncStrat = MutexSynchronizationStrategy
>
class CoreSingleton {
public:
    // Delete constructors and assignment operators
    CoreSingleton() = delete;
    CoreSingleton(const CoreSingleton&) = delete;
    CoreSingleton& operator=(const CoreSingleton&) = delete;
    CoreSingleton(CoreSingleton&&) = delete;
    CoreSingleton& operator=(CoreSingleton&&) = delete;

    /**
     * Access the single instance of the class
     * @return Reference to the singleton instance
     */
    static T& GetInstance() {
        // Fast check without locking first
        if (!instance_ptr_) {
            // Lock only if instance doesn't exist
            typename SyncStrat<T>::SyncGuard sync_guard;
            
            // Double-check after lock acquisition
            if (!instance_ptr_) {
                // Handle already destroyed instance
                if (was_destroyed_) {
                    DisposeStrat<T>::HandleDeadInstance();
                    was_destroyed_ = false;
                }
                
                // Create new instance
                instance_ptr_ = AllocStrat<T>::Allocate();
                
                // Schedule destruction
                DisposeStrat<T>::RegisterForCleanup(&DestroyInstance);
            }
        }
        return *instance_ptr_;
    }

private:
    /**
     * Destroy the singleton instance
     */
    static void DestroyInstance() {
        typename SyncStrat<T>::SyncGuard sync_guard;
        AllocStrat<T>::Deallocate(instance_ptr_);
        instance_ptr_ = nullptr;
        was_destroyed_ = true;
    }

    // Static members
    inline static T* instance_ptr_ = nullptr;
    inline static bool was_destroyed_ = false;
};

} // namespace pattern

#endif // CORE_SINGLETON_HPP