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
// Find where a leak was allocated.  A logger numbers every allocation; the
// leak report prints the numbers of the allocations that were not freed.
// Run again with that number as breakOn, and a breakpoint in
// onBreakAllocation () stops at the allocation that leaked.
#include <cstdlib>
#include <iostream>
#include <vector>

#include "numbering_logger.h"

typedef cookmem::MemContext<cookmem::MmapArena, NumberingLogger> Context;

// Somewhere in a large program, one of these allocations is not freed.
static void
work (Context& memCtx)
{
    std::vector<void*> blocks;
    for (int i = 0; i < 10; ++i)
    {
        blocks.push_back (memCtx.allocate (16 * (i + 1)));
    }
    for (std::size_t i = 0; i < blocks.size (); ++i)
    {
        if (i != 6)                     // the bug
        {
            memCtx.deallocate (blocks[i]);
        }
    }
}

int
main (int argc, const char* argv[])
{
    // The number to stop at, from the command line: 04_allocation_numbers 7
    unsigned long breakOn = argc > 1 ? std::strtoul (argv[1], nullptr, 10) : 0;

    cookmem::MmapArena arena;
    NumberingLogger logger (true, breakOn);
    Context memCtx (arena, logger, cookmem::MAX_PADDING_SIZE);

    work (memCtx);

    std::size_t leaks = memCtx.forEachAllocation ([&logger] (void* ptr, std::size_t size)
    {
        // Expect output Allocation #7 (112 bytes) was not freed
        std::cout << "Allocation #" << logger.numberOf (ptr) << " (" << size
                  << " bytes) was not freed\n";
    });
    if (leaks != 0 && breakOn == 0)
    {
        std::cout << "Run again with that number, and break in onBreakAllocation ()\n";
    }
    return leaks == 1 ? 0 : 1;
}
