# Chapter 1: Folder Structure & Why References Exist

## Engineering Problem

The first goal of Chimera was to build an `Order` struct that represents a single order in the matching engine. My initial instinct was to pass an `Order` object by value because it is simple and straightforward.

However, a real exchange processes millions of orders every second. An `Order` contains information such as the order ID, price, quantity, side, and timestamp. Passing the object by value creates a complete copy every time a function is called. If the order passes through validation, logging, risk management, and matching, multiple unnecessary copies are created. The real engineering problem was learning how to let functions operate on an `Order` efficiently without copying the entire object while still preventing accidental modification of data.

---

## Theory

The primary concept introduced in this chapter was **references**.

A reference (`Order&`) is another name (alias) for an existing object. Unlike pass-by-value, passing by reference does not create a copy, making it much more efficient for large objects.

A `const Order&` provides the same efficiency while guaranteeing that the object cannot be modified. This allows functions to read large objects without copying them and without risking accidental changes.

### Trade-offs

### Pass-by-Value

**Advantages**

- Original object cannot be modified.
- Easy to understand.

**Disadvantages**

- Creates a complete copy.
- Expensive for large objects.

### Pass-by-Reference

**Advantages**

- No copy is created.
- Very efficient.
- Allows modification of the original object.

**Disadvantages**

- Modifies the original object if not used carefully.

### Pass-by-Const-Reference

**Advantages**

- No copy.
- Prevents accidental modification.
- Preferred for read-only access.

### Complexity

| Method | Complexity |
|--------|------------|
| Pass by Value | O(size of object) |
| Pass by Reference | O(1) |
| Pass by Const Reference | O(1) |

---

## Experiment

To understand the difference between pass-by-value and pass-by-reference, I created a simple experiment using a structure containing an integer array.

Before running the program, I predicted that modifying an object passed by value would not affect the original object, while modifying an object passed by reference would change the original data.

After compiling and executing the program, the prediction was correct.

### Code

```cpp
#include <iostream>

struct Big
{
    int data[5] = {1,2,3,4,5};
};

void byValue(Big b)
{
    b.data[0] = 999;
}

void byRef(Big& b)
{
    b.data[0] = 999;
}

int main()
{
    Big x;

    byValue(x);
    std::cout << "After byValue: " << x.data[0] << '\n';

    byRef(x);
    std::cout << "After byRef: " << x.data[0] << '\n';
}
```

### Output

```text
After byValue: 1
After byRef: 999
```

### Explanation

Passing by value creates a copy of the object, so modifications affect only the copy.

Passing by reference allows the function to work directly on the original object, so any modification changes the original data.

This experiment demonstrated why references are preferred in performance-critical systems such as matching engines.

---

## Chimera Design

For this chapter, I organized the Chimera project using a professional C++ folder structure.

```text
chimera/
├── CMakeLists.txt
├── docs/
│   └── chapter01_references.md
├── include/
│   └── chimera/
│       └── order.hpp
├── src/
│   └── order.cpp
├── tests/
│   └── test_order.cpp
└── experiments/
    └── ch01_references.cpp
```

I implemented the `Order` structure containing:

- `id`
- `price`
- `originalQuantity`
- `filledQuantity`
- `side`
- `timeStamp`

I also implemented the helper functions:

- `applyFill()`
- `remainingQuantity()`
- `isFullyFilled()`

Finally, I replaced manual console output with automated tests using `assert()` so that failures are detected automatically.

---

## Bugs Found & Fixed

### Bug 1 – Incorrect Variable Name

**Problem**

The variable was named `origianQuantity`.

**Root Cause**

A spelling mistake during implementation.

**Fix**

Renamed it to `originalQuantity` throughout the project.

---

### Bug 2 – Incorrect Order ID Type

**Problem**

The project design specified `uint64_t`, but the implementation used `uint32_t`.

**Root Cause**

The implementation did not match the original design.

**Fix**

Changed the type to `uint64_t` to maintain consistency across the project.

---

### Bug 3 – Redundant Boolean Expression

**Problem**

```cpp
return true ? (order.filledQuantity == order.originalQuantity) : false;
```

**Root Cause**

An unnecessary ternary operator was used.

**Fix**

Simplified the code to:

```cpp
return order.filledQuantity == order.originalQuantity;
```

---

### Bug 4 – Manual Testing Instead of Assertions

**Problem**

The original tests relied on reading console output manually.

**Root Cause**

Console output cannot automatically detect failures.

**Fix**

Replaced print statements with `assert()`.

The final tests verify:

- Partial fill
- Exact fill
- Rejected overfill

This makes the tests automatically fail whenever the implementation behaves incorrectly.

---

## Lessons Learned

- Always use pass-by-reference for large objects when copying is unnecessary.
- Use `const` references whenever a function only needs to read data.
- Good variable names improve readability and reduce future bugs.
- Keep data types consistent across the entire project.
- Prefer simple, readable code over unnecessarily complex expressions.
- Automated testing with `assert()` is much more reliable than manually checking console output.
- Organizing the project into a clean folder structure makes it easier to maintain and extend as the project grows.