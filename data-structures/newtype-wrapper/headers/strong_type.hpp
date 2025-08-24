// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef STRONG_TYPE_HPP
#define STRONG_TYPE_HPP

// historical context of the strong type wrapper (newtype) pattern
// the strong type wrapper pattern, also known as the "newtype" pattern, has its origins in functional
// programming languages like haskell where it was introduced to create distinct types with zero runtime
// overhead. in haskell, the newtype declaration creates a new type that is distinct from its underlying
// representation at compile-time but identical at runtime. this pattern migrated to c++ as developers
// sought ways to create type-safe interfaces that prevent mixing incompatible values even when they have
// the same underlying type. the pattern gained prominence in c++ with the rise of template metaprogramming
// and was further refined with c++11's explicit constructors and c++20's concepts. modern implementations
// leverage crtp (curiously recurring template pattern) to provide composable functionality through mixins,
// allowing developers to selectively add operations like arithmetic, comparison, or streaming capabilities.
// the pattern is particularly valuable in domain-driven design where distinct business concepts need to be
// represented as distinct types even when they share the same underlying representation, preventing errors
// like mixing meters with feet or dollars with euros at compile-time rather than runtime.

#include <concepts>
#include <compare>
#include <functional>
#include <iostream>
#include <format>
#include <type_traits>
#include <utility>
#include <expected>
#include <string>
#include <sstream>

// phantom type tag for type distinction
template<typename PhantomTag>
struct TypeTag {
    // explicit deleted constructors to prevent instantiation
    TypeTag() = delete;
    TypeTag(const TypeTag&) = delete;
    TypeTag(TypeTag&&) = delete;
    TypeTag& operator=(const TypeTag&) = delete;
    TypeTag& operator=(TypeTag&&) = delete;
    ~TypeTag() = delete;
};

// mixin for equality comparison operations
template<typename Derived>
struct EqualityComparable {
    // enable equality comparison for the derived type
    friend constexpr bool operator==(const Derived& lhs, const Derived& rhs) noexcept(noexcept(lhs.value() == rhs.value())) {
        return lhs.value() == rhs.value();
    }
    
    // inequality is automatically generated in c++20
};

// mixin for ordered comparison operations
template<typename Derived>
struct OrderedComparable : EqualityComparable<Derived> {
    // enable three-way comparison for complete ordering
    friend constexpr auto operator<=>(const Derived& lhs, const Derived& rhs) noexcept(noexcept(lhs.value() <=> rhs.value())) {
        return lhs.value() <=> rhs.value();
    }
};

// mixin for arithmetic operations
template<typename Derived>
struct Arithmetic {
    // addition operator
    friend constexpr Derived operator+(const Derived& lhs, const Derived& rhs) noexcept(noexcept(Derived{lhs.value() + rhs.value()})) {
        return Derived{lhs.value() + rhs.value()};
    }
    
    // subtraction operator
    friend constexpr Derived operator-(const Derived& lhs, const Derived& rhs) noexcept(noexcept(Derived{lhs.value() - rhs.value()})) {
        return Derived{lhs.value() - rhs.value()};
    }
    
    // multiplication operator
    friend constexpr Derived operator*(const Derived& lhs, const Derived& rhs) noexcept(noexcept(Derived{lhs.value() * rhs.value()})) {
        return Derived{lhs.value() * rhs.value()};
    }
    
    // division operator with error handling
    friend constexpr std::expected<Derived, std::string> operator/(const Derived& lhs, const Derived& rhs) noexcept {
        if (rhs.value() == typename Derived::value_type{}) {
            return std::unexpected("division by zero");
        }
        return Derived{lhs.value() / rhs.value()};
    }
    
    // compound assignment operators
    constexpr Derived& operator+=(const Derived& other) noexcept(noexcept(std::declval<typename Derived::value_type&>() += other.value())) {
        static_cast<Derived&>(*this).m_value += other.value();
        return static_cast<Derived&>(*this);
    }
    
    constexpr Derived& operator-=(const Derived& other) noexcept(noexcept(std::declval<typename Derived::value_type&>() -= other.value())) {
        static_cast<Derived&>(*this).m_value -= other.value();
        return static_cast<Derived&>(*this);
    }
    
    constexpr Derived& operator*=(const Derived& other) noexcept(noexcept(std::declval<typename Derived::value_type&>() *= other.value())) {
        static_cast<Derived&>(*this).m_value *= other.value();
        return static_cast<Derived&>(*this);
    }
    
