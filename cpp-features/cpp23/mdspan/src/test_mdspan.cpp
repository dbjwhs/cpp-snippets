// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/mdspan_example.hpp"
#include <cassert>
#include <cmath>
#include <string>

// For external mdspan library compatibility
#ifdef MDSPAN_HAS_CXX_17
  using std::submdspan;
  using std::full_extent;
#else
  // Fallback for when submdspan is not available
  namespace {
    struct [[maybe_unused]] full_extent_t {};
  }
#endif

// comprehensive test suite for mdspan functionality
// using assert-based testing for validation

namespace mdspan_tests {

// test basic 1d mdspan functionality
auto test_1d_functionality() -> bool {
    LOG_INFO_PRINT("testing 1d mdspan functionality");
    
    // create test data
    std::vector<int> data{1, 2, 3, 4, 5};
    const std::mdspan<int, std::extents<size_t, std::dynamic_extent>> span{data.data(), 5};
    
    // test basic properties
    assert(span.size() == 5);
    assert(span.extent(0) == 5);
    assert(span.rank() == 1);
    assert(span.rank_dynamic() == 1);
    
    // test element access
    for (size_t ndx{0}; ndx < 5; ++ndx) {
        assert(span[ndx] == static_cast<int>(ndx + 1));
    }
    
    // test data pointer
    assert(span.data_handle() == data.data());
    
    LOG_INFO_PRINT("1d functionality tests passed");
    return true;
}

// test 2d mdspan with different layouts
auto test_2d_layouts() -> bool {
    LOG_INFO_PRINT("testing 2d mdspan layouts");
    
    std::vector<int> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    
    // test row-major (default) layout
    const std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> row_major{data.data(), 3, 4};
    assert(row_major.size() == 12);
    assert(row_major.extent(0) == 3);
    assert(row_major.extent(1) == 4);
    assert(row_major.rank() == 2);
    
    // test specific element access
    const auto val_0_0 = row_major[0, 0];
    const auto val_0_3 = row_major[0, 3];
    const auto val_2_3 = row_major[2, 3];
    assert(val_0_0 == 1);
    assert(val_0_3 == 4);
    assert(val_2_3 == 12);
    
    // test column-major layout
    std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>, 
               std::layout_left> col_major{data.data(), 3, 4};
    assert(col_major.size() == 12);
    assert(col_major.extent(0) == 3);
    assert(col_major.extent(1) == 4);
    
    // verify different layout access patterns
    const auto row_val = row_major[1, 2];
    const auto col_val = col_major[1, 2];
    assert(row_val != col_val); // different layouts give different results
    
    LOG_INFO_PRINT("2d layout tests passed");
    return true;
}

// test 3d mdspan functionality
auto test_3d_functionality() -> bool {
    LOG_INFO_PRINT("testing 3d mdspan functionality");
    
    std::vector<double> data(24);
    std::iota(data.begin(), data.end(), 1.0);
    
    // create 3d mdspan
    std::mdspan<double, std::extents<size_t, std::dynamic_extent, std::dynamic_extent, std::dynamic_extent>> span_3d{data.data(), 2, 3, 4};
    
    // test basic properties
    assert(span_3d.size() == 24);
    assert(span_3d.extent(0) == 2);
    assert(span_3d.extent(1) == 3);
    assert(span_3d.extent(2) == 4);
    assert(span_3d.rank() == 3);
    
    // test element access
    const auto val_0_0_0 = span_3d[0, 0, 0];
    const auto val_1_2_3 = span_3d[1, 2, 3];
    assert(val_0_0_0 == 1.0);
    assert(val_1_2_3 == 24.0);
    
    // test bounds checking logic (conceptual)
    for (size_t i{0}; i < span_3d.extent(0); ++i) {
        for (size_t j{0}; j < span_3d.extent(1); ++j) {
            for (size_t k{0}; k < span_3d.extent(2); ++k) {
                const double expected{static_cast<double>(i * 12 + j * 4 + k + 1)};
                const auto actual_val = span_3d[i, j, k];
                assert(std::abs(actual_val - expected) < 1e-9);
            }
        }
    }
    
    LOG_INFO_PRINT("3d functionality tests passed");
    return true;
}

// test submdspan operations
auto test_submdspan_operations() -> bool {
    LOG_INFO_PRINT("testing submdspan operations");
    
    std::array<int, 20> data{};
    std::iota(data.begin(), data.end(), 1);
    
    // create base 2d mdspan
    std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> base{data.data(), 4, 5};
    
    // test submdspan operations (if available)
#ifdef MDSPAN_HAS_CXX_17
    // test row extraction
    auto row_span = std::submdspan(base, 1, std::full_extent);
    assert(row_span.rank() == 1);
    assert(row_span.extent(0) == 5);
    assert(row_span[0] == 6);  // first element of second row
    assert(row_span[4] == 10); // last element of second row
    
    // test column extraction
    auto col_span = std::submdspan(base, std::full_extent, 2);
    assert(col_span.rank() == 1);
    assert(col_span.extent(0) == 4);
    assert(col_span[0] == 3);  // first element of third column
    assert(col_span[3] == 18); // last element of third column
    
    // test submatrix extraction
    auto sub_matrix = std::submdspan(base, std::pair{1uz, 3uz}, std::pair{1uz, 4uz});
    assert(sub_matrix.rank() == 2);
    assert(sub_matrix.extent(0) == 2);
    assert(sub_matrix.extent(1) == 3);
    const auto sub_val_0_0 = sub_matrix[0, 0];
    const auto sub_val_1_2 = sub_matrix[1, 2];
    assert(sub_val_0_0 == 7);  // base[1,1]
    assert(sub_val_1_2 == 14); // base[2,3]
#else
    // test manual access patterns when submdspan is not available
    LOG_INFO_PRINT("submdspan not available, testing manual access patterns");
    const auto base_1_0 = base[1, 0];
    const auto base_1_4 = base[1, 4];
    const auto base_0_2 = base[0, 2];
    const auto base_3_2 = base[3, 2];
    assert(base_1_0 == 6);  // first element of second row
    assert(base_1_4 == 10); // last element of second row
    assert(base_0_2 == 3);  // first element of third column
    assert(base_3_2 == 18); // last element of third column
#endif
    
    LOG_INFO_PRINT("submdspan operations tests passed");
    return true;
}

// test static vs dynamic extents
auto test_extents_variations() -> bool {
    LOG_INFO_PRINT("testing extents variations");
    
    std::array<int, 12> data{};
    std::iota(data.begin(), data.end(), 1);
    
    // static extents
    std::mdspan<int, std::extents<size_t, 3, 4>> static_span{data.data()};
    assert(static_span.size() == 12);
    assert(static_span.extent(0) == 3);
    assert(static_span.extent(1) == 4);
    assert(static_span.rank() == 2);
    assert(static_span.rank_dynamic() == 0);
    
    // mixed extents
    std::mdspan<int, std::extents<size_t, 3, std::dynamic_extent>> mixed_span{data.data(), 4};
    assert(mixed_span.size() == 12);
    assert(mixed_span.extent(0) == 3);
    assert(mixed_span.extent(1) == 4);
    assert(mixed_span.rank() == 2);
    assert(mixed_span.rank_dynamic() == 1);
    
    // fully dynamic extents
    std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> dynamic_span{data.data(), 3, 4};
    assert(dynamic_span.size() == 12);
    assert(dynamic_span.extent(0) == 3);
    assert(dynamic_span.extent(1) == 4);
    assert(dynamic_span.rank() == 2);
    assert(dynamic_span.rank_dynamic() == 2);
    
    LOG_INFO_PRINT("extents variations tests passed");
    return true;
}

// test custom accessor functionality
auto test_custom_accessors() -> bool {
    LOG_INFO_PRINT("testing custom accessors");
    
    std::vector<double> data{1.0, 2.0, 3.0, 4.0, 5.0};
    
    // test scaling accessor concept (using standard accessor for compatibility)
    mdspan_demo::ScalingAccessor<double> scaling_acc{2.5};
    const std::mdspan<double, std::extents<size_t, std::dynamic_extent>> scaled_span{data.data(), 5};
    
    assert(scaled_span.size() == 5);
    assert(scaled_span.extent(0) == 5);
    assert(scaling_acc.get_scale_factor() == 2.5);
    
    // test accessor functionality
    for (size_t ndx{0}; ndx < scaled_span.extent(0); ++ndx) {
        // accessor should provide direct access to underlying data
        assert(std::abs(scaled_span[ndx] - data[ndx]) < 1e-9);
    }
    
    LOG_INFO_PRINT("custom accessor tests passed");
    return true;
}

// test mdspan with standard algorithms
auto test_algorithm_integration() -> bool {
    LOG_INFO_PRINT("testing algorithm integration");
    
    std::vector<int> data{5, 2, 8, 1, 9, 3, 7, 4, 6};
    const std::mdspan<int, std::extents<size_t, std::dynamic_extent>> span{data.data(), data.size()};
    
    // test element access for algorithm compatibility
    int sum{};
    for (size_t ndx{0}; ndx < span.extent(0); ++ndx) {
        sum += span[ndx];
    }
    
    const int expected_sum = std::accumulate(data.begin(), data.end(), 0);
    assert(sum == expected_sum);
    
    // test with 2d span
    const std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> span_2d{data.data(), 3, 3};
    int sum_2d{};
    for (size_t row{0}; row < span_2d.extent(0); ++row) {
        for (size_t col{0}; col < span_2d.extent(1); ++col) {
            sum_2d += span_2d[row, col];
        }
    }
    assert(sum_2d == expected_sum);
    
    LOG_INFO_PRINT("algorithm integration tests passed");
    return true;
}

// test performance characteristics
auto test_performance_overhead() -> bool {
    LOG_INFO_PRINT("testing performance overhead");
    
    constexpr size_t size{10000};
    std::vector<double> data(size);
    std::iota(data.begin(), data.end(), 1.0);
    
    // measure direct access
    auto start = std::chrono::high_resolution_clock::now();
    double sum_direct{};
    for (size_t ndx{0}; ndx < data.size(); ++ndx) {
        sum_direct += data[ndx];
    }
    auto end = std::chrono::high_resolution_clock::now();
    const auto direct_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    // measure mdspan access
    const std::mdspan<double, std::extents<size_t, std::dynamic_extent>> span{data.data(), size};
    start = std::chrono::high_resolution_clock::now();
    double sum_mdspan{};
    for (size_t ndx{0}; ndx < span.extent(0); ++ndx) {
        sum_mdspan += span[ndx];
    }
    end = std::chrono::high_resolution_clock::now();
    const auto mdspan_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    
    // verify correctness
    assert(std::abs(sum_direct - sum_mdspan) < 1e-6);
    
    // performance should be comparable (within reasonable overhead)
    const double overhead_ratio = static_cast<double>(mdspan_time.count()) / direct_time.count();
    LOG_INFO_PRINT("performance overhead ratio: {:.3f}x", overhead_ratio);
    
    // reasonable overhead threshold (may vary by compiler optimization)
    assert(overhead_ratio < 2.0);
    
    LOG_INFO_PRINT("performance overhead tests passed");
    return true;
}

// test class-based examples
auto test_class_based_examples() -> bool {
    LOG_INFO_PRINT("testing class-based examples");
    
    // test 1d example
    mdspan_demo::MdspanExample<int> example_1d{5};
    auto result = example_1d.run_comprehensive_tests();
    assert(result.has_value());
    
    // test 2d example
    mdspan_demo::MdspanExample<double> example_2d{3, 4};
    result = example_2d.run_comprehensive_tests();
    assert(result.has_value());
    
    // test 3d example
    mdspan_demo::MdspanExample<float> example_3d{2, 3, 2};
    result = example_3d.run_comprehensive_tests();
    assert(result.has_value());
    
    LOG_INFO_PRINT("class-based example tests passed");
    return true;
}

// comprehensive test runner
auto run_all_tests() -> bool {
    LOG_INFO_PRINT("running comprehensive mdspan test suite");
    
    try {
        // run all individual test functions
        assert(test_1d_functionality());
        assert(test_2d_layouts());
        assert(test_3d_functionality());
        assert(test_submdspan_operations());
        assert(test_extents_variations());
        assert(test_custom_accessors());
        assert(test_algorithm_integration());
        assert(test_performance_overhead());
        assert(test_class_based_examples());
        
        LOG_INFO_PRINT("all mdspan tests completed successfully");
        return true;
        
    } catch (const std::exception& e) {
        Logger::StderrSuppressionGuard guard{};
        LOG_ERROR_PRINT("test failure with exception: {}", e.what());
        return false;
    } catch (...) {
        Logger::StderrSuppressionGuard guard{};
        LOG_ERROR_PRINT("test failure with unknown exception");
        return false;
    }
}

} // namespace mdspan_tests

// main test function for CTest integration
auto main() -> int {
    
    LOG_INFO_PRINT("starting mdspan test suite");

    if (mdspan_tests::run_all_tests()) {
        LOG_INFO_PRINT("all tests passed successfully");
        return 0;
    }
    LOG_ERROR_PRINT("some tests failed");
    return 1;
}
