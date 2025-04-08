#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include <iostream>
#include <thread>
#include <vector>
#include "../include/core_singleton.hpp"


class InstrumentedObject {
public:
    InstrumentedObject() : instance_id_(instance_count_++) {
        std::cout << "InstrumentedObject " << instance_id_ << " created" << std::endl;
    }
    
    ~InstrumentedObject() { 
        std::cout << "InstrumentedObject " << instance_id_ << " destroyed" << std::endl;
        instance_count_--; 
    }
    
    int getInstanceId() const { return instance_id_; }
    static int getInstanceCount() { return instance_count_; }
    
private:
    int instance_id_;
    static int instance_count_;
};

// Initialize static counter
int InstrumentedObject::instance_count_ = 0;

// Define test singleton types using different strategies

// No synchronization singleton (single-threaded)
using SingleThreadSingleton = pattern::CoreSingleton<
    InstrumentedObject,
    pattern::NewAllocationStrategy,
    pattern::StandardDisposalStrategy,
    pattern::NoSynchronizationStrategy
>;

// Mutex-synchronized singleton
using ThreadSafeSingleton = pattern::CoreSingleton<
    InstrumentedObject,
    pattern::NewAllocationStrategy,
    pattern::StandardDisposalStrategy,
    pattern::MutexSynchronizationStrategy
>;

// Atomic-synchronized singleton
using AtomicSingleton = pattern::CoreSingleton<
    InstrumentedObject,
    pattern::NewAllocationStrategy,
    pattern::StandardDisposalStrategy,
    pattern::AtomicSynchronizationStrategy
>;

/**
 * Basic singleton instance uniqueness test
 */
TEST_CASE("Singleton ensures uniqueness of instances", "[singleton]") {
    SECTION("Single-threaded singleton creates exactly one instance") {
        // Get first reference
        InstrumentedObject& instance1 = SingleThreadSingleton::GetInstance();
        int id1 = instance1.getInstanceId();
        
        // Get second reference
        InstrumentedObject& instance2 = SingleThreadSingleton::GetInstance();
        int id2 = instance2.getInstanceId();
        
        // Verify uniqueness
        REQUIRE(id1 == id2);
        REQUIRE(&instance1 == &instance2);
        REQUIRE(InstrumentedObject::getInstanceCount() == 1);
    }
}

/**
 * Multi-threaded singleton test
 */
TEST_CASE("Singleton is thread-safe with proper synchronization", "[singleton][threading]") {
    SECTION("Multiple threads access the same singleton instance") {
        const int THREAD_COUNT = 8;
        std::vector<std::thread> threads;
        std::vector<int> thread_instance_ids(THREAD_COUNT);
        
        // Create multiple threads that all access the singleton
        for (int i = 0; i < THREAD_COUNT; ++i) {
            threads.emplace_back([i, &thread_instance_ids]() {
                // Access the singleton from each thread
                InstrumentedObject& instance = ThreadSafeSingleton::GetInstance();
                thread_instance_ids[i] = instance.getInstanceId();
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Verify that all threads got the same instance
        for (int i = 1; i < THREAD_COUNT; ++i) {
            REQUIRE(thread_instance_ids[0] == thread_instance_ids[i]);
        }
        
        // Only one instance should exist
        REQUIRE(InstrumentedObject::getInstanceCount() == 2);
    }
}

/**
 * Atomic synchronization test
 */
TEST_CASE("Singleton with atomic synchronization works correctly", "[singleton][atomic]") {
    SECTION("Atomic synchronization creates exactly one instance across threads") {
        const int THREAD_COUNT = 8;
        std::vector<std::thread> threads;
        
        // Create multiple threads that all access the atomic singleton
        for (int i = 0; i < THREAD_COUNT; ++i) {
            threads.emplace_back([]() {
                // Access the singleton from each thread
                InstrumentedObject& instance = AtomicSingleton::GetInstance();
                int id = instance.getInstanceId();
                std::cout << "Thread got atomic instance id: " << id << std::endl;
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Verify that only one instance was created
        REQUIRE(InstrumentedObject::getInstanceCount() == 3);
    }
}