cmake_minimum_required(VERSION 3.14)
project(OrthogonalSingleton 
        VERSION 1.0.0
        DESCRIPTION "Singleton pattern implementation with orthogonal strategies"
        LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define option for tests
option(BUILD_TESTS "Build test suite" ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add threading support
find_package(Threads REQUIRED)

# Add the main demo application
add_executable(singleton_demo src/demo.cpp)
target_link_libraries(singleton_demo PRIVATE Threads::Threads)

# Configure tests if enabled
if(BUILD_TESTS)
    # Include FetchContent module
    include(FetchContent)
    
    # Fetch and make Catch2 available
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.3.2  # Adjust as needed
    )
    FetchContent_MakeAvailable(Catch2)
    
    # Add test executable
    add_executable(singleton_tests test/singleton_test.cpp)
    target_link_libraries(singleton_tests PRIVATE Catch2::Catch2WithMain Threads::Threads)
    
    # Enable CTest integration
    include(CTest)
    include(Catch)
    catch_discover_tests(singleton_tests)
    
    # Add a custom target for running tests
    add_custom_target(run_tests 
                     COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/singleton_tests
                     DEPENDS singleton_tests
                     COMMENT "Running singleton tests")
endif()

# Installation configuration
install(TARGETS singleton_demo
        RUNTIME DESTINATION bin)

# Install header files
install(DIRECTORY include/
        DESTINATION include/orthogonal_singleton
        FILES_MATCHING PATTERN "*.hpp")

# Export targets
if(BUILD_TESTS)
    install(TARGETS singleton_tests
            RUNTIME DESTINATION bin)
endif()