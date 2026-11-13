Tutorial 3: Single-Threaded Performance
=======================================

For performance-critical code: thread-per-core servers, MPP executors,
batch jobs, simulations and game loops.  A shared heap must synchronize on
every `malloc` and `free`; a memory context belongs to one thread at a
time, so it never locks.  You also choose the scope of a context: a thread,
a block of code, or any lifetime you own.

* [Slides](slides.pptx)

On one machine (AMD Ryzen 7 5800H), a context per thread was 1.6 to 1.9
times faster than the Windows heap, and 1.1 to 1.4 times faster than glibc's
`malloc`, whose per-thread cache already avoids most locking for small
allocations.  The gain depends on your workload and your system allocator,
so run the benchmark, and better, your own workload.

Examples
--------

| Example | What it shows |
|---------|---------------|
| [01_thread_and_block_scope.cpp](examples/01_thread_and_block_scope.cpp) | A context per thread with `TLSContextProvider`, and a context per block with `TLSContextScope` |
| [02_threads_benchmark.cpp](examples/02_threads_benchmark.cpp) | The same work on 1, 2, 4 and 8 threads with the shared heap and with a context per thread; prints the times |
| [03_fixed_arena.cpp](examples/03_fixed_arena.cpp) | Requests served from a buffer allocated once, with a fallback context for requests that do not fit |
| [04_segments.cpp](examples/04_segments.cpp) | How the minimum segment size changes the number of segments a context requests, counted by `StatsLogger` |

The benchmark only reports times; CI checks that it runs, not how fast.
Build in Release mode to measure.  See
[Building the Examples](../README.md#building-the-examples).

Further Reading
---------------

* [Thread-local and scoped context STL allocators](https://coconut2015.github.io/cookmem/tutorials.html#tlsSTLAlloc)
* [Limitations: no thread safety](https://coconut2015.github.io/cookmem/limitations.html)
