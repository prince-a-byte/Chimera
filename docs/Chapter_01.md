# Chapter 1 --- Folder Structure & Why References Exist

### *Building a Professional C++ Foundation for Chimera*

> **Project:** Chimera --- Building a Low-Latency Exchange in Modern C++

------------------------------------------------------------------------

# 🎯 Learning Objectives

By the end of this chapter you will be able to:

-   Create a professional C++ project structure.
-   Explain why large C++ projects separate headers and source files.
-   Explain why references exist.
-   Differentiate pass-by-value from pass-by-reference.
-   Understand why `const T&` is the default choice for read-only APIs.
-   Predict the behavior of reference-based code before running it.
-   Apply references in Chimera's `Order` API.

------------------------------------------------------------------------

# ⚙️ Engineering Problem

Imagine you are writing the first component of a matching engine.

Your first instinct might be:

``` cpp
void processOrder(Order order)
{
    // process order
}
```

Nothing looks wrong.

Now imagine that `Order` stores:

-   Order ID
-   Client ID
-   Side
-   Price
-   Quantity
-   Timestamp
-   Exchange metadata

Every function call copies the entire object.

Now picture the order flowing through the engine:

``` text
Incoming Order
      │
      ▼
 Validation
      │
      ▼
 Risk Check
      │
      ▼
 Logging
      │
      ▼
 Matching Engine
      │
      ▼
 Persistence
```

If every stage copies the order, you've introduced multiple unnecessary
memory copies before executing any real business logic.

High-frequency trading systems process millions of orders every second.
Tiny costs repeated millions of times become measurable latency.

> **Engineering Question**
>
> How can a function operate on an object without copying it while still
> preventing accidental modification?

------------------------------------------------------------------------

# 📁 Project Setup

Create the following structure before writing any code.

