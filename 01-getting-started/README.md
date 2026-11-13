Tutorial 1: Getting Started
===========================

An overview of CookMem: what a memory context is, who CookMem is for, what
a context can do, and how to start using one.  Later tutorials go deeper
into each kind of use.

* [Slides](slides.pptx)

Freeing everything at once does not call destructors, so C++ RAII does not
happen.  It suits objects that own nothing but memory from the same context,
such as syntax tree nodes; objects that hold files, locks or memory from the
global heap must be destroyed first.

Examples
--------

| Example | What it shows |
|---------|---------------|
| [01_first_allocation.cpp](examples/01_first_allocation.cpp) | `allocate ()`, `reallocate ()` and `deallocate ()`, like `malloc`, `realloc` and `free` |
| [02_release_all.cpp](examples/02_release_all.cpp) | Freeing everything at once with `releaseAll ()` |
| [03_stl_allocator.cpp](examples/03_stl_allocator.cpp) | `std::vector` and `std::list` with `cookmem::Allocator` |
| [04_pmr.cpp](examples/04_pmr.cpp) | `std::pmr` containers with `cookmem::MemResource` (C++17) |
| [05_checks_and_limits.cpp](examples/05_checks_and_limits.cpp) | Padding checks, a footprint limit and a leak report |

See [Building the Examples](../README.md#building-the-examples) to build and
run them.

Further Reading
---------------

* [CookMem tutorials](https://coconut2015.github.io/cookmem/tutorials.html)
* [Detailed behavior](https://coconut2015.github.io/cookmem/behavior.html)
