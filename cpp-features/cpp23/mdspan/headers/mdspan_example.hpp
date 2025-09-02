// MIT License
// Copyright (c) 2025 dbjwhs

#pragma once

#include <mdspan>
#include <vector>
#include <array>
#include <expected>
#include <string>
#include <concepts>
#include "../../../../headers/project_utils.hpp"

// std::mdspan is a C++23 feature that provides a non-owning multidimensional array view over a contiguous sequence of
// objects. This pattern originated from the Linear Algebra SIG's proposal for multidimensional arrays in scientific
// computing applications. The concept was first introduced as part of P0009 and evolved through extensive standardization
// work involving major contributors from Sandia National Labs, NVIDIA, and other HPC organizations.
//
// The mdspan pattern emerged from the need to provide zero-cost abstractions for multidimensional data structures
// commonly used in scientific computing, machine learning, and image processing. Unlike traditional array-of-arrays
// approaches, mdspan provides a unified interface that can work with various memory layouts while maintaining
// performance characteristics essential for high-performance computing applications.
//
// Key innovations include:
// - Layout policies that abstract memory organization (row-major, column-major, strided)
// - Accessor policies for custom element access patterns
// - Extents that encode dimensional information at compile-time when possible
// - Integration with existing STL containers without requiring data copies
//
// This design enables efficient interoperability between different libraries and frameworks while maintaining
// type safety and enabling compiler optimizations. The pattern has been successfully deployed in production
// HPC codebases including Kokkos, RAJA, and various scientific computing frameworks.

namespace mdspan_demo {

// error type for railway-oriented programming
enum class MdspanError {
    invalid_dimensions,
    out_of_bounds,
    incompatible_layout,
    allocation_failure
};

// result type using std::expected
template<typename ValueType>
using Result = std::expected<ValueType, MdspanError>;

// modern template parameter naming with concept constraints
template<std::copyable ElementType>
class MdspanExample {
private:
    // storage for the underlying data
    std::vector<ElementType> m_data{};
    
    // dimensions for our examples
    size_t m_rows{};
    size_t m_cols{};
    size_t m_depth{};

public:
    // constructor for 1d data
    explicit MdspanExample(size_t size);
    
    // constructor for 2d data
    MdspanExample(size_t rows, size_t cols);
    
    // constructor for 3d data
    MdspanExample(size_t rows, size_t cols, size_t depth);
    
    // demonstrate basic 1d mdspan usage
    auto demonstrate_1d_basics() -> Result<void>;
    
    // demonstrate 2d mdspan with different layouts
    auto demonstrate_2d_layouts() -> Result<void>;
    
    // demonstrate 3d mdspan for tensor operations
    auto demonstrate_3d_tensor() -> Result<void>;
    
    // demonstrate submdspan functionality
    auto demonstrate_subspan() -> Result<void>;
    
    // demonstrate custom accessor patterns
    auto demonstrate_custom_accessors() -> Result<void>;
    
    // demonstrate integration with algorithms
    auto demonstrate_algorithm_integration() -> Result<void>;
    
    // demonstrate performance characteristics
    auto demonstrate_performance() -> Result<void>;
    
    // run comprehensive test suite
    auto run_comprehensive_tests() -> Result<void>;

private:
    // helper to initialize data with pattern
    auto initialize_data_pattern() -> void;
    
    // helper to validate bounds
    [[nodiscard]] auto validate_bounds(size_t row, size_t col, size_t depth = 0) const -> bool;
    
    // helper to get linear index
    [[nodiscard]] auto get_linear_index(size_t row, size_t col, size_t depth = 0) const -> size_t;
};

// custom accessor for demonstration
template<std::copyable ElementType>
struct ScalingAccessor {
    using offset_policy = std::default_accessor<ElementType>;
    using element_type = ElementType;
    using reference = ElementType&;
    using data_handle_type = ElementType*;
    
private:
    ElementType m_scale_factor{};
    
public:
    constexpr explicit ScalingAccessor(ElementType scale = ElementType{2}) : m_scale_factor{scale} {}
    
    constexpr auto access(data_handle_type p, size_t i) const noexcept -> reference {
        return p[i];
    }
    
    constexpr auto offset(data_handle_type p, size_t i) const noexcept -> data_handle_type {
        return p + i;
    }
    
    constexpr auto get_scale_factor() const noexcept -> ElementType {
        return m_scale_factor;
    }
};

// demonstration functions for various mdspan features
auto demonstrate_basic_usage() -> Result<void>;
auto demonstrate_layout_policies() -> Result<void>;
auto demonstrate_submdspan_operations() -> Result<void>;
auto demonstrate_accessor_policies() -> Result<void>;
auto demonstrate_extents_usage() -> Result<void>;
auto demonstrate_performance_characteristics() -> Result<void>;

} // namespace mdspan_demo
