Tutorial 5: Limiting Memory Use
===============================

For programs that run plugins, scripts or untrusted input, and services with
many tenants.  A footprint limit caps the memory a context takes from the
system; allocations over it fail with `nullptr`, or with `std::bad_alloc`
from containers, and the rest of the program goes on.  Many C libraries
accept allocation functions, so a small adapter puts them on a budget too.

Limits also manage a workload as a whole.  When many worker threads run at
once, their memory adds up, and past the physical memory the system swaps
and slows to a crawl.  A budget for each worker thread, set on its
thread-local context, keeps the total in memory; a worker that reaches its
budget spills to disk, processes smaller batches, or waits, as database
engines do for each sort or hash join (PostgreSQL's `work_mem`).

* [Slides](slides.pptx)

The limit covers only memory allocated through the context.  Code that calls
`malloc` or `new` directly bypasses it, so this is not process isolation.

Examples
--------

| Example | What it shows |
|---------|---------------|
| [01_footprint_limit.cpp](examples/01_footprint_limit.cpp) | Allocations over the limit return `nullptr`; the context stays usable |
| [02_containers_bad_alloc.cpp](examples/02_containers_bad_alloc.cpp) | A container under a limit throws `std::bad_alloc` and stays usable |
| [03_untrusted_input.cpp](examples/03_untrusted_input.cpp) | A record that claims gigabytes is rejected instead of exhausting memory |
| [04_library_hooks.cpp](examples/04_library_hooks.cpp) | A Lua-style allocation hook gives each plugin of a stand-in library its own budget |

Example 04 uses a stand-in library with the same allocation hook as Lua's
`lua_Alloc`, so it builds without Lua; with Lua, pass the same adapter to
`lua_newstate`.

See [Building the Examples](../README.md#building-the-examples) to build and
run them.

Further Reading
---------------

* [Restricting memory usage](https://coconut2015.github.io/cookmem/tutorials.html#footprint)
* [Detailed behavior: releasing memory](https://coconut2015.github.io/cookmem/behavior.html)
