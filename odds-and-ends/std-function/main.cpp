#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <cassert>

// To educate me on std::function I created this example to demonstrates all the ways to use std::function and raw
// function pointers in modern C++20
//
// No classes are used, only free functions, lambdas, and function objects via structs

// ==================== PART 1: FUNCTION DECLARATIONS ====================

// Regular free function that will be used with both raw function pointers and std::function
bool freeFunction(const int x, const double y) {
    std::cout << "Free function called with " << x << " and " << y << std::endl;
    return x > static_cast<int>(y);
}

// Overloaded free function - demonstrates how std::function can help disambiguate
bool freeFunction(const std::string& str) {
    std::cout << "Overloaded free function called with: " << str << std::endl;
    return !str.empty();
}

// Variadic function template - can be used with specific instantiations
template<typename... Args>
bool variadicFunction(Args... args) {
    std::cout << "Variadic function with " << sizeof...(args) << " arguments" << std::endl;
    return sizeof...(args) > 0;
}

// Function that takes a raw function pointer
bool takesRawFunctionPointer(bool (*funcPtr)(int, double), int a, double b) {
    std::cout << "Called via raw function pointer" << std::endl;
    return funcPtr(a, b);
}

// Function that takes std::function
bool takesStdFunction(const std::function<bool(int, double)>& func, int a, double b) {
    std::cout << "Called via std::function" << std::endl;
    return func(a, b);
}

// Function returning a raw function pointer
bool (*getFunctionPointer())(int, double) {
    return &freeFunction;
}

// Function returning std::function
std::function<bool(int, double)> getStdFunction() {
    // Need to disambiguate the overloaded freeFunction
    return static_cast<bool(*)(int, double)>(&freeFunction);
}

// ==================== PART 2: MAIN FUNCTION WITH EXAMPLES ====================

