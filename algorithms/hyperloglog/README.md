# Simple HyperLogLog Implementation

## Overview

This project implements a simplified version of the HyperLogLog algorithm, a probabilistic data structure designed for
efficiently estimating the number of unique elements in a large dataset.

## Note

This is a simplified implementation, just me messing around with understanding the basics of this. For production use,
consider battle-tested libraries or implementations. e.g. don't even start here unless you never have heard of hyperloglog.

### What is HyperLogLog?

HyperLogLog is an ingenious algorithm that provides an approximate count of unique elements using a fixed amount of
memory. It trades a small amount of accuracy for extraordinary space efficiency, making it ideal for working with
massive datasets.

## Key Features

- **Space Efficiency**: Uses a constant amount of memory regardless of input size
- **Fast Operations**: O(1) time complexity for adding elements and estimating count
- **Probabilistic Estimation**: Provides an approximate count of unique elements

## How It Works

The algorithm works by:
1. Hashing each input element
2. Tracking the maximum number of leading zeros in the hash
3. Using a probabilistic estimation formula to calculate unique element count

### Analogy

Think of HyperLogLog like a marine biologist estimating fish populations by sampling, rather than counting every single fish in the ocean.

## Example Usage

```cpp
SimpleHyperLogLog hll;

// Add elements
hll.add("apple");
hll.add("banana");
hll.add("apple");  // Duplicate

// Estimate unique count
int uniqueCount = hll.uniqueCount();
std::cout << "Estimated unique elements: " << uniqueCount << std::endl;
```

## Performance Characteristics

- **Memory Usage**: Constant memory (default 16 registers)
- **Time Complexity**:
    - Add: O(1)
    - Unique Count Estimation: O(1)
- **Accuracy**: Typically within 2% of actual unique count

## Limitations

- Provides an approximation, not an exact count
- Accuracy depends on the number of registers
- Not suitable for scenarios requiring 100% precise counting

## Testing

The implementation includes a basic test function that:
- Adds elements with duplicates
- Compares actual unique count with HyperLogLog estimate

## Potential Improvements

- Implement more sophisticated error correction
- Add configurable register count
- Enhance hash function
- Implement template-based element type support

## Use Cases

- Website unique visitor tracking
- Big data analytics
- Streaming data analysis
- Database query optimization

## References

- Original HyperLogLog Paper: ["HyperLogLog in Near Constant Space"](https://hal.science/hal-00406166/document) by Philippe Flajolet, Éric Fusy, Olivier Gandouet, and Frédéric Meunier
- Redis HyperLogLog Implementation: [Redis HyperLogLog Documentation](https://redis.io/docs/data-types/hyperloglogs/)
- Additional Reading: [Google's Implementation Details](https://research.google/pubs/pub40671/)

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
