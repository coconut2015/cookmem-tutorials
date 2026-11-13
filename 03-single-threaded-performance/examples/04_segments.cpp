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
// The only step that may synchronize is the arena getting a segment from
// the system.  A larger minimum segment size makes it rarer.  StatsLogger
// counts the segments a context requests.
#include <cstdio>
#include <vector>

#include <cookmem.h>

typedef cookmem::MemContext<cookmem::MmapArena, cookmem::StatsLogger> Context;

// Allocate 100,000 blocks of 64 bytes, then free them, and return how many
// segments the context requested.
static std::size_t
countSegments (std::size_t minSegmentSize)
{
    cookmem::MmapArena arena (minSegmentSize);
    cookmem::StatsLogger logger;
    Context memCtx (arena, logger);

    std::vector<void*> blocks;
    for (int i = 0; i < 100000; ++i)
    {
        blocks.push_back (memCtx.allocate (64));
    }
    for (void* block : blocks)
    {
        memCtx.deallocate (block);
    }
    return logger.getStats ().getNumGetSegments ();
}

int
main ()
{
    std::printf ("minimum segment size   segments requested\n");
    for (std::size_t size : { std::size_t (64) * 1024, std::size_t (1024) * 1024,
                              std::size_t (16) * 1024 * 1024 })
    {
        std::printf ("%14zu KB   %18zu\n", size / 1024, countSegments (size));
    }
    return 0;
}
