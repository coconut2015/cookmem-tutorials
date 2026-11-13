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
// StatsLogger counts allocations, bytes and segments.  Comparing the
// numbers between the phases of a job shows memory that keeps growing.
#include <iostream>
#include <vector>

#include <cookmem.h>

typedef cookmem::SimpleMemContext<cookmem::MmapArena, cookmem::StatsLogger> Context;

static void
report (const char* phase, const Context& memCtx)
{
    const cookmem::MemStatistics& stats = memCtx.getLogger ().getStats ();
    std::cout << phase << ": " << stats.getCurrentAllocated () << " bytes in use, peak "
              << stats.getPeakAllocated () << ", " << stats.getCurrentSegments ()
              << " segments\n";
}

int
main ()
{
    Context memCtx;
    std::vector<void*> cache;    // a cache that is never trimmed: the bug

    for (int phase = 1; phase <= 3; ++phase)
    {
        std::vector<void*> scratch;
        for (int i = 0; i < 1000; ++i)
        {
            scratch.push_back (memCtx.allocate (100));
        }
        cache.push_back (memCtx.allocate (10000));
        for (void* p : scratch)
        {
            memCtx.deallocate (p);
        }
        // The bytes in use grow by 10,000 every phase.
        report (phase == 1 ? "Phase 1" : phase == 2 ? "Phase 2" : "Phase 3", memCtx);
    }

    const cookmem::MemStatistics& stats = memCtx.getLogger ().getStats ();
    std::cout << stats.getNumAllocations () << " allocations, "
              << stats.getNumDeallocations () << " deallocations\n";
    for (void* p : cache)
    {
        memCtx.deallocate (p);
    }
    return 0;
}
