Tutorial 6: Replacing malloc and new
====================================

For anyone considering a different allocator for a whole program.  This
tutorial is not only about CookMem: it looks at the ways an allocator can
take over a program's memory, from replacing `malloc` and `free` to an
explicit allocator, and at what each one gains and risks.

* [Slides](slides.pptx)

There are no example programs.  The code on the slides is a sketch of each
approach, not a complete replacement, and it is not built.

Four Levels
-----------

| Level | Reaches | Notes |
|-------|---------|-------|
| `malloc` and `free` | The whole process, C libraries included | Global: every library uses it, and `LD_PRELOAD` is inherited by every child process |
| Global `operator new` and `delete` | All C++ allocations | Standard, but 20 functions in C++17, and many details to get right; the effort can be worth it |
| Class-specific `operator new` | One class hierarchy | Can release a whole object graph at once |
| Explicit allocators | What you pass them to | A context, `cookmem::Allocator` or `cookmem::MemResource`: the safer approach |

The broader the level, the less code changes, and the more can go wrong.

What Can Go Wrong
-----------------

* **A missing function.**  The glibc manual requires `malloc`, `free`,
  `calloc` and `realloc`, and recommends the rest of the family.  C++17 has
  8 replaceable forms of `operator new` and 12 of `operator delete`.  A
  missing one lets memory from one allocator be freed by the other.
* **Library boundaries.**  A replacement of `new` and `delete` in the
  program does not reach libraries that resolve them themselves: every DLL
  on Windows, and on Linux libraries with a static C++ runtime or linked
  with `-Bsymbolic`.  Memory must not cross such a boundary.
* **Mismatched pairs.**  When `new` is inlined from a header into your code
  and the matching `delete` is compiled into a third-party library that uses
  the default allocator, or the other way around, an object is freed by an
  allocator that did not allocate it.
* **Threads.**  A CookMem context is used by one thread at a time.  A global
  replacement needs a context for each thread, and memory freed by another
  thread has no cheap way to find its owner.  jemalloc, mimalloc and
  tcmalloc are built for this.
* **Startup and shutdown.**  Static objects allocate before `main`, memory
  outlives the thread that allocated it, and static destructors free memory
  in an order that is hard to control.
* **Recursion and tools.**  The allocator must not allocate through itself.
  AddressSanitizer replaces `malloc` too, and Valgrind needs
  `--soname-synonyms=somalloc=NONE`.

Releasing Everything at Once
----------------------------

A class-specific `operator new` backed by a context gives two ways to free
objects.  `delete node;` runs the destructor and returns the memory to the
context.  `releaseAll ()` on the context frees every object at once, without
`delete` and without running destructors, which is fine for objects that own
nothing but context memory.

A global replacement cannot do this safely: `releaseAll ()` on a global
context also frees memory owned by libraries and the standard library, and
their next use touches freed memory.

What to Use When
----------------

| Goal | Use |
|------|-----|
| Change the allocator of a whole program | jemalloc, mimalloc or tcmalloc, built to replace `malloc` thread-safely |
| Change all C++ allocations | A complete replacement of `operator new` and `delete`, if the effort is worth it |
| A job, a request or a thread | An explicit CookMem context, with `Allocator` or `MemResource` for containers |
| The objects of one graph | A class-specific `operator new` backed by a context, released at the end |
| A global CookMem replacement | Only with one thread, static linking, no memory crossing libraries, and no `releaseAll ()` |

Further Reading
---------------

* [Replacing malloc](https://www.gnu.org/software/libc/manual/html_node/Replacing-malloc.html) (glibc manual)
* [Replaceable allocation functions](https://en.cppreference.com/w/cpp/memory/new/operator_new) (cppreference)
* [Who CookMem Is For](https://github.com/coconut2015/cookmem#who-cookmem-is-for)
