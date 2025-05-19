#include <iostream>

int main() {
    std::cout << "C++ Standard: " << __cplusplus << "\n";

#if defined(__cpp_lib_jthread)
    std::cout << "jthread is supported (version " << __cpp_lib_jthread << ")\n";
#else
    std::cout << "jthread is not supported\n";
#endif

    return 0;
}