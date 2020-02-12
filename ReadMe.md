Trying to create analog for C# Linq.

1. What works now:
   - Filtering
   - Converting to any container with constructor *(begin(), end())*

2. What will work (hopefully) in future:
   - Converting items
   - Ordering by labmda (asc, desc)
   - Reversing
   - Grouping
   - Creating range for consecutive integral values
   - Consuming part of elements

3. What to think about:
   - Is there really need in shared_ptr? Maybe there should be unique_ptr?
   - How to prolong temporary chain link without smart pointers?
   - Should Collection be copyable?
   - How to avoid extra copies?
   - Why am I doing this?..
