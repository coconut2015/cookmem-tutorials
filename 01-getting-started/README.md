Tutorial 1: Getting Started
===========================

What a memory context is, how to allocate from one, how to free everything
at once, and how standard containers use one.

* [Slides](slides.pptx)

Freeing everything at once does not call destructors, so C++ RAII does not
happen.  It suits objects that own nothing but memory from the same context,
such as syntax tree nodes; objects that hold files, locks or memory from the
global heap must be destroyed first.

Examples
--------

| Example | What it shows |
|---------|---------------|
| [01_first_allocation.cpp](examples/01_first_allocation.cpp) | `allocate ()` and `deallocate ()` with a `SimpleMemContext` |
| [02_release_all.cpp](examples/02_release_all.cpp) | Freeing everything at once with `releaseAll ()` |
| [03_stl_allocator.cpp](examples/03_stl_allocator.cpp) | `std::vector` and `std::list` with `cookmem::Allocator` |
| [04_pmr.cpp](examples/04_pmr.cpp) | `std::pmr` containers with `cookmem::MemResource` (C++17) |

See [Building the Examples](../README.md#building-the-examples) to build and
run them.

Further Reading
---------------

* [CookMem tutorials](https://coconut2015.github.io/cookmem/tutorials.html)
* [Detailed behavior](https://coconut2015.github.io/cookmem/behavior.html)