    // division with assignment requires error handling
    constexpr std::expected<void, std::string> divide_assign(const Derived& other) noexcept {
        if (other.value() == typename Derived::value_type{}) {
            return std::unexpected("division by zero");
        }
        static_cast<Derived&>(*this).m_value /= other.value();
        return {};
    }
};

// mixin for increment and decrement operations
template<typename Derived>
struct Incrementable {
    // pre-increment
    constexpr Derived& operator++() noexcept(noexcept(++std::declval<typename Derived::value_type&>())) {
        ++static_cast<Derived&>(*this).m_value;
        return static_cast<Derived&>(*this);
    }
    
    // post-increment
    constexpr Derived operator++(int) noexcept(noexcept(std::declval<typename Derived::value_type&>()++)) {
        Derived temp{static_cast<const Derived&>(*this)};
        ++static_cast<Derived&>(*this).m_value;
        return temp;
    }
    
    // pre-decrement
    constexpr Derived& operator--() noexcept(noexcept(--std::declval<typename Derived::value_type&>())) {
        --static_cast<Derived&>(*this).m_value;
        return static_cast<Derived&>(*this);
    }
    
    // post-decrement
    constexpr Derived operator--(int) noexcept(noexcept(std::declval<typename Derived::value_type&>()--)) {
        Derived temp{static_cast<const Derived&>(*this)};
        --static_cast<Derived&>(*this).m_value;
        return temp;
    }
};

// mixin for stream operations
template<typename Derived>
struct Streamable {
    // output stream operator
    friend std::ostream& operator<<(std::ostream& os, const Derived& obj) {
        return os << obj.value();
    }
    
    // input stream operator with error handling
    friend std::istream& operator>>(std::istream& is, Derived& obj) {
        typename Derived::value_type temp;
        is >> temp;
        if (is) {
            obj = Derived{std::move(temp)};
        }
        return is;
    }
};

// mixin for hash support
template<typename Derived>
struct Hashable {
    // hash specialization friend function
    friend struct std::hash<Derived>;
};

// core strong type wrapper template
template<std::copyable UnderlyingType, typename TagType, template<typename> typename... Mixins>
    requires std::regular<UnderlyingType>
class StrongType : public Mixins<StrongType<UnderlyingType, TagType, Mixins...>>... {
public:
    // type aliases for introspection
    using value_type = UnderlyingType;
    using tag_type = TagType;
    
private:
    // underlying value storage
    UnderlyingType m_value{};
    
    // friend declarations for mixin access
    template<typename> friend struct EqualityComparable;
    template<typename> friend struct OrderedComparable;
    template<typename> friend struct Arithmetic;
    template<typename> friend struct Incrementable;
    template<typename> friend struct Streamable;
    template<typename> friend struct Hashable;
    
public:
    // default constructor - value initialized
    constexpr StrongType() noexcept(std::is_nothrow_default_constructible_v<UnderlyingType>) = default;
    
    // explicit constructor from underlying type
    constexpr explicit StrongType(const UnderlyingType& value) noexcept(std::is_nothrow_copy_constructible_v<UnderlyingType>)
        : m_value{value} {}
    
    // explicit constructor from rvalue
    constexpr explicit StrongType(UnderlyingType&& value) noexcept(std::is_nothrow_move_constructible_v<UnderlyingType>)
        : m_value{std::move(value)} {}
    
    // perfect forwarding constructor for in-place construction
    template<typename... Args>
        requires std::constructible_from<UnderlyingType, Args...>
    constexpr explicit StrongType(std::in_place_t, Args&&... args) 
        noexcept(std::is_nothrow_constructible_v<UnderlyingType, Args...>)
        : m_value{std::forward<Args>(args)...} {}
    
    // copy constructor
    constexpr StrongType(const StrongType&) noexcept(std::is_nothrow_copy_constructible_v<UnderlyingType>) = default;
    
    // move constructor
    constexpr StrongType(StrongType&&) noexcept(std::is_nothrow_move_constructible_v<UnderlyingType>) = default;
    
    // copy assignment
    constexpr StrongType& operator=(const StrongType&) noexcept(std::is_nothrow_copy_assignable_v<UnderlyingType>) = default;
    
    // move assignment
    constexpr StrongType& operator=(StrongType&&) noexcept(std::is_nothrow_move_assignable_v<UnderlyingType>) = default;
    
    // destructor
    ~StrongType() = default;
    
    // const accessor for underlying value
    [[nodiscard]] constexpr const UnderlyingType& value() const& noexcept {
        return m_value;
    }
    
    // mutable accessor for underlying value
    [[nodiscard]] constexpr UnderlyingType& value() & noexcept {
        return m_value;
    }
    
