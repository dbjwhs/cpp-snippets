# Hoare Logic Implementation in C++

Hoare Logic is a formal system for reasoning about the correctness of computer programs, developed by British computer scientist
Sir Charles Antony Richard Hoare (C.A.R. Hoare) in 1969. It provides a logical framework to verify that programs perform as
expected through mathematical proofs. The fundamental concept in Hoare Logic is the Hoare triple, written as `{P} C {Q}`, where
P is a precondition (assertions that must be true before execution), C is a command or program segment, and Q is a postcondition
(assertions that will be true after execution). This formal verification approach enables mathematically proving program
correctness rather than relying solely on testing.

Hoare Logic addresses several critical problems in software development. It provides a systematic method for ensuring program
correctness, particularly valuable in safety-critical systems where bugs can lead to catastrophic consequences. It helps
developers think about program specifications rigorously before implementation, reducing logical errors. The formal verification
approach catches edge cases that might be missed in traditional testing. Additionally, Hoare Logic has influenced programming
language design, type systems, and automated verification tools, contributing to more reliable software development practices
across the industry.

## Implementation Overview

This C++ implementation demonstrates Hoare Logic with classes representing:

- `ProgramState`: Tracks variable values during execution
- `Assertion`: Base class for logical assertions (preconditions and postconditions)
- `Command`: Base class for program statements
- `HoareTriple`: Encapsulates the `{P} C {Q}` relation

The implementation includes:

- Assertion types: constant, variable equality, and conjunction (AND)
- Command types: assignment and sequence
- Testing framework to verify triple validity

## Usage Examples

Basic usage involves creating assertions, commands, and verifying that Hoare triples hold:

```cpp
// Create a program state with variable x = 5
ProgramState state;
state.setVariable("x", 5);

// Create a Hoare triple: {x == 5} x = 6 {x == 6}
auto precondition = std::make_shared<VariableEqualityAssertion>("x", 5);
auto command = std::make_shared<AssignmentCommand>("x", 6);
auto postcondition = std::make_shared<VariableEqualityAssertion>("x", 6);
HoareTriple triple(precondition, command, postcondition);

// Verify the triple holds after execution
bool result = triple.executeAndVerify(state);
```

## Common Applications

- **Safety-critical systems**: Aerospace, medical devices, automotive control systems
- **Security protocols**: Cryptographic algorithms, access control mechanisms
- **Smart contracts**: Blockchain applications where bugs can have severe financial consequences
- **Compiler correctness**: Ensuring code transformations preserve semantics

## Best Practices

- Write clear, precise preconditions and postconditions
- Use weakest preconditions for optimal verification
- Start with small, well-defined functions before tackling larger programs
- Combine with other verification techniques like model checking for complete coverage

## Limitations

- Manual proof construction can be tedious for complex programs
- State explosion with large programs
- Difficulty handling certain language features (pointers, exceptions)

## Further Reading

1. Hoare, C.A.R. (1969). "An Axiomatic Basis for Computer Programming". Communications of the ACM.
2. Dijkstra, E.W. (1976). "A Discipline of Programming". Prentice Hall.
3. Gries, D. (1981). "The Science of Programming". Springer-Verlag.
4. Apt, K.R. & Olderog, E.R. (2019). "Verification of Sequential and Concurrent Programs". Springer.

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
