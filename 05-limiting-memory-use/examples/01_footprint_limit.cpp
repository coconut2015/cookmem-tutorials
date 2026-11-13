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
// A footprint limit caps the memory a context takes from the system.  An
// allocation that would go over it fails with nullptr, and the context
// stays usable.
#include <iostream>
#include <vector>

#include <cookmem.h>

int
main ()
{
    cookmem::SimpleMemContext<> memCtx;
    memCtx.setFootprintLimit (1024 * 1024);     // 1 MB

    // Allocate 100 KB blocks until the limit is reached.
    std::vector<void*> blocks;
    for (;;)
    {
        void* block = memCtx.allocate (100 * 1024);
        if (block == nullptr)
        {
            break;      // over the limit: handle it, the program goes on
        }
        blocks.push_back (block);
    }
    // The footprint includes the bookkeeping of each segment, so a little
    // less than the limit is available for allocations.
    std::cout << blocks.size () << " blocks of 100 KB fit, footprint "
              << memCtx.getFootprint () << " of " << memCtx.getFootprintLimit ()
              << " bytes\n";

    // Freeing memory makes room again.
    memCtx.deallocate (blocks.back ());
    blocks.pop_back ();
    void* again = memCtx.allocate (100 * 1024);
    // Expect output After a free: allocated
    std::cout << "After a free: " << (again ? "allocated" : "failed") << "\n";
    blocks.push_back (again);

    for (void* block : blocks)
    {
        memCtx.deallocate (block);
    }
    return again != nullptr ? 0 : 1;
}
