Tutorial 2: Complex Pointer Graphs
==================================

For programs that build large graphs of objects pointing to each other:
compilers and interpreters, parsers, database engines, and servers that
build state for each request.  Give each lifetime its own memory context,
allocate the graph from it, and release the whole graph in one call, cycles
and all.

* Slides: [PowerPoint](slides.pptx), [PDF](slides.pdf)

Releasing a context does not call destructors, so C++ RAII does not happen.
The objects in these examples hold only plain values, pointers into the same
context, and strings and lists whose memory comes from the same context, so
releasing everything at once is complete.

Examples
--------

| Example | What it shows |
|---------|---------------|
| [01_expression_ast.cpp](examples/01_expression_ast.cpp) | A recursive-descent parser builds an expression tree in a context; the tree, and the half-built tree of a failed parse, are released in one call |
| [02_cyclic_graph.cpp](examples/02_cyclic_graph.cpp) | A control-flow graph with a back edge, released in one call |
| [03_keep_results.cpp](examples/03_keep_results.cpp) | Work in a short-lived context and copy the results into a longer-lived one; `contains ()` tells them apart |
| [04_rewrite_in_place.cpp](examples/04_rewrite_in_place.cpp) | A tree that keeps changing frees replaced nodes one at a time, and its footprint stays flat |
| [05_per_request.cpp](examples/05_per_request.cpp) | One `CachedMemContext` per request, released after each request and reused by the next |

The helpers they share:

* [make.h](examples/make.h): `make<T> ()` and `destroy ()`, and the
  context-backed `String` and `List` types.
* [ast.h](examples/ast.h): the expression node, `print ()` and `evaluate ()`.
* [parser.h](examples/parser.h): the parser.

See [Building the Examples](../README.md#building-the-examples) to build and
run them.

Further Reading
---------------

* [Who CookMem Is For](https://github.com/coconut2015/cookmem#who-cookmem-is-for)
* [Detailed behavior](https://coconut2015.github.io/cookmem/behavior.html)
