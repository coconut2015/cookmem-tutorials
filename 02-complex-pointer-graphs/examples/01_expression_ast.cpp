/*
 * Copyright (c) 2026 Heng Yuan
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// Parse an arithmetic expression into a tree whose nodes are allocated in a
// memory context, evaluate it, and release the whole tree in one call.
#include <iostream>
#include <stdexcept>

#include "parser.h"

static std::size_t
countAllocations (const Context& memCtx)
{
    return memCtx.forEachAllocation ([] (void*, std::size_t)
    {
    });
}

int
main ()
{
    Context memCtx;
    Variables vars;
    vars["x"] = 1;
    vars["y"] = 8;

    // Parse, evaluate, and drop the tree.
    const char* text = "max(1, 2 * (3 - x)) + -y / 4";
    Node* tree = Parser (memCtx, text).parse ();

    // Expect output (+ (max 1 (* 2 (- 3 x))) (/ (- y) 4))
    print (std::cout, tree);
    std::cout << "\n";
    // Expect output = 2
    std::cout << "= " << evaluate (tree, vars) << "\n";
    std::cout << countAllocations (memCtx) << " allocations in the context\n";

    // No tree walk, no delete: one call frees every node, name and list.
    memCtx.releaseAll ();
    // Expect output 0 allocations after releaseAll
    std::cout << countAllocations (memCtx) << " allocations after releaseAll\n";

    // A parse error leaves half-built nodes behind.  They are freed the
    // same way, so the error path needs no cleanup code.
    try
    {
        Parser (memCtx, "1 + (2 * ").parse ();
    }
    catch (const std::runtime_error& ex)
    {
        std::cout << "Error: " << ex.what () << "\n";
    }
    memCtx.releaseAll ();
    return countAllocations (memCtx) == 0 ? 0 : 1;
}