    // rvalue accessor for underlying value
    [[nodiscard]] constexpr UnderlyingType&& value() && noexcept {
        return std::move(m_value);
    }
    
    // const rvalue accessor for underlying value
    [[nodiscard]] constexpr const UnderlyingType&& value() const&& noexcept {
        return std::move(m_value);
    }
    
    // explicit conversion to underlying type
    [[nodiscard]] constexpr explicit operator const UnderlyingType&() const& noexcept {
        return m_value;
    }
    
    // explicit conversion to mutable underlying type
    [[nodiscard]] constexpr explicit operator UnderlyingType&() & noexcept {
        return m_value;
    }
    
    // swap operation
    constexpr void swap(StrongType& other) noexcept(std::is_nothrow_swappable_v<UnderlyingType>) {
        using std::swap;
        swap(m_value, other.m_value);
    }
    
    // friend swap function
    friend constexpr void swap(StrongType& lhs, StrongType& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }
    
    // transform operation for railway-oriented programming
    template<std::invocable<const UnderlyingType&> TransformFunc>
        requires std::constructible_from<UnderlyingType, std::invoke_result_t<TransformFunc, const UnderlyingType&>>
    [[nodiscard]] constexpr auto transform(TransformFunc&& func) const& 
        -> std::expected<StrongType<std::invoke_result_t<TransformFunc, const UnderlyingType&>, TagType, Mixins...>, std::string> {
        try {
            return StrongType<std::invoke_result_t<TransformFunc, const UnderlyingType&>, TagType, Mixins...>{
                std::forward<TransformFunc>(func)(m_value)
            };
        } catch (const std::exception& e) {
            return std::unexpected(std::string{e.what()});
        } catch (...) {
            return std::unexpected("unknown error in transform");
        }
    }
    
    // and_then operation for chaining
    template<std::invocable<const UnderlyingType&> ThenFunc>
    [[nodiscard]] constexpr auto and_then(ThenFunc&& func) const& 
        -> std::invoke_result_t<ThenFunc, const UnderlyingType&> {
        return std::forward<ThenFunc>(func)(m_value);
    }
    
    // or_else operation for error recovery
    template<typename DefaultValue>
        requires std::convertible_to<DefaultValue, UnderlyingType>
    [[nodiscard]] constexpr UnderlyingType value_or(DefaultValue&& default_value) const& noexcept {
        return m_value != UnderlyingType{} ? m_value : static_cast<UnderlyingType>(std::forward<DefaultValue>(default_value));
    }
};

// hash specialization for hashable strong types
namespace std {
    template<std::copyable UnderlyingType, typename TagType, template<typename> typename... Mixins>
        requires std::regular<UnderlyingType>
    struct hash<StrongType<UnderlyingType, TagType, Mixins...>> {
        [[nodiscard]] constexpr std::size_t operator()(const StrongType<UnderlyingType, TagType, Mixins...>& obj) const noexcept {
            return std::hash<UnderlyingType>{}(obj.value());
        }
    };
}

// utility macro for defining strong type aliases
#define DEFINE_STRONG_TYPE(Name, UnderlyingType, ...) \
    using Name = StrongType<UnderlyingType, TypeTag<struct Name##Tag> __VA_OPT__(,) __VA_ARGS__>

// common strong type definitions with various mixin combinations
namespace StrongTypes {
    // numeric types with full arithmetic support
    DEFINE_STRONG_TYPE(Integer, int, EqualityComparable, OrderedComparable, Arithmetic, Incrementable, Streamable, Hashable);
    DEFINE_STRONG_TYPE(Real, double, EqualityComparable, OrderedComparable, Arithmetic, Streamable, Hashable);
    
    // identifier types with minimal operations
    DEFINE_STRONG_TYPE(UserId, std::size_t, EqualityComparable, OrderedComparable, Streamable, Hashable);
    DEFINE_STRONG_TYPE(SessionId, std::string, EqualityComparable, Streamable, Hashable);
    
    // measurement types with arithmetic
    DEFINE_STRONG_TYPE(Meters, double, EqualityComparable, OrderedComparable, Arithmetic, Streamable);
    DEFINE_STRONG_TYPE(Seconds, double, EqualityComparable, OrderedComparable, Arithmetic, Streamable);
    
    // currency type with controlled operations
    DEFINE_STRONG_TYPE(Dollars, double, EqualityComparable, OrderedComparable, Arithmetic, Streamable);
    
    // percentage type with bounds checking capability
    DEFINE_STRONG_TYPE(Percentage, double, EqualityComparable, OrderedComparable, Streamable);
}

#endif // STRONG_TYPE_HPP