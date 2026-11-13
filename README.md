CookMem Tutorials
=================

Tutorials for [CookMem](https://github.com/coconut2015/cookmem), a
header-only C++11 memory context and allocator based on dlmalloc.  Each
tutorial has slides, and tutorials 1 to 5 have example programs; some will
also have a video.

The examples are built and run by CI against the CookMem release named in
[CMakeLists.txt](CMakeLists.txt) (currently `v1.6.0`).

CookMem is not a drop-in replacement for `malloc`: code allocates from a
memory context explicitly, one thread at a time.  See
[Who CookMem Is For](https://github.com/coconut2015/cookmem#who-cookmem-is-for)
for the kinds of programs it helps most.

Tutorials
---------

Tutorial 1 is an overview, tutorials 2 to 5 follow those audiences, and
tutorial 6 looks at replacing `malloc` and `new` for a whole program.

| # | Tutorial | For | Status |
|---|----------|-----|--------|
| 1 | [Getting Started](01-getting-started/) ([slides](01-getting-started/slides.pptx), [PDF](01-getting-started/slides.pdf), [examples](01-getting-started/examples/)) | Everyone | Available |
| 2 | [Complex Pointer Graphs](02-complex-pointer-graphs/) ([slides](02-complex-pointer-graphs/slides.pptx), [PDF](02-complex-pointer-graphs/slides.pdf), [examples](02-complex-pointer-graphs/examples/)) | Compilers, parsers, database engines | Available |
| 3 | [Single-Threaded Performance](03-single-threaded-performance/) ([slides](03-single-threaded-performance/slides.pptx), [PDF](03-single-threaded-performance/slides.pdf), [examples](03-single-threaded-performance/examples/)) | Performance-critical code | Available |
| 4 | [Debugging Without Valgrind](04-debugging-without-valgrind/) ([slides](04-debugging-without-valgrind/slides.pptx), [PDF](04-debugging-without-valgrind/slides.pdf), [examples](04-debugging-without-valgrind/examples/)) | Services that Valgrind cannot run | Available |
| 5 | [Limiting Memory Use](05-limiting-memory-use/) ([slides](05-limiting-memory-use/slides.pptx), [PDF](05-limiting-memory-use/slides.pdf), [examples](05-limiting-memory-use/examples/)) | Sandboxing third-party code | Available |
| 6 | [Replacing malloc and new](06-replacing-malloc-and-new/) ([slides](06-replacing-malloc-and-new/slides.pptx), [PDF](06-replacing-malloc-and-new/slides.pdf)) | Anyone considering a global allocator | Available |

Each tutorial has its slides as PowerPoint and as a PDF export.
For reference documentation, see the
[CookMem documentation](https://coconut2015.github.io/cookmem/).

Building the Examples
---------------------

The examples need CMake 3.14 or later and a C++11 compiler (C++17 for the
`std::pmr` examples).  CMake downloads CookMem with `FetchContent`.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

To try another CookMem release, pass `-DCOOKMEM_TAG=<tag>` to the first
command.  To measure performance, as in tutorial 3, build in Release mode:
pass `-DCMAKE_BUILD_TYPE=Release` to the first command, or with Visual
Studio, `--config Release` to the build and `-C Release` to ctest.

License
-------

* The example code is licensed under the [Apache License, Version 2.0](LICENSE.txt).
* The slides and videos are licensed under
  [CC BY 4.0](LICENSE-slides.md).
