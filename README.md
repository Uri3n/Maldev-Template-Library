# Maldev-Template-Library
## A WIP reimplementation of the C++ standard library with no CRT library dependencies.
## Compiled and tested with Visual Studio 2022.

If you dabble in Windows malware development then you might be aware that removing the CRT 
library from your projects can result in possibly better levels of evasion.
While it's often never the deciding factor of malware being detected by security solutions,
removing the CRT does have its small benefits, including:

  **1.** lower binary entropy.

  **2.** Reduced file bloat.

  **3.** Lower threat scores on sites like VirusTotal (although frankly, this is
          mainly due to the poor methodology for detection that it implements).

For these reasons, malware developers in the past have sometimes opted to **completely remove** the CRT library
from their projects. In doing this, however, you lose out on quite a bit of functionality in your programs, especially if
you're using C++. 

I created the **Maldev Template Library** as a solution to this. It provides a handful of useful template classes that don't require the
CRT to be used. These classes require only **Windows.h**, as well as **malutils.hpp**, a file included in this repo that aids in their
functionality. All of the classes as well as helper functions are accessible within the **mtl::** namespace.

Currently, there isn't **too** much here, but I'm planning on adding more
as time goes on. As of right now, the library includes:

  1. **String classes**, both ANSI and Unicode types.
  
  2. **Vectors (dynamic arrays)**.
  
  3. **Stacks**.
  
  4. **Queues**.
  
  5. **Linked Lists** (only singly for now, doubly linked lists are in the works).
  
  6. **Maps**. This is not a traditional hashmap, as its underlying data structure is a tree. However,
      This ends up being more flexible than a classic hashmap, because the type of the key can be
      whatever you desire (plus it's a lot easier to implement, no need to worry about hash collisions).
  
  7. **Smart Pointers**. A class that wraps around a raw pointer to heap memory, akin to **std::unique_ptr** or
      **std::shared_ptr**. Note though that this does not provide the same kind of borrowing and ownership semantics that
      these standard library classes do. It's merely a wrapper around heap memory that uses its destructor to prevent memory leaks.

# Important Considerations
Each class in this library has an **is_ok()** method. This can be used after each method call to check to see if a HeapAlloc call failed for
whatever reason. The chances of HeapAlloc failing is next to zero, but it is technically possible so it's there if you want to use it.
These template classes only work with **primitive types**. They do not work in conjunction with other classes.
Also, I made this for fun, mostly because I like data structures. If you want to use it, go ahead, but it's fairly incomplete right now.

# What I Plan On Adding
I'd like to add some more data structures, of course. Namely tuples, sets, and possibly graphs. I also want to add some general purpose functions
for malware development, outside of classes, such as an easy to use custom GetProcAddress and things of that nature.
