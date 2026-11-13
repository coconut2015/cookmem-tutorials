Tutorial 4: Debugging Without Valgrind
======================================

For services and applications that Valgrind cannot run, or would slow down
too much.  CookMem's checks are part of the allocator: padding catches
writes past the end of an allocation, double frees throw, and
`forEachAllocation ()` lists leaks.  They run in normal builds, for each
context separately, so you can turn them on with a startup setting when a
problem needs to be reproduced.

* [Slides](slides.pptx)

CookMem complements Valgrind and AddressSanitizer; it does not replace them.
It does not see reads, so it cannot catch out-of-bounds reads or use after
free, and it checks padding only when memory is freed.

Padding costs time: in a program that does nothing but allocate and free,
8 bytes of padding made each allocation and free 38 to 50 percent slower,
and used 3.7 percent more memory.  In a real program the total is much
smaller, but it is a good reason to turn padding on only when you need it.

Examples
--------

| Example | What it shows |
|---------|---------------|
| [01_padding_check.cpp](examples/01_padding_check.cpp) | A write past the end is caught when the memory is freed, and the exception says where |
| [02_double_free.cpp](examples/02_double_free.cpp) | A double free throws |
| [03_leak_report.cpp](examples/03_leak_report.cpp) | A leak report for each request, with the exact sizes |
| [04_allocation_numbers.cpp](examples/04_allocation_numbers.cpp) | A logger that numbers allocations, so a leak report says which allocation leaked, and a debugger can stop at it |
| [05_statistics.cpp](examples/05_statistics.cpp) | `StatsLogger` shows memory that grows from phase to phase |
| [06_debug_config.cpp](examples/06_debug_config.cpp) | The checks turned on at startup with `--debug-memory` or `COOKMEM_DEBUG_MEMORY=1` |
| [07_padding_cost.cpp](examples/07_padding_cost.cpp) | What padding costs in time and memory |

[numbering_logger.h](examples/numbering_logger.h) is the numbering logger
used by examples 04 and 06.

See [Building the Examples](../README.md#building-the-examples) to build and
run them; build in Release mode to measure.

Further Reading
---------------

* [Memory corruption detection](https://coconut2015.github.io/cookmem/tutorials.html#boundcheck)
* [Finding memory leaks](https://coconut2015.github.io/cookmem/tutorials.html#leaks)
* [Limitations: best-effort error detection](https://coconut2015.github.io/cookmem/limitations.html)
