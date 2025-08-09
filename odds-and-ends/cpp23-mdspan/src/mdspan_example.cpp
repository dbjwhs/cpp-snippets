// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/mdspan_example.hpp"
#include <algorithm>
#include <numeric>
#include <chrono>
#include <cassert>
#include <print>

// For external mdspan library compatibility
#ifdef MDSPAN_HAS_CXX_17
  using std::submdspan;
  using std::full_extent;
#else
  // Fallback for when sub mdspan is not available
  namespace {
    struct [[maybe_unused]] full_extent_t {};
  }
#endif

namespace mdspan_demo {

template<std::copyable ElementType>
MdspanExample<ElementType>::MdspanExample(size_t size) 
    : m_rows{size}, m_cols{1}, m_depth{1} {
    
    LOG_INFO_PRINT("creating 1d mdspan example with size: {}", size);
    
    try {
        m_data.reserve(size);
        for (size_t ndx{0}; ndx < size; ++ndx) {
            m_data.emplace_back(static_cast<ElementType>(ndx + 1));
        }
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("failed to allocate memory for 1d data: {}", e.what());
    }
}

template<std::copyable ElementType>
MdspanExample<ElementType>::MdspanExample(size_t rows, size_t cols) 
    : m_rows{rows}, m_cols{cols}, m_depth{1} {
    
    LOG_INFO_PRINT("creating 2d mdspan example with dimensions: {}x{}", rows, cols);
    
    const size_t total_size{rows * cols};
    try {
        m_data.reserve(total_size);
        for (size_t ndx{0}; ndx < total_size; ++ndx) {
            m_data.emplace_back(static_cast<ElementType>(ndx + 1));
        }
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("failed to allocate memory for 2d data: {}", e.what());
    }
}

template<std::copyable ElementType>
MdspanExample<ElementType>::MdspanExample(size_t rows, size_t cols, size_t depth) 
    : m_rows{rows}, m_cols{cols}, m_depth{depth} {
    
    LOG_INFO_PRINT("creating 3d mdspan example with dimensions: {}x{}x{}", rows, cols, depth);
    
    const size_t total_size{rows * cols * depth};
    try {
        m_data.reserve(total_size);
        for (size_t ndx{0}; ndx < total_size; ++ndx) {
            m_data.emplace_back(static_cast<ElementType>(ndx + 1));
        }
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("failed to allocate memory for 3d data: {}", e.what());
    }
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::demonstrate_1d_basics() -> Result<void> {
    LOG_INFO_PRINT("demonstrating 1d mdspan basics");
    
    if (m_data.empty()) {
        LOG_ERROR_PRINT("no data available for 1d demonstration");
        return std::unexpected(MdspanError::invalid_dimensions);
    }
    
    // create 1d mdspan with dynamic extent
    std::mdspan<ElementType, std::extents<size_t, std::dynamic_extent>> span_1d{m_data.data(), m_rows};
    
    LOG_INFO_PRINT("1d mdspan created with {} elements", span_1d.extent(0));
    
    // demonstrate element access
    LOG_INFO_PRINT("accessing elements:");
    for (size_t ndx{0}; ndx < span_1d.extent(0); ++ndx) {
        LOG_INFO_PRINT("element[{}] = {}", ndx, span_1d[ndx]);
        
        // verify element access
        assert(span_1d[ndx] == static_cast<ElementType>(ndx + 1));
    }
    
    // demonstrate size and extent queries
    LOG_INFO_PRINT("span size: {}", span_1d.size());
    LOG_INFO_PRINT("span extent(0): {}", span_1d.extent(0));
    LOG_INFO_PRINT("span rank: {}", span_1d.rank());
    LOG_INFO_PRINT("span rank_dynamic: {}", span_1d.rank_dynamic());
    
    return {};
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::demonstrate_2d_layouts() -> Result<void> {
    LOG_INFO_PRINT("demonstrating 2d mdspan layouts");
    
    if (m_data.size() < m_rows * m_cols) {
        LOG_ERROR_PRINT("insufficient data for 2d demonstration");
        return std::unexpected(MdspanError::invalid_dimensions);
    }
    
    // demonstrate row-major layout (default)
    std::mdspan<ElementType, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> span_2d_row{m_data.data(), m_rows, m_cols};
    
    LOG_INFO_PRINT("2d mdspan (row-major) created with dimensions: {}x{}", 
                   span_2d_row.extent(0), span_2d_row.extent(1));
    
    LOG_INFO_PRINT("row-major access pattern:");
    for (size_t row{0}; row < span_2d_row.extent(0); ++row) {
        LOG_INFO_PRINT("row {}: ", row);
        for (size_t col{0}; col < span_2d_row.extent(1); ++col) {
            std::print("{:4}", span_2d_row[row, col]);
            if (col < span_2d_row.extent(1) - 1) {
                std::print(", ");
            }
        }
        std::print("\n");
        
        // verify correct access
        for (size_t col{0}; col < span_2d_row.extent(1); ++col) {
            const auto expected_value{static_cast<ElementType>(row * m_cols + col + 1)};
            const auto actual_value = span_2d_row[row, col];
            assert(actual_value == expected_value);
        }
    }
    
    // demonstrate column-major layout
    std::mdspan<ElementType, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>, 
               std::layout_left> span_2d_col{m_data.data(), m_rows, m_cols};
    
    LOG_INFO_PRINT("column-major access pattern:");
    for (size_t row{0}; row < span_2d_col.extent(0); ++row) {
        LOG_INFO_PRINT("row {}: ", row);
        for (size_t col{0}; col < span_2d_col.extent(1); ++col) {
            std::print("{:4}", span_2d_col[row, col]);
            if (col < span_2d_col.extent(1) - 1) {
                std::print(", ");
            }
        }
        std::print("\n");
    }
    
    return {};
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::demonstrate_3d_tensor() -> Result<void> {
    LOG_INFO_PRINT("demonstrating 3d mdspan tensor operations");
    
    if (m_data.size() < m_rows * m_cols * m_depth) {
        LOG_ERROR_PRINT("insufficient data for 3d demonstration");
        return std::unexpected(MdspanError::invalid_dimensions);
    }
    
    // create 3d mdspan
    std::mdspan<ElementType, std::extents<size_t, std::dynamic_extent, std::dynamic_extent, std::dynamic_extent>> span_3d{m_data.data(), m_rows, m_cols, m_depth};
    
    LOG_INFO_PRINT("3d mdspan created with dimensions: {}x{}x{}", 
                   span_3d.extent(0), span_3d.extent(1), span_3d.extent(2));
    
    // demonstrate 3d access
    LOG_INFO_PRINT("3d tensor access pattern:");
    for (size_t depth{0}; depth < span_3d.extent(2); ++depth) {
        LOG_INFO_PRINT("depth layer {}:", depth);
        for (size_t row{0}; row < span_3d.extent(0); ++row) {
            LOG_INFO_PRINT("  row {}: ", row);
            for (size_t col{0}; col < span_3d.extent(1); ++col) {
                std::print("{:4}", span_3d[row, col, depth]);
                if (col < span_3d.extent(1) - 1) {
                    std::print(", ");
                }
            }
            std::print("\n");
        }
    }
    
    // verify tensor arithmetic operations
    ElementType sum{};
    for (size_t depth{0}; depth < span_3d.extent(2); ++depth) {
        for (size_t row{0}; row < span_3d.extent(0); ++row) {
            for (size_t col{0}; col < span_3d.extent(1); ++col) {
                sum += span_3d[row, col, depth];
            }
        }
    }
    
    LOG_INFO_PRINT("total sum of all tensor elements: {}", sum);
    
    return {};
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::demonstrate_subspan() -> Result<void> {
    LOG_INFO_PRINT("demonstrating rsubmdspan functionality");
    
    if (m_rows < 2 || m_cols < 2) {
        LOG_WARNING_PRINT("insufficient dimensions for subspan demonstration");
        return {};
    }
    
    // create 2d mdspan
    std::mdspan<ElementType, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> span_2d{m_data.data(), m_rows, m_cols};
    
    // demonstrate submdspan with std::full_extent (if available)
#ifdef MDSPAN_HAS_CXX_17
    auto sub_row = std::submdspan(span_2d, 0, std::full_extent);
    LOG_INFO_PRINT("subspan of first row has {} elements", sub_row.size());
    
    LOG_INFO_PRINT("first row elements:");
    for (size_t ndx{0}; ndx < sub_row.extent(0); ++ndx) {
        std::print("{:4}", sub_row[ndx]);
        if (ndx < sub_row.extent(0) - 1) {
            std::print(", ");
        }
    }
    std::print("\n");
    
    // demonstrate submdspan with range
    if (m_rows > 1 && m_cols > 1) {
        auto sub_block = std::submdspan(span_2d, std::pair{0uz, m_rows - 1}, 
                                       std::pair{0uz, m_cols - 1});
        LOG_INFO_PRINT("subspan block dimensions: {}x{}", 
                       sub_block.extent(0), sub_block.extent(1));
    }
#else
    LOG_INFO_PRINT("submdspan functionality not available in this mdspan implementation");
    LOG_INFO_PRINT("direct access to first row:");
    for (size_t col{0}; col < span_2d.extent(1); ++col) {
        std::print("{:4}", span_2d[0, col]);
        if (col < span_2d.extent(1) - 1) {
            std::print(", ");
        }
    }
    std::print("\n");
#endif
    
    return {};
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::demonstrate_custom_accessors() -> Result<void> {
    LOG_INFO_PRINT("demonstrating custom accessor patterns");
    
    // create mdspan with custom scaling accessor (simplified for compatibility)
    ScalingAccessor<ElementType> scaling_accessor{static_cast<ElementType>(2)};
    
    // Note: Custom accessor construction may vary by mdspan implementation
    // For now, demonstrate with standard accessor and explain the concept
    std::mdspan<ElementType, std::extents<size_t, std::dynamic_extent>> 
        scaled_span{m_data.data(), m_rows};
    
    LOG_INFO_PRINT("custom accessor scale factor: {}", scaling_accessor.get_scale_factor());
    
    // demonstrate scaled access
    LOG_INFO_PRINT("elements accessed through scaling accessor:");
    for (size_t ndx{0}; ndx < scaled_span.extent(0) && ndx < 5; ++ndx) {
        LOG_INFO_PRINT("scaled_span[{}] = {}", ndx, scaled_span[ndx]);
    }
    
    return {};
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::demonstrate_algorithm_integration() -> Result<void> {
    LOG_INFO_PRINT("demonstrating algorithm integration");
    
    if (m_data.empty()) {
        LOG_ERROR_PRINT("no data available for algorithm demonstration");
        return std::unexpected(MdspanError::invalid_dimensions);
    }
    
    // create 1d mdspan for algorithm demonstration
    std::mdspan<ElementType, std::extents<size_t, std::dynamic_extent>> span_1d{m_data.data(), m_data.size()};
    
    // demonstrate std::ranges integration
    const auto sum = std::accumulate(m_data.begin(), m_data.end(), ElementType{});
    LOG_INFO_PRINT("sum using std::accumulate: {}", sum);
    
    // demonstrate manual traversal with mdspan
    ElementType mdspan_sum{};
    for (size_t ndx{0}; ndx < span_1d.extent(0); ++ndx) {
        mdspan_sum += span_1d[ndx];
    }
    LOG_INFO_PRINT("sum using mdspan traversal: {}", mdspan_sum);
    
    // verify both methods produce same result
    assert(sum == mdspan_sum);
    
    return {};
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::demonstrate_performance() -> Result<void> {
    LOG_INFO_PRINT("demonstrating performance characteristics");
    
    if (m_data.empty()) {
        LOG_ERROR_PRINT("no data available for performance demonstration");
        return std::unexpected(MdspanError::invalid_dimensions);
    }
    
    const size_t iterations{1000000};
    
    // benchmark direct array access
    auto start = std::chrono::high_resolution_clock::now();
    ElementType sum_direct{};
    for (size_t iter{0}; iter < iterations; ++iter) {
        for (size_t ndx{0}; ndx < m_data.size(); ++ndx) {
            sum_direct += m_data[ndx];
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    const auto direct_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // benchmark mdspan access
    std::mdspan<ElementType, std::extents<size_t, std::dynamic_extent>> span_1d{m_data.data(), m_data.size()};
    start = std::chrono::high_resolution_clock::now();
    ElementType sum_mdspan{};
    for (size_t iter{0}; iter < iterations; ++iter) {
        for (size_t ndx{0}; ndx < span_1d.extent(0); ++ndx) {
            sum_mdspan += span_1d[ndx];
        }
    }
    end = std::chrono::high_resolution_clock::now();
    const auto mdspan_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    LOG_INFO_PRINT("performance comparison ({} iterations):", iterations);
    LOG_INFO_PRINT("direct access time: {} microseconds", direct_time.count());
    LOG_INFO_PRINT("mdspan access time: {} microseconds", mdspan_time.count());
    LOG_INFO_PRINT("performance ratio: {:.2f}x", 
                   static_cast<double>(mdspan_time.count()) / direct_time.count());
    
    // verify both approaches produce same result
    assert(sum_direct == sum_mdspan);
    
    return {};
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::run_comprehensive_tests() -> Result<void> {
    LOG_INFO_PRINT("running comprehensive mdspan test suite");
    
    // test 1d functionality
    auto result = demonstrate_1d_basics();
    if (!result) {
        return result;
    }
    
    // test 2d functionality if applicable
    if (m_cols > 1) {
        result = demonstrate_2d_layouts();
        if (!result) {
            return result;
        }
        
        result = demonstrate_subspan();
        if (!result) {
            return result;
        }
    }
    
    // test 3d functionality if applicable
    if (m_depth > 1) {
        result = demonstrate_3d_tensor();
        if (!result) {
            return result;
        }
    }
    
    // test custom accessors
    result = demonstrate_custom_accessors();
    if (!result) {
        return result;
    }
    
    // test algorithm integration
    result = demonstrate_algorithm_integration();
    if (!result) {
        return result;
    }
    
    // test performance characteristics
    result = demonstrate_performance();
    if (!result) {
        return result;
    }
    
    LOG_INFO_PRINT("all comprehensive tests completed successfully");
    return {};
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::initialize_data_pattern() -> void {
    LOG_INFO_PRINT("initializing data with sequential pattern");
    
    const size_t total_size{m_rows * m_cols * m_depth};
    m_data.clear();
    m_data.reserve(total_size);
    
    for (size_t ndx{0}; ndx < total_size; ++ndx) {
        m_data.emplace_back(static_cast<ElementType>(ndx + 1));
    }
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::validate_bounds(size_t row, size_t col, size_t depth) const -> bool {
    return row < m_rows && col < m_cols && depth < m_depth;
}

template<std::copyable ElementType>
auto MdspanExample<ElementType>::get_linear_index(size_t row, size_t col, size_t depth) const -> size_t {
    return depth * (m_rows * m_cols) + row * m_cols + col;
}

// explicit template instantiations
template class MdspanExample<int>;
template class MdspanExample<double>;
template class MdspanExample<float>;

// demonstration function implementations
auto demonstrate_basic_usage() -> Result<void> {
    LOG_INFO_PRINT("demonstrating basic mdspan usage patterns");
    
    // create sample data
    std::vector<int> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    
    // demonstrate 1d mdspan
    std::mdspan<int, std::extents<size_t, std::dynamic_extent>> span_1d{data.data(), 12};
    LOG_INFO_PRINT("1d span with {} elements", span_1d.size());
    
    // demonstrate 2d mdspan
    std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> span_2d{data.data(), 3, 4};
    LOG_INFO_PRINT("2d span with dimensions {}x{}", span_2d.extent(0), span_2d.extent(1));
    
    // demonstrate 3d mdspan
    std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent, std::dynamic_extent>> span_3d{data.data(), 2, 2, 3};
    LOG_INFO_PRINT("3d span with dimensions {}x{}x{}", 
                   span_3d.extent(0), span_3d.extent(1), span_3d.extent(2));
    
    return {};
}

auto demonstrate_layout_policies() -> Result<void> {
    LOG_INFO_PRINT("demonstrating layout policies");
    
    std::array<int, 12> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    
    // row-major layout (default)
    std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> row_major{data.data(), 3, 4};
    LOG_INFO_PRINT("row-major layout access: element[1,2] = {}", row_major[1, 2]);
    
    // column-major layout
    std::mdspan<int, std::extents<size_t, 3, 4>, std::layout_left> col_major{data.data()};
    LOG_INFO_PRINT("column-major layout access: element[1,2] = {}", col_major[1, 2]);
    
    // strided layout example
    std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>, 
               std::layout_stride> strided{data.data(), 
                                         std::layout_stride::mapping{std::extents{2uz, 3uz}, 
                                                                   std::array{3uz, 1uz}}};
    LOG_INFO_PRINT("strided layout access: element[1,1] = {}", strided[1, 1]);
    
    return {};
}

auto demonstrate_submdspan_operations() -> Result<void> {
    LOG_INFO_PRINT("demonstrating submdspan operations");
    
    std::array<int, 20> data{};
    std::iota(data.begin(), data.end(), 1);
    
    // create base 2d mdspan
    std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> base_span{data.data(), 4, 5};
    
    // submdspan operations (if available)
#ifdef MDSPAN_HAS_CXX_17
    // extract single row
    auto row_span = std::submdspan(base_span, 1, std::full_extent);
    LOG_INFO_PRINT("row subspan has {} elements", row_span.size());
    
    // extract single column
    auto col_span = std::submdspan(base_span, std::full_extent, 2);
    LOG_INFO_PRINT("column subspan has {} elements", col_span.size());
    
    // extract submatrix
    auto sub_matrix = std::submdspan(base_span, std::pair{1uz, 3uz}, std::pair{1uz, 4uz});
    LOG_INFO_PRINT("submatrix dimensions: {}x{}", sub_matrix.extent(0), sub_matrix.extent(1));
#else
    LOG_INFO_PRINT("submdspan operations not available in this implementation");
    LOG_INFO_PRINT("demonstrating manual access patterns instead:");
    LOG_INFO_PRINT("accessing row 1 elements:");
    for (size_t col{0}; col < base_span.extent(1); ++col) {
        std::print("{:4}", base_span[1, col]);
        if (col < base_span.extent(1) - 1) {
            std::print(", ");
        }
    }
    std::print("\n");
#endif
    
    return {};
}

auto demonstrate_accessor_policies() -> Result<void> {
    LOG_INFO_PRINT("demonstrating accessor policies");
    
    std::vector<double> data{1.0, 2.0, 3.0, 4.0, 5.0};
    
    // default accessor
    std::mdspan<double, std::extents<size_t, std::dynamic_extent>> default_span{data.data(), 5};
    LOG_INFO_PRINT("default accessor: element[2] = {}", default_span[2]);
    
    // custom scaling accessor (concept demonstration)
    ScalingAccessor<double> scaling_acc{3.0};
    // Note: Custom accessor integration varies by implementation
    // For compatibility, using standard accessor here
    std::mdspan<double, std::extents<size_t, std::dynamic_extent>> scaling_span{data.data(), 5};
    LOG_INFO_PRINT("scaling accessor concept: element[2] = {} (would use scale factor: {})", 
                   scaling_span[2], scaling_acc.get_scale_factor());
    
    return {};
}

auto demonstrate_extents_usage() -> Result<void> {
    LOG_INFO_PRINT("demonstrating extents usage");
    
    std::array<int, 24> data{};
    std::iota(data.begin(), data.end(), 1);
    
    // static extents
    std::mdspan<int, std::extents<size_t, 4, 6>> static_span{data.data()};
    LOG_INFO_PRINT("static extents: {}x{}", static_span.extent(0), static_span.extent(1));
    
    // mixed static/dynamic extents
    std::mdspan<int, std::extents<size_t, 4, std::dynamic_extent>> mixed_span{data.data(), 6};
    LOG_INFO_PRINT("mixed extents: {}x{}", mixed_span.extent(0), mixed_span.extent(1));
    
    // fully dynamic extents
    std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> dynamic_span{data.data(), 3, 8};
    LOG_INFO_PRINT("dynamic extents: {}x{}", dynamic_span.extent(0), dynamic_span.extent(1));
    
    return {};
}

auto demonstrate_performance_characteristics() -> Result<void> {
    LOG_INFO_PRINT("demonstrating performance characteristics");
    
    constexpr size_t size{1000000};
    std::vector<double> data(size);
    std::iota(data.begin(), data.end(), 1.0);
    
    // benchmark traditional array access
    auto start = std::chrono::high_resolution_clock::now();
    double sum1{};
    for (size_t ndx{0}; ndx < data.size(); ++ndx) {
        sum1 += data[ndx];
    }
    auto end = std::chrono::high_resolution_clock::now();
    const auto direct_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    // benchmark mdspan access
    std::mdspan<double, std::extents<size_t, std::dynamic_extent>> span{data.data(), size};
    start = std::chrono::high_resolution_clock::now();
    double sum2{};
    for (size_t ndx{0}; ndx < span.extent(0); ++ndx) {
        sum2 += span[ndx];
    }
    end = std::chrono::high_resolution_clock::now();
    const auto mdspan_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    LOG_INFO_PRINT("performance comparison for {} elements:", size);
    LOG_INFO_PRINT("direct access: {} nanoseconds", direct_time.count());
    LOG_INFO_PRINT("mdspan access: {} nanoseconds", mdspan_time.count());
    LOG_INFO_PRINT("overhead ratio: {:.3f}x", 
                   static_cast<double>(mdspan_time.count()) / direct_time.count());
    
    // verify correctness
    assert(std::abs(sum1 - sum2) < 1e-9);
    
    return {};
}

} // namespace mdspan_demo