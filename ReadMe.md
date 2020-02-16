Trying to create analog for C# Linq.
Now iterators fully satisfies [InputIterator](https://en.cppreference.com/w/cpp/named_req/InputIterator)

1. What works now:
   - Filtering
   - Converting to any container with constructor *(begin(), end())*
   - Reversing
   - Creating range for consecutive integral values

2. What will work (hopefully) in future:
   - Converting items
   - Ordering by labmda (asc, desc)
   - Grouping
   - Consuming part of elements

3. What to think about:
   - Is there really need in shared_ptr? Maybe there should be unique_ptr?
   - How to prolong temporary chain link without smart pointers?
   - Should Collection be copyable?
   - Leave it just input iterable?
   - rbegin/rend?
   - Optimizations of something like Reverse -> Reverse or Where -> Where or smth like that
   - How to avoid extra copies?
   - Why am I doing this?..
