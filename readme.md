# Create and enter the build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the demo application
./bin/singleton_demo

# Run the tests
./bin/singleton_tests