``` text
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

## Why not put everything in one folder?

  -----------------------------------------------------------------------
  Folder                            Purpose
  --------------------------------- -------------------------------------
  `include/chimera/`                Public headers. Prevents filename
                                    collisions (`chimera/order.hpp`).

  `src/`                            Implementation files (`.cpp`). Keeps
                                    interface separate from
                                    implementation.

  `tests/`                          Automated tests. Detect regressions
                                    immediately.

  `experiments/`                    Safe sandbox for learning. Never
                                    affects production code.

  `docs/`                           Engineering notebook documenting what
                                    you learned and why.
  -----------------------------------------------------------------------

### ✅ Implementation Task

Create every folder and file above.

Empty files are perfectly fine.

**Do not continue until the project skeleton exists.**

------------------------------------------------------------------------

# 📖 Theory

## What is a Reference?

A reference is another name (an **alias**) for an existing object.

``` cpp
Order order;
Order& ref = order;
```

No copy is created.

Both names refer to exactly the same object.

    Memory

    +--------------------+
    | Order              |
    | quantity = 100     |
    | price = 250.50     |
    +--------------------+
          ▲
          │
     order     ref

Changing `ref` changes `order`.

------------------------------------------------------------------------

## Properties of References

✅ Must refer to an existing object.

✅ Cannot be null.

✅ Cannot be reseated.

✅ No object copy is created.

------------------------------------------------------------------------

## `const Order&`

``` cpp
void validate(const Order& order);
```

Meaning:

> "Borrow this object, but I promise not to modify it."

Benefits:

-   No copy.
-   Compiler guarantees read-only access.
-   Clear API intent.

------------------------------------------------------------------------

## Complexity

  Method              Complexity          Copies?
  ------------------- ------------------- ---------
  Pass by Value       O(size of object)   ✅ Yes
  Pass by Reference   O(1)                ❌ No

Regardless of how large `Order` becomes, a reference remains
constant-time to pass.

------------------------------------------------------------------------

# 🧠 Internal Working

Although references feel magical, the compiler implements them similarly
to an automatically dereferenced pointer.

Conceptually:

``` cpp
Order& ref = order;
```

behaves like

``` text
hiddenPointer ---> order
```

When you write:

``` cpp
ref.quantity = 50;
```

the compiler effectively performs:

``` text
(*hiddenPointer).quantity = 50;
```

The syntax is cleaner, but the underlying idea is simply accessing the
original object.

------------------------------------------------------------------------

# 🧪 Experiment

Before compiling anything:

## Step 1

Predict the output on paper.

``` cpp
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
    std::cout << "After byValue: "
              << x.data[0]
              << "\n";

    byRef(x);
    std::cout << "After byRef: "
              << x.data[0]
              << "\n";
}
```

## Step 2

Compile and run.

``` bash
g++ -std=c++20 experiments/ch01_references.cpp -o ch01
./ch01
```

## Step 3

Write down:

-   Your prediction
-   Actual output
-   Why they differ
-   Draw the memory before and after each function call

> Do **not** skip the prediction. Thinking before compiling is one of
> the fastest ways to improve as an engineer.

------------------------------------------------------------------------

# 🏭 Production Discussion

🚧 **Intentionally postponed.**

Complete the experiment first.

Only after you explain *why* the output changed will we discuss how real
matching engines use references to eliminate unnecessary copies while
preserving correctness.

------------------------------------------------------------------------

# 🐉 Chimera Design

This chapter prepares us to write functions like:

``` cpp
int remainingQuantity(const Order&);
bool isFullyFilled(const Order&);
void applyFill(Order&, int);
```

Notice the design:

-   Reading uses `const Order&`.
-   Modifying uses `Order&`.

This distinction will become a core design principle throughout Chimera.

------------------------------------------------------------------------

# 💻 Implementation

Complete the following:

-   [ ] Create the project structure.
-   [ ] Create all empty files.
-   [ ] Create `experiments/ch01_references.cpp`.
-   [ ] Run the experiment.
-   [ ] Record your observations in `docs/chapter01_references.md`.

------------------------------------------------------------------------

# 🔍 Code Review Checklist

Can you answer these?

-   Why is passing `Order` by value expensive?
-   What is a reference?
-   What is the difference between a pointer and a reference?
-   Why use `const Order&`?
-   When should you use a non-const reference?

------------------------------------------------------------------------

# ⚠️ Common Mistakes

-   Passing large objects by value unnecessarily.
-   Thinking references create copies.
-   Assuming `const` creates a copy.
-   Forgetting that modifying a reference modifies the original object.

------------------------------------------------------------------------

# 🚀 Performance Notes

Performance is rarely lost because of one huge mistake.

It is usually lost because of thousands of tiny inefficiencies repeated
millions of times.

Avoiding unnecessary object copies is one of the simplest and most
effective optimizations in systems programming.

------------------------------------------------------------------------

# 🏋️ Exercises

1.  Increase the array size to 10,000 elements and rerun the experiment.
2.  Print the address of `x` inside `main()` and inside `byRef()`.
3.  Explain why the addresses are identical.
4.  Replace `Big&` with `const Big&`. What changes?
5.  Explain, in your own words, why references are essential in a
    matching engine.

------------------------------------------------------------------------

# 📌 Summary

-   References are aliases, not copies.
-   `const T&` provides efficient read-only access.
-   `T&` allows controlled modification.
-   Separating headers, sources, tests, docs, and experiments creates
    maintainable projects.
-   Understanding references is the first step toward writing
    low-latency C++ systems.

------------------------------------------------------------------------

# 🌱 Git Commit

``` bash
git add .

git commit -m "Chapter 1: Project structure and C++ references"
```

------------------------------------------------------------------------

# ⏭️ What's Next

In this chapter you learned how to borrow data efficiently.

Next, we'll answer an equally important question:

> **How can the compiler stop us from accidentally modifying borrowed
> data?**

The answer is **const correctness**, one of the most powerful design
principles in modern C++.
