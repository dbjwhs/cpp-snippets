#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Get the compiler executable
CXX=${CXX:-clang++}
echo "Using compiler: $CXX"

# Check if we're using Clang or GCC and set appropriate flags
if [[ "$CXX" == *"clang"* ]]; then
    # Clang module flags
    MODULE_FLAGS="-std=c++20 -fmodules -fbuiltin-module-map"
    echo "Using Clang module flags"
else
    # GCC module flags
    MODULE_FLAGS="-std=c++20 -fmodules-ts"
    echo "Using GCC module flags"
fi

# Ensure module cache directory exists
mkdir -p pcm

# Step 1: Compile the module interface
echo "Compiling math_module interface..."
$CXX $MODULE_FLAGS -c ../src/math_module.cppm -o math_module.o

# Step 2: Compile the module implementation
echo "Compiling math_module implementation..."
$CXX $MODULE_FLAGS -c ../src/math_module.cpp -o math_module_impl.o

# Step 3: Compile the main file
echo "Compiling main file..."
$CXX $MODULE_FLAGS -c ../src/modules.cpp -o modules.o

# Step 4: Link everything together
echo "Linking executable..."
$CXX math_module.o math_module_impl.o modules.o -o modules_example

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo "Build completed successfully. Run the executable with:"
    echo "./modules_example"
else
    echo "Build failed. Check the errors above."
fi
