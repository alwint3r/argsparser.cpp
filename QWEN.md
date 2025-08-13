## Philosophy

Write code as if the one who will maintain this project after you left is a violent psychopath who knows where you live and how to find you. This means:
- **Clarity is paramount**: Code should be easy to understand, well-documented, and self-explanatory wherever possible.
- **Robustness is key**: Anticipate edge cases and potential failure points. Write defensive code.
- **Maintainability is a must**: Structure code logically, use meaningful names, and avoid unnecessary complexity. Future you (and others) will appreciate it.
- **Simplicity is valued**: Avoid overcomplicating solutions. Prefer straightforward approaches that are easy to reason about and maintain.

## Writing Test

This project does not use any test framework, a basic assertion function should be used instead by including the `<cassert>` header.

### Test Structures

- Each test should be placed in files under the `tests/` directory.
- The test files should be named with the prefix `test_` followed by the name of the unit being tested.
- Each test file should contain a `main` function that runs the tests.
- Each test should be defined as a function with the prefix `test_` followed by the name of the unit being tested.

## Building

- Run `build.sh` to compile and test the project.
- Always run the `build.sh` whenever changes are introduced into the source code and build system.

## Examples

Examples are located in the `examples/` directory.

## C++ Coding Principles for Embedded System (Target) & Simulation (Host)

**Core Principle:** There's no hardware, only data. Focus on data transformations and interactions.

**Library Design Considerations (for external consumers):**

- **Testability & Simulation (Host Environment):**
    - All code, including hardware abstractions, MUST be designed for testability.
    - Enable comprehensive unit and integration testing on the Host (simulation environment) without reliance on physical hardware.
    - Design clear interfaces that allow for mocking or stubbing of hardware-dependent components.
    - Remember: If it's not testable in simulation, it's not ready.
- **Easy Implementation on Real Hardware (Target Environment):**
    - The transition from simulated hardware abstractions to real hardware implementations should be straightforward and require minimal code changes to the core logic.
    - Hardware Abstraction Layers (HALs) should be well-defined and isolate hardware-specific details.
- **Interface Stability (especially for Hardware Abstractions):**
    - For C++ class declarations, particularly those abstracting hardware, modifications to the public interface (methods, signatures, member variables) are STRONGLY DISCOURAGED unless explicitly requested and justified.
    - Prioritize backward compatibility and minimize breaking changes. If a change is unavoidable, it must be clearly documented and communicated.

**General C++ Best Practices (apply to both Target and Host):**

- **Resource Management (RAII for Non-Memory Resources):**
    - **Dynamic Memory Allocation is STRONGLY DISCOURAGED.** Avoid `new`, `delete`, `malloc`, `free`, and standard library containers that use dynamic allocation by default (e.g., `std::vector` without a custom allocator, `std::string` in its typical usage).
    - Utilize RAII (Resource Acquisition Is Initialization) principles for managing system resources other than dynamic memory. This includes file handles, mutexes, hardware device access, timers, or any other resource with a limited lifetime that must be explicitly acquired and released.
    - Create classes that acquire a resource in their constructor and release it in their destructor. This ensures resources are automatically cleaned up when the object goes out of scope, preventing leaks and simplifying code.
    - **Example (Mutex Lock Guard):**
      ```cpp
      // class MutexGuard {
      // public:
      //     explicit MutexGuard(Mutex& m) : mutex_(m) { mutex_.lock(); }
      //     ~MutexGuard() { mutex_.unlock(); }
      // private:
      //     Mutex& mutex_;
      // };
      ```
- **Static Polymorphism Preferred:**
    - Favor static polymorphism (e.g., templates, compile-time polymorphism with concepts if available. Runtime polymorphism can introduce vtable overhead, make behavior harder to predict at compile-time, and potentially complicate testing or increase code size.
    - **Avoid designing public interfaces that *require* consumers to use or implement runtime polymorphism (e.g., inheriting from a base class with pure virtual functions) unless absolutely necessary and thoroughly justified for the specific embedded context and performance/memory constraints.**
- **Error Handling:**
    - Employ robust error handling mechanisms using std::expected from C++23, error codes, status flags, or other deterministic methods in that order depending on the availability.
    - **C++ exceptions MUST be avoided** due to their indeterministic nature, potential for hidden control flow, and performance/memory overhead, all of which are unsuitable for this project.
    - All error states should be handled explicitly and locally where possible. Function return values should clearly indicate success or failure.
- **Const Correctness:** Use `const` extensively to ensure data integrity and clearly define intent.
- **Modern C++ Features (Judiciously):** Leverage modern C++ features (C++11/14/17/20/23 and beyond, as appropriate for the project's compiler and embedded constraints) to write safer and more expressive code. Be mindful of potential code size or performance implications of certain features in constrained environments.
- **Code Style & Formatting:** Adhere to a consistent code style. (Consider adding a reference to a specific style guide or a `.clang-format` file if you have one).
- **Documentation:**
    - Document all public interfaces (classes, methods, functions) clearly.
    - Explain complex logic or algorithms with comments.
    - Keep documentation up-to-date with code changes.
- **Avoid Magic Numbers and Strings:** Use named constants, `enum`, or `enum class` instead.
- **Single Responsibility Principle (SRP):** Each class and function should have one well-defined responsibility.
- **Don't Repeat Yourself (DRY):** Avoid code duplication. Encapsulate common logic into reusable functions or classes.