int main() {
    std::cout << "===== MODERN C++20 FUNCTION EXAMPLES =====" << std::endl;

    // ========== RAW FUNCTION POINTERS ==========
    std::cout << "\n----- RAW FUNCTION POINTERS -----\n" << std::endl;

    // 1. Basic function pointer declaration and assignment
    bool (*rawFuncPtr)(int, double) = &freeFunction;
    // The & is optional in most cases, but recommended for clarity

    // 2. Calling a function through a raw function pointer
    bool result1 = rawFuncPtr(5, 3.5);
    std::cout << "Result 1: " << std::boolalpha << result1 << std::endl;

    // 3. Function pointer initialization without explicit type - need to disambiguate overloaded function
    auto rawFuncPtrAuto = static_cast<bool(*)(int, double)>(&freeFunction);
    bool result2 = rawFuncPtrAuto(10, 7.5);
    std::cout << "Result 2: " << result2 << std::endl;

    // 4. Using typedef to simplify function pointer types
    typedef bool (*FunctionPointerType)(int, double);
    FunctionPointerType typedeffedFuncPtr = &freeFunction;
    bool result3 = typedeffedFuncPtr(10, 7.5);
    std::cout << "Result 3: " << result3 << std::endl;

    // 5. Using alias (more modern than typedef)
    using ModernFuncPtrType = bool(*)(int, double);
    ModernFuncPtrType modernFuncPtr = &freeFunction;
    bool result_mod = modernFuncPtr(10, 7.5);
    std::cout << "Result Modern Type: " << result_mod << std::endl;

    // 6. Storing function pointers in an array
    bool (*funcPtrArray[3])(int, double) = {&freeFunction, &freeFunction, &freeFunction};
    for (int i = 0; i < 3; ++i) {
        bool result_array = funcPtrArray[i](1, i + 0.5);
        std::cout << "Result Modern Type: (" << i << ") " << result_array << std::endl;
    }

    // 7. Passing function pointers to other functions
    bool result_takes = takesRawFunctionPointer(rawFuncPtr, 15, 10.5);
    std::cout << "Result Takes Type:" << result_takes << std::endl;

    // 8. Getting function pointers from functions
    bool (*retrievedFuncPtr)(int, double) = getFunctionPointer();
    bool result_retrieved = retrievedFuncPtr(20, 15.5);
    std::cout << "Result Retrieved Type:" << result_retrieved << std::endl;

    // 9. Function pointers as template arguments
    std::vector<bool(*)(int, double)> funcPtrVector;
    funcPtrVector.push_back(static_cast<bool(*)(int, double)>(&freeFunction));

    // 10. Null function pointer check
    bool (*nullFuncPtr)(int, double) = nullptr;
    if (nullFuncPtr == nullptr) {
        std::cout << "Function pointer is null" << std::endl;
    }

    // ========== STD::FUNCTION ==========
    std::cout << "\n----- STD::FUNCTION EXAMPLES -----\n" << std::endl;

    // 1. Basic std::function creation - need to disambiguate overloaded function
    std::function<bool(int, double)> stdFunc = static_cast<bool(*)(int, double)>(&freeFunction);
    bool result4 = stdFunc(25, 20.5);
    std::cout << "Result 4: " << result4 << std::endl;

    // 2. std::function with lambda (stateless)
    std::function<bool(int, double)> lambdaFunc = [](int x, double y) {
        std::cout << "Stateless lambda called with " << x << " and " << y << std::endl;
        return x < static_cast<int>(y);
    };
    bool result_lambda1 = lambdaFunc(30, 25.5);
    std::cout << "Lambda 1 " << result_lambda1 << std::endl;

    // 3. std::function with stateful lambda (with captures)
    int multiplier = 2;
    std::function<bool(int, double)> statefulLambda = [multiplier](int x, double y) {
        std::cout << "Stateful lambda with multiplier=" << multiplier << std::endl;
        return (x * multiplier) > y;
    };
    bool result_lambda2 = statefulLambda(5, 15.0);
    std::cout << "Lambda 2 " << result_lambda2 << std::endl;

    // 4. std::function with a function object (functor via struct)
    struct Functor {
        bool operator()(int x, double y) const {
            std::cout << "Functor called with " << x << " and " << y << std::endl;
            return x == static_cast<int>(y);
        }
    };
    std::function<bool(int, double)> functorFunc = Functor{};
    bool result_lambda3 = functorFunc(35, 35.0);
    std::cout << "Lambda 3 " << result_lambda3 << std::endl;

    // 5. std::function with std::bind - binding a free function
    // Need to disambiguate overloaded function for bind
    auto boundFunc = std::bind(static_cast<bool(*)(int, double)>(&freeFunction),
                              std::placeholders::_1, 100.5);
    std::function<bool(int)> bindFunc = boundFunc;
    bool result_bind = bindFunc(40);
    std::cout << "Bind " << result_bind << std::endl;

    // 6. std::function with std::bind with reordered parameters
    auto reorderedBindFunc = std::bind(static_cast<bool(*)(int, double)>(&freeFunction),
                                      std::placeholders::_2, // Second arg becomes first
                                      std::placeholders::_1); // First arg becomes second
    std::function<bool(double, int)> reorderedFunc = reorderedBindFunc;
    bool result_reorder = reorderedFunc(45.5, 50);
    std::cout << "Bind " << result_reorder << std::endl;

    // 7. std::function with std::bind to bind some parameters
    auto partialBindFunc = std::bind(static_cast<bool(*)(int, double)>(&freeFunction), 60, std::placeholders::_1);
    std::function<bool(double)> partialFunc = partialBindFunc;
    bool result_partial = partialFunc(55.5);
    std::cout << "Bind partial" << result_partial << std::endl;

    // 8. std::function to store overloaded function (explicit disambiguation)
    std::function<bool(const std::string&)> overloadedFunc =
        static_cast<bool(*)(const std::string&)>(&freeFunction);
    bool result_overload = overloadedFunc("Hello, std::function!");
    std::cout << "Overload " << result_overload << std::endl;

    // 9. Empty std::function check
    std::function<void()> emptyFunc;
    if (!emptyFunc) {
        std::cout << "std::function is empty" << std::endl;
    }

    // 10. Assigning to an empty std::function
    emptyFunc = []() { std::cout << "No longer empty" << std::endl; };
    if (emptyFunc) {
        emptyFunc();
    }

    // 11. std::function with variadic template function
    std::function<bool(int, double, std::string)> variadicFunc =
        variadicFunction<int, double, std::string>;
    bool result_variadic = variadicFunc(70, 65.5, "Variadic");
    std::cout << "Variadic " << result_variadic << std::endl;

    // 12. Passing std::function to functions
    bool result_stdfunc = takesStdFunction(stdFunc, 75, 70.5);
    std::cout << "std::func " << result_stdfunc << std::endl;

    // 13. Getting std::function from functions
    std::function<bool(int, double)> retrievedStdFunc = getStdFunction();
    bool result_retrieved2 = retrievedStdFunc(80, 75.5);
    std::cout << "retrieved 2 " << result_retrieved2 << std::endl;

    // 14. Storing std::function in containers
    std::vector<std::function<bool(int, double)>> funcVector;
    funcVector.push_back(static_cast<bool(*)(int, double)>(&freeFunction));
    funcVector.push_back(lambdaFunc);
    funcVector.push_back(statefulLambda);
    funcVector.push_back(functorFunc);

    for (const auto& func : funcVector) {
        bool result = func(85, 80.5);
        std::cout << "funcVector " << result << std::endl;
    }

    // 15. Nested std::function (function returning function)
    std::function<std::function<bool(double)>(int)> nestedFunc =
        [](int x) -> std::function<bool(double)> {
            return [x](double y) {
                std::cout << "Nested function with " << x << " and " << y << std::endl;
                return x > static_cast<int>(y);
            };
        };

    auto innerFunc = nestedFunc(90);
    bool result_nested = innerFunc(85.5);
    std::cout << "funcVector " << result_nested << std::endl;

    // ========== ADVANCED EXAMPLES ==========
    std::cout << "\n----- ADVANCED EXAMPLES -----\n" << std::endl;

    // 1. Function pointer to a function template instantiation
    bool (*templateFuncPtr)(int, double, std::string) = &variadicFunction<int, double, std::string>;
    bool result_template_fptr = templateFuncPtr(95, 90.5, "Template");
    std::cout << "function template " << result_template_fptr << std::endl;

    // 2. std::function storing a mutable lambda
    int counter = 0;
    std::function<bool()> mutableLambda = [counter]() mutable {
        std::cout << "Mutable lambda counter: " << ++counter << std::endl;
        return counter > 5;
    };

    for (int i = 0; i < 3; ++i) {
        bool result_mut_lambda = mutableLambda();
        std::cout << "function template " << result_mut_lambda << std::endl;
    }

    // 3. std::function with std::ref to capture by reference
    int externalState = 100;
    std::function<void()> refCaptureLambda = [&externalState]() {
        std::cout << "Before modification: " << externalState << std::endl;
        externalState += 10;
        std::cout << "After modification: " << externalState << std::endl;
    };
    refCaptureLambda();

    // 4. Using std::bind with std::ref
    int bindRefValue = 200;
    auto bindWithRef = std::bind([](int& x, const double y) -> bool {
        std::cout << "Bind with ref before: " << x << std::endl;
        x += 20;
        std::cout << "Bind with ref after: " << x << std::endl;
        return x > y;
    }, std::ref(bindRefValue), std::placeholders::_1);

    std::function<bool(double)> bindRefFunc = bindWithRef;
    bool result_bindref = bindRefFunc(205.5);
    std::cout << "result_bindref after call: " << result_bindref << std::endl;

    // 5. Storing generic lambda (C++14 and later)
    // Note: We cannot directly create a std::function with auto parameters,
    // so we store specific instantiations instead
    auto genericLambda = [](auto x, auto y) {
        std::cout << "Generic lambda with " << x << " and " << y << std::endl;
        return sizeof(x) > sizeof(y);
    };

    // Use the generic lambda with specific types
    bool result_generic1 = genericLambda(100, 99.5);
    std::cout << "result_generic1 after call: " << result_generic1 << std::endl;
    bool result_generic2 = genericLambda("hello", 123);
    std::cout << "result_generic2 after call: " << result_generic2 << std::endl;

    // Store specific instantiations in std::function
    std::function<bool(int, double)> specificGenericLambda =
        [](auto x, auto y) {
            std::cout << "Specific generic lambda with " << x << " and " << y << std::endl;
            return sizeof(x) < sizeof(y);
        };
    bool result_specificGenericLambda = specificGenericLambda(210, 205.5);
    std::cout << "result_specificGenericLambda after call: " << result_specificGenericLambda << std::endl;

    // 6. Using C++20 std::bind_front (simpler than std::bind)
    auto bindFrontFunc = std::bind_front(static_cast<bool(*)(int, double)>(&freeFunction), 220);
    std::function<bool(double)> bindFrontStdFunc = bindFrontFunc;
    bool result_bindFrontStdFunc = bindFrontStdFunc(215.5);
    std::cout << "result_bindFrontStdFunc after call: " << result_bindFrontStdFunc << std::endl;

    // 7. std::function with noexcept lambda (C++17 and later)
    std::function<bool(int, double)> noexceptLambda = [](int x, double y) noexcept {
        std::cout << "Noexcept lambda with " << x << " and " << y << std::endl;
        return x >= static_cast<int>(y);
    };
    bool result_noexceptLambda = noexceptLambda(230, 225.5);
    std::cout << "noexceptLambda after call: " << result_noexceptLambda << std::endl;

    // 8. Smart pointer to hold dynamically allocated std::function
    auto funcPtr = std::make_unique<std::function<bool(int, double)>>(
        [](int x, double y) {
            std::cout << "Function in smart pointer with " << x << " and " << y << std::endl;
            return x != static_cast<int>(y);
        }
    );
    bool result_funcptr = (*funcPtr)(240, 235.5);
    std::cout << "result_funcptr after call: " << result_funcptr << std::endl;

    // ========== PERFORMANCE CONSIDERATIONS ==========
    std::cout << "\n----- PERFORMANCE CONSIDERATIONS -----\n" << std::endl;

    // Raw function pointers are more lightweight than std::function
    std::cout << "Size of raw function pointer: "
              << sizeof(bool(*)(int, double)) << " bytes" << std::endl;

    std::cout << "Size of std::function: "
              << sizeof(std::function<bool(int, double)>) << " bytes" << std::endl;

    // The difference is because std::function uses type erasure and may allocate memory

    // ========== C++20 FEATURES ==========
    std::cout << "\n----- C++20 FEATURES -----\n" << std::endl;

    // 1. C++20 requires expression for disambiguating function overloads
    auto overloadedFuncPtr = +[](const int x, const double y) {
        return freeFunction(x, y);
    };
    std::function<bool(int, double)> requiresFunc = overloadedFuncPtr;
    bool result_requiresFunc = requiresFunc(250, 245.5);
    std::cout << "result_requiresFunc after call: " << result_requiresFunc << std::endl;

    // Note: We also previously used std::bind_front which is a C++20 feature

    std::cout << "\n===== END OF EXAMPLES =====" << std::endl;
    return 0;
}
