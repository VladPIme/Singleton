#ifndef ALLOCATION_STRATEGY_HPP
#define ALLOCATION_STRATEGY_HPP

#include <memory>
#include <cstdlib>
#include <new>

namespace pattern {

/**
 * Strategy for allocating and deallocating objects using new/delete
 * 
 * This is the most common allocation strategy using standard C++ 
 * heap allocation mechanisms.
 */
template <typename T>
struct NewAllocationStrategy {
    /**
     * Allocate an instance of T using the new operator
     * @return Pointer to the newly allocated object
     */
    static T* Allocate() {
        return new T();
    }
    
    /**
     * Deallocate an instance of T using the delete operator
     * @param instance Pointer to the object to be deallocated
     */
    static void Deallocate(T* instance) {
        delete instance;
    }
};

/**
 * Strategy for allocating and deallocating objects using malloc/free
 * 
 * This strategy uses C-style memory allocation with placement new
 * for object construction.
 */
template <typename T>
struct RawMemoryAllocationStrategy {
    /**
     * Allocate an instance of T using malloc and placement new
     * @return Pointer to the newly allocated object
     */
    static T* Allocate() {
        // Allocate raw memory
        void* raw_memory = std::malloc(sizeof(T));
        
        // Check allocation success
        if (!raw_memory) {
            throw std::bad_alloc();
        }
        
        // Construct object in-place
        return new(raw_memory) T();
    }
    
    /**
     * Deallocate an instance of T using destructor call and free
     * @param instance Pointer to the object to be deallocated
     */
    static void Deallocate(T* instance) {
        if (instance) {
            // Call destructor manually
            instance->~T();
            
            // Free the raw memory
            std::free(instance);
        }
    }
};

/**
 * Strategy for allocating and deallocating objects using smart pointers
 * 
 * This strategy leverages std::shared_ptr for automatic memory management.
 */
template <typename T>
struct SmartPointerAllocationStrategy {
    /**
     * Allocate an instance of T using std::make_shared
     * @return Pointer to the newly allocated object
     */
    static T* Allocate() {
        std::shared_ptr<T>& smart_ptr = GetSharedPointer();
        smart_ptr = std::make_shared<T>();
        return smart_ptr.get();
    }
    
    /**
     * Deallocate an instance of T by resetting the shared pointer
     * @param instance Pointer to the object to be deallocated
     */
    static void Deallocate(T* /*instance*/) {
        GetSharedPointer().reset();
    }

private:
    /**
     * Access the static shared pointer instance
     * @return Reference to the static shared pointer
     */
    static std::shared_ptr<T>& GetSharedPointer() {
        static std::shared_ptr<T> ptr_instance;
        return ptr_instance;
    }
};

} // namespace pattern

#endif // ALLOCATION_STRATEGY_HPP