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
// A control-flow graph with cycles, allocated in a memory context and
// released in one call.  No ownership rules are needed: blocks point to each
// other freely, including back edges.
#include <initializer_list>
#include <iostream>

#include "context.h"

// A basic block of a function.  Its edges point in both directions, so the
// graph is full of cycles.
struct Block
{
    String       name;
    List<Block*> successors;
    List<Block*> predecessors;

    Block (Context& memCtx, const char* blockName)
    : name (blockName, cookmem::Allocator<char> (memCtx)),
      successors (cookmem::Allocator<Block*> (memCtx)),
      predecessors (cookmem::Allocator<Block*> (memCtx))
    {
    }
};

static void
link (Block* from, Block* to)
{
    from->successors.push_back (to);
    to->predecessors.push_back (from);
}

int
main ()
{
    Context memCtx;

    // for (i = 0; i < n; ++i) { if (a[i]) sum += a[i]; } return sum;
    Block* entry = memCtx.create<Block> (memCtx, "entry");
    Block* loop = memCtx.create<Block> (memCtx, "loop");
    Block* test = memCtx.create<Block> (memCtx, "test");
    Block* add = memCtx.create<Block> (memCtx, "add");
    Block* next = memCtx.create<Block> (memCtx, "next");
    Block* exit = memCtx.create<Block> (memCtx, "exit");
    link (entry, loop);
    link (loop, test);
    link (loop, exit);
    link (test, add);
    link (test, next);
    link (add, next);
    link (next, loop);          // the back edge closes a cycle

    for (Block* b : { entry, loop, test, add, next, exit })
    {
        std::cout << b->name << " ->";
        for (Block* s : b->successors)
        {
            std::cout << " " << s->name;
        }
        std::cout << "\n";
    }

    // With std::shared_ptr for the edges, the cycle would keep every block
    // alive and leak.  With manual delete, each block would have to be
    // deleted exactly once, after nothing points to it.  Here, one call
    // frees the whole graph.
    memCtx.releaseAll ();
    // Expect output Footprint after releaseAll: 0
    std::cout << "Footprint after releaseAll: " << memCtx.getFootprint () << "\n";
    return memCtx.getFootprint () == 0 ? 0 : 1;
}
