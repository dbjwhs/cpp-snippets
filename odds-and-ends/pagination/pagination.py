#!/usr/bin/env python3

def paginate(current_page, total_pages, max_pages):
    """
    Generate pagination string with smart ellipsis placement.

    Args:
        current_page: The current active page (will be shown in brackets)
        total_pages: Total number of pages
        max_pages: Maximum number of page numbers to display

    Returns:
        A string representation of the pagination
    """
    # Ensure parameters are valid
    current_page = max(1, min(current_page, total_pages))
    total_pages = max(1, total_pages)
    max_pages = max(3, max_pages)  # Minimum 3 to ensure we can show at least first, current, last

    # For very few pages, just show all pages
    if total_pages <= max_pages:
        result = []
        for i in range(1, total_pages + 1):
            if i == current_page:
                result.append(f"[{i}]")
            else:
                result.append(str(i))
        return " ".join(result)

    # Determine how many pages to show around the current page
    # We need to distribute max_pages - 2 pages (excluding first and last)
    remaining_pages = max_pages - 2
    pages_to_show = []

    # First, determine if we need one or two ellipses
    need_left_ellipsis = (current_page > remaining_pages // 2 + 2)
    need_right_ellipsis = (current_page < total_pages - remaining_pages // 2 - 1)

    # Add first page to the list
    pages_to_show.append(1)

    # Determine start and end page numbers to show in the middle section
    if need_left_ellipsis and need_right_ellipsis:
        # Need both ellipses, current page is in the middle
        # Distribute remaining slots evenly on both sides of current page
        side_pages = (remaining_pages - 1) // 2  # -1 for current page
        start_page = max(2, current_page - side_pages)
        end_page = min(total_pages - 1, current_page + side_pages)

        # Adjust if we have an even number of pages to show
        if remaining_pages % 2 == 0:
            # We need to add one more page - decide which side
            if current_page - start_page < end_page - current_page:
                # More room on the right side
                end_page += 1
            else:
                # More room on the left side or equal
                start_page -= 1
    elif need_left_ellipsis:
        # Only need left ellipsis, current page is near the end
        start_page = total_pages - remaining_pages
        end_page = total_pages - 1
    elif need_right_ellipsis:
        # Only need right ellipsis, current page is near the beginning
        start_page = 2
        end_page = remaining_pages + 1
    else:
        # Should never get here with our constraints
        start_page = 2
        end_page = total_pages - 1

    # Add left ellipsis if needed
    if need_left_ellipsis:
        pages_to_show.append(-1)  # -1 represents ellipsis

    # Add middle pages
    for i in range(start_page, end_page + 1):
        pages_to_show.append(i)

    # Add right ellipsis if needed
    if need_right_ellipsis:
        pages_to_show.append(-1)  # -1 represents ellipsis

    # Add last page if not already included
    if total_pages > 1:
        pages_to_show.append(total_pages)

    # Convert to string
    result = []
    for i, page in enumerate(pages_to_show):
        if page == -1:
            # Ellipsis
            result.append("...")
        elif page == current_page:
            # Current page with brackets
            result.append(f"[{page}]")
        else:
            # Regular page
            result.append(str(page))

    return " ".join(result)


def test_pagination_with_examples():
    """Test the pagination functionality with the provided examples."""
    print("Testing pagination with provided examples")

    # Test case 1: all pages fit
    expected = "[1] 2 3 4 5 6 7 8 9 10 11"
    result = paginate(1, 11, 11)
    print(f"Example 1: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case 2: first page, right ellipsis
    expected = "[1] 2 3 4 5 6 7 8 9 10 ... 30"
    result = paginate(1, 30, 11)
    print(f"Example 2: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case 3: all pages fit, smaller dataset
    expected = "1 2 3 4 5 [6] 7 8 9 10"
    result = paginate(6, 10, 11)
    print(f"Example 3: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case 4: middle pages, right ellipsis
    expected = "1 2 3 4 5 [6] 7 8 9 10 ... 30"
    result = paginate(6, 30, 11)
    print(f"Example 4: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case 5: both ellipses
    expected = "1 ... 3 4 5 6 [7] 8 9 10 11 ... 30"
    result = paginate(7, 30, 11)
    print(f"Example 5: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case 6: both ellipses, later in a list
    expected = "1 ... 20 21 22 23 [24] 25 26 27 28 ... 30"
    result = paginate(24, 30, 11)
    print(f"Example 6: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case 7: left ellipsis, near end
    expected = "1 ... 21 22 23 24 25 26 [27] 28 29 30"
    result = paginate(27, 30, 11)
    print(f"Example 7: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case 8: left ellipsis, last page
    expected = "1 ... 21 22 23 24 25 26 27 28 29 [30]"
    result = paginate(30, 30, 11)
    print(f"Example 8: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case 9: even number of max pages
    expected = "[1] 2 3 4 5 6 7 8 9 ... 30"
    result = paginate(1, 30, 10)
    print(f"Example 9: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case 10: even number of max pages, last page
    expected = "1 ... 22 23 24 25 26 27 28 29 [30]"
    result = paginate(30, 30, 10)
    print(f"Example 10: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    print("All examples passed!")


def test_additional_cases():
    """Test additional edge cases and variations."""
    print("Testing additional edge cases")

    # Test case: very small total pages
    expected = "[1] 2 3"
    result = paginate(1, 3, 11)
    print(f"Small dataset: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case: single page
    expected = "[1]"
    result = paginate(1, 1, 5)
    print(f"Single page: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case: current page out of bounds (too high)
    expected = "1 ... 21 22 23 24 25 26 27 28 29 [30]"
    result = paginate(35, 30, 11)  # Should correct to page 30
    print(f"Current page too high: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case: current page out of bounds (too low)
    expected = "[1] 2 3 4 5 6 7 8 9 10 ... 30"
    result = paginate(0, 30, 11)  # Should correct to page 1
    print(f"Current page too low: Expected: '{expected}', Got: '{result}'")
    assert result == expected

    # Test case: very small max pages
    expected = "1 ... [5] ... 30"
    result = paginate(5, 30, 3)
    print(f"Small max pages: '{result}'")
    assert result == expected

    # Test case: large number of pages
    expected = "1 ... 46 47 48 49 [50] 51 52 53 54 ... 1000"
    result = paginate(50, 1000, 11)
    print(f"Large dataset: '{result}'")
    assert result == expected

    print("All additional tests completed!")


if __name__ == "__main__":
    test_pagination_with_examples()
    test_additional_cases()