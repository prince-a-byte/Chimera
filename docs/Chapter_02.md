# Chapter 2: std::map and Why the Order Book Needs a Sorted Container

## Engineering Problem

In the previous chapter, Chimera only handled a single `Order`. However, a real exchange never deals with just one order. At any moment, thousands of buy and sell orders are resting in the order book, waiting to be matched. Every incoming order must quickly determine whether it can trade immediately or should be stored for future matching.

The most important question the matching engine asks is:

- What is the highest bid (best buy price)?
- What is the lowest ask (best sell price)?

A naive solution would be to store all orders inside a `std::vector` and scan the entire container every time a new order arrives. While simple, this approach requires checking every order to find the best price, making the lookup slower as the order book grows. Since every incoming order performs this lookup, an exchange built this way becomes slower under heavier trading activity, which is exactly the opposite of the desired behavior.

To solve this problem efficiently, the order book needs a data structure that supports fast insertion while always keeping prices sorted.

---

## Theory

The C++ Standard Library provides `std::map`, which stores key-value pairs in sorted order. Internally, `std::map` is implemented as a Red-Black Tree, a self-balancing binary search tree. The balancing algorithm guarantees that the tree remains approximately balanced after every insertion and deletion, allowing operations to remain logarithmic in complexity.

For an order book, the key is the price and the value is a `PriceLevel`, which stores all orders resting at that price.

```cpp
std::map<int64_t, PriceLevel> bids_;
std::map<int64_t, PriceLevel> ask_;
```

Important properties of `std::map`:

- Insert a new price level: **O(log n)**
- Find a price level: **O(log n)**
- Remove a price level: **O(log n)**
- Iterate through prices in sorted order: **O(n)**

### Comparison with Other Containers

### std::vector

Advantages:

- O(1) random access by index.
- Excellent cache locality.

Disadvantages:

- Finding the highest or lowest price requires scanning the entire container.
- Keeping prices sorted requires shifting elements after insertion.

For an order book, this leads to poor scalability.

---

### std::unordered_map

Advantages:

- Average O(1) insertion and lookup.

Disadvantages:

- Keys have no ordering.
- Cannot efficiently determine the highest or lowest price.
- Requires scanning every element to find the best bid or ask.

While `unordered_map` is excellent for looking up orders by ID, it is unsuitable for maintaining sorted market prices.

---

### std::map

Advantages:

- Automatically maintains sorted prices.
- Efficient insertion and lookup.
- Immediate access to the lowest and highest prices using iterators.

Trade-offs:

- Slightly slower insertion than a hash table.
- More memory overhead due to tree nodes.

For an order book, maintaining sorted prices is far more important than achieving average O(1) lookup.

---

## Experiment

Before integrating `std::map` into Chimera, I created a small experiment to understand how it behaves.

I inserted prices in the following order:

```text
105
101
110
103
```

Before running the program, I expected one of two possibilities:

1. The map would preserve insertion order.
2. The map would automatically sort the prices.

After compiling and executing the program, the output was:

```text
101
103
105
110
```

This confirmed that `std::map` always keeps its keys sorted regardless of insertion order.

I also observed:

```cpp
prices.begin()->first
```

returned the smallest price.

```cpp
prices.rbegin()->first
```

returned the largest price.

This behavior makes `std::map` a natural fit for an order book because the best ask can always be found at the beginning of the map, while the best bid can be obtained from the reverse beginning iterator.

One additional lesson from the experiment was understanding the behavior of `operator[]`. Accessing a missing key automatically inserts a new element into the map. Because this operation modifies the container, `operator[]` cannot be used inside a `const` member function. Instead, `find()` must be used when only reading data.

---

## Chimera Design

The OrderBook was designed around the concept of price levels rather than individual orders.

```cpp
std::map<int64_t, PriceLevel> bids_;
std::map<int64_t, PriceLevel> ask_;
```

Each map key represents a unique market price.

Each `PriceLevel` stores all resting orders at that price.

```cpp
struct PriceLevel
{
    int64_t price;
    std::list<Order> orders;
};
```

The order book exposes four primary operations:

- `addBid()`
- `addAsk()`
- `bestBid()`
- `bestAsk()`

A bid is inserted into the bid map, while an ask is inserted into the ask map. Since both maps remain sorted automatically, retrieving the highest bid or lowest ask requires only accessing the appropriate iterator instead of scanning every order.

This design also prepares the project for future chapters, where each price level will act as a FIFO queue during order matching.

---

## Bugs Found & Fixed

### Bug 1 — Swapped bestBid() and bestAsk()

**Problem**

The first implementation accidentally searched the wrong container.

`bestBid()` read from the ask map.

`bestAsk()` read from the bid map.

As a result, the reported best prices were incorrect.

**Root Cause**

The two maps had similar implementations, leading to an accidental mix-up during coding.

**Fix**

Each function was updated to access the correct container.

```cpp
bestBid()  -> bids_
bestAsk()  -> ask_
```

---

### Bug 2 — Copy Instead of Reference

**Problem**

The first implementation used:

```cpp
auto level = bids_[price];
```

This copied the `PriceLevel` stored inside the map.

Any modifications were applied only to the temporary copy.

After the function returned, the copy was destroyed and the map remained unchanged.

**Root Cause**

I forgot that `auto` creates a copy unless a reference is explicitly requested.

**Fix**

The implementation was changed to:

```cpp
auto& level = bids_[price];
```

The reference points directly to the object inside the map, allowing modifications to update the actual stored price level.

This bug directly reinforced the concept learned in Chapter 1 about the difference between copies and references.

---

### Bug 3 — Overwriting Instead of Appending

**Problem**

The initial implementation inserted orders using:

```cpp
level.orders = {order};
```

Every new order replaced the existing order list.

If two orders arrived at the same price, the older order disappeared from the book.

**Root Cause**

I mistakenly assigned a new list instead of adding to the existing list.

**Fix**

The implementation now uses:

```cpp
level.orders.push_back(order);
```

This appends the new order to the end of the existing FIFO queue while preserving all previously resting orders.

---

## Lessons Learned

This chapter taught me that selecting the correct data structure is just as important as writing correct algorithms. A `std::vector` or `std::unordered_map` may appear simpler, but neither provides the ordered behavior required by an exchange.

I also learned that small implementation details can create serious bugs. Forgetting a reference caused updates to disappear silently, while replacing a container instead of appending removed existing orders without any compiler error. These mistakes emphasized the importance of understanding C++ value semantics and container operations instead of relying on intuition.

Finally, I learned that good tests should verify internal behavior rather than only checking visible outputs. Simply confirming the best bid was correct would not detect an overwritten order list. Future tests should be designed to catch these kinds of subtle bugs before they reach production.

By the end of this chapter, I understand why modern matching engines organize orders into sorted price levels, why `std::map` is an appropriate first implementation, and how references, container operations, and careful testing all work together to build a reliable order book.