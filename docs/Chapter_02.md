# Chapter 2 --- `std::map` and Why the Order Book Needs a Sorted Container

### *Choosing the Right Data Structure for Price Discovery*

> **Project:** Chimera --- Building a Low-Latency Exchange in Modern C++

------------------------------------------------------------------------

Good --- you got the right result, and your instinct is close, but let's
sharpen it because this distinction matters a lot later when we compare
references vs pointers.

Small correction: it's not quite "we're changing the value at that
particular memory address" --- because even in byValue, b has a memory
address too (its own, on the stack). The real reason is:

byValue(Big b) --- a brand new Big object is constructed, and its bytes
are copied from x. Inside the function, b and x are two completely
separate objects living at two different addresses. Modifying b cannot
touch x. byRef(Big& b) --- no new object is constructed. b is not a
copy, it's another name for x itself. There is only one object, one
address, one set of bytes. Modifying b is modifying x, because they are
the same object.

That's the core mental model to keep: pass-by-value = two objects.
Pass-by-reference = one object, two names.

STEP 4: Production Discussion

At firms like Jane Street or Citadel, this isn't academic --- the "hot
path" (the exact sequence of function calls between receiving a packet
and generating a trade) is measured in nanoseconds, and every
unnecessary copy is a candidate for deletion during profiling. The
convention you'll see in real low-latency codebases:

Small trivial types (an int, a double, a Price if it's just a wrapped
int64_t) --- pass by value. Copying 8 bytes is cheaper than the
indirection of a reference in some cases, and simpler to reason about.
Anything larger, or anything you want to guarantee isn't copied (like
our Order) --- pass by const& if read-only, by & if the function needs
to mutate it.

There's no universal "always use references" rule --- it's a judgment
call based on size and intent. That's the trade-off discussion; there's
no single correct answer, just informed defaults.

STEP 5: Chimera Design

Now let's decide where this applies to Order.

Question for you before I give the task: an Order will need fields like
an ID, price, quantity, side (buy/sell), and timestamp. When we later
write functions like validateOrder(...), logOrder(...), matchOrder(...)
--- which of these do you think should take const Order&, and which (if
any) might need plain Order&? Give me your reasoning, even if you're not
fully sure --- I want to see your judgment before I confirm it.

Scrutinized code for typos, type mismatches, and inadequate test
assertions Scrutinized code for typos, type mismatches, and inadequate
test assertions

Nice work --- the core logic is actually correct (I traced through your
test by hand and it produces the right guard behavior). But this is a
real PR review, and there are things I would not approve yet. Let's go
through it like a senior engineer would on your team.

Correctness ✅

Your applyFill guard is right:

cpp if(order.origianQuantity \>= (order.filledQuantity + fillQuantity))

This correctly rejects the overfill. I traced your test manually: 75
filled → 25 remaining → +25 fills to exactly 100 → +25 more gets
rejected, staying at 100. Logic holds.

Issues I'd flag before merging

1.  Naming --- origianQuantity This is a typo (origian instead of
    original) but I'm not flagging it as "oops fix your spelling" ---
    I'm flagging it because naming is API design. Anyone reading this
    struct six months from now, or a new hire joining your team, will
    trip on it, maybe even copy the typo into new code out of
    consistency. Fix it everywhere: header, .cpp, tests.

2.  id type mismatch We designed Order.id as uint64_t in STEP 5. You
    implemented uint32_t id. This isn't just cosmetic --- if some other
    part of Chimera (an OrderIndex map, a network protocol field)
    expects uint64_t and you pass this uint32_t, you get implicit
    narrowing/widening conversions scattered everywhere, which is
    exactly how subtle bugs creep into real systems. Pick one width and
    be consistent across the whole codebase from day one.

3.  isFullyFilled --- redundant ternary

cpp return true ? (order.filledQuantity == order.origianQuantity) :
false;

true ? X : false is always just X. This is a code smell --- it signals
either uncertainty about boolean expressions, or leftover code from an
earlier iteration you forgot to clean up. A senior reviewer would bounce
this back immediately. It should simply be:

cpp return order.filledQuantity == order.originalQuantity;

4.  Your "tests" aren't tests --- they're console output you have to
    read yourself

This is the most important issue. Look at your test_order.cpp: it prints
numbers, and you are the one who has to look at the output and judge
"yep, that's correct." That doesn't scale, and it's not how professional
engineering works. Six months from now, if someone refactors applyFill
and breaks the overfill guard, nobody is going to manually re-read
console output --- they need the test itself to fail loudly and
automatically.

The acceptance criteria said "test cases," not "print statements." Real
test cases assert an expected value and crash/report failure if it's
wrong:

cpp #include `<cassert>`{=html} // ... applyFill(order, 75);
assert(order.filledQuantity == 75); assert(remainingQuantity(order) ==
25); assert(!isFullyFilled(order));

If that assert fires, the program itself tells you something is broken
--- you don't have to eyeball anything.

Your revision task (before we move to Chapter 2) Fix origianQuantity →
originalQuantity everywhere. Change id to uint64_t in the header.
Simplify isFullyFilled. Rewrite test_order.cpp to use `<cassert>`{=html}
with real assertions for your 3 cases (partial fill, exact fill,
rejected overfill) --- keep the std::cout if you want for readability,
but the assertions are what actually make it a test.

Paste the revised three files back and I'll approve or push back again.

order.hpp

20 lines

HPP

order.cpp

22 lines

CPP

test_order.cpp

23 lines

CPP

i have fixed the errors and typi

Validated fixes and identified incomplete test coverage gaps Validated
fixes and identified incomplete test coverage gaps

Good --- the header and implementation are clean now. originalQuantity,
uint64_t id, the simplified isFullyFilled --- all correct. That part is
approved.

But I'm bouncing the test file back. Look at what you actually covered:

cpp applyFill(order, 75); assert(order.filledQuantity == 75);
assert(remainingQuantity(order) == 25); assert(!isFullyFilled(order));

That's only 1 of the 3 required cases --- the partial fill. You dropped
the other two:

Exact fill to zero remaining --- you never call applyFill(order, 25)
again to bring it to exactly 100, and never assert isFullyFilled(order)
== true. Rejected overfill --- you never test that calling applyFill
with a quantity that would exceed originalQuantity actually gets
rejected and leaves filledQuantity unchanged.

This matters more than it looks. The overfill guard is the single most
important invariant in this whole file --- it's the line that prevents
Chimera from ever reporting more shares filled than were ordered. If you
don't have a test that actively tries to break it and confirms it holds,
you have no proof the guard works --- you only have proof the happy path
works. In production, the happy path is never what takes down an
exchange.

Task: finish the test file

Add to main():

After your existing partial-fill block, call applyFill(order, 25) to
bring it to exactly 100, and assert isFullyFilled(order) == true and
remainingQuantity(order) == 0. Then attempt an overfill --- call
applyFill(order, 10) (or any positive amount) and assert that
filledQuantity is still 100, i.e. the call had no effect.

Once all three cases have real assertions, compile and run it (g++
-std=c++20 test_order.cpp order.cpp -o test_order && ./test_order) --- a
clean run with no assertion failures is your proof of correctness, not
eyeballed console output.

Paste the final test file, and confirm it ran clean. Once that's in,
we'll do STEP 9 (docs) and STEP 10 (git commit) to close out Chapter 1,
then move to Chapter 2: std::map and why the order book needs a sorted
container.

move to the next chapter

Acknowledged incomplete work, pivoted toward next chapter Acknowledged
incomplete work, pivoted toward next chapter

Noted --- I'll hold you to finishing that test file before we write
actual Chimera book code, since an unverified overfill guard is exactly
the kind of thing that bites you in Phase 2. Consider it a standing item
on your list. For now, let's move forward.

Chapter 2: std::map and Why the Order Book Needs a Sorted Container STEP
1: Engineering Problem

Right now you have a single Order. But an exchange doesn't have one
order --- it has thousands of buy orders and thousands of sell orders
sitting at different prices, waiting to match.

Here's the actual problem: at any instant, the matching engine must
answer one question as fast as physically possible: "What is the current
best bid (highest buy price) and best ask (lowest sell price)?" Every
single incoming order needs this answer to decide whether it can match
immediately or must rest in the book.

Naive idea: store all buy orders in a std::vector`<Order>`{=html} and
scan it every time to find the max price. That's O(n) per lookup. With
thousands of resting orders and this check happening on every incoming
order, you've built an exchange that gets slower as it gets busier ---
the opposite of what you want.

So the real question: what data structure gives you fast "find the best
price" and fast "insert a new price level" at the same time?

STEP 2: Theory

This is where std::map enters --- but before syntax, understand why it
exists.

std::map\<Key, Value\> in C++ keeps its keys always sorted. Internally
it's a Red-Black Tree (a self-balancing binary search tree) --- we will
dig into how that balancing works in a later lesson, but for now, know
what it buys you:

Insert a new key: O(log n) Find the smallest or largest key: O(log n)
(or O(1) if you cache an iterator to begin()) Iterate all keys in sorted
order: O(n), already sorted, no extra sort step needed

Compare to alternatives:

std::vector --- O(1) access by index, but finding max/inserting in
sorted position is O(n) unless you keep it sorted and pay for shifting
elements. std::unordered_map --- O(1) average insert/lookup by key, but
no ordering at all. You cannot ask it "give me the smallest key" without
scanning everything. Great for "does this order ID exist," useless for
"what's the best price."

This is the trade-off: std::map sacrifices the O(1) of a hash map in
exchange for guaranteed ordering. For an order book, where "what's the
best price" is the single most frequent, most latency-critical query,
that trade is worth it.

Complexity summary for what we're about to build:

Operation std::vector (unsorted) std::unordered_map std::map Insert O(1)
O(1) avg O(log n) Find best price O(n) impossible (no order) O(1)
(begin/rbegin) Iterate sorted O(n log n) impossible O(n) STEP 3:
Experiment

Before touching Chimera, build intuition in experiments/ch02_map.cpp.
Predict the output first, then compile and run.

cpp #include `<iostream>`{=html} #include `<map>`{=html}

int main() { std::map\<int, std::string\> prices;

    prices[105] = "order_A";
    prices[101] = "order_B";
    prices[110] = "order_C";
    prices[103] = "order_D";

    std::cout << "Iterating the map:\n";
    for (const auto& [price, name] : prices) {
        std::cout << price << " -> " << name << "\n";
    }

    std::cout << "Smallest price (begin): " << prices.begin()->first << "\n";
    std::cout << "Largest price (rbegin): " << prices.rbegin()->first << "\n";

}

Write down, before compiling:

What order will the prices print in the loop --- insertion order, or
something else? What will begin() and rbegin() give you?

Then compile it (g++ -std=c++20 ch02_map.cpp -o ch02 && ./ch02), compare
to your prediction, and tell me --- in your own words --- why the map
behaves this way given what we just covered in STEP 2.
