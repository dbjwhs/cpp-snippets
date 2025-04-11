// MIT License
// Copyright (c) 2025 dbjwhs

// implementation file for the math module
// in a real-world scenario, we would put implementation details here
// to separate interface from implementation

module math_module;

// note: since our example is simple, all implementations are in the interface file
// this file serves as a demonstration of how implementation files work with modules

// we can have module-private implementation details here that won't be visible to importers
namespace {
    // this function is only visible within this module implementation
    int gcd(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }
}

// implementation details for the advanced partition
module math_module:advanced;

// additional implementation details for the advanced partition can go here
// in this example, all implementations are in the interface file
