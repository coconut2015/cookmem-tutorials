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
// Turn the checks on with a startup setting.  A normal run has no padding
// and no leak report.  To reproduce a problem, run with --debug-memory (or
// set COOKMEM_DEBUG_MEMORY=1): the same code then pads every allocation,
// numbers every allocation, and reports leaks.
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "numbering_logger.h"

typedef cookmem::MemContext<cookmem::MmapArena, NumberingLogger> Context;

struct MemoryConfig
{
    bool debug = false;

    static MemoryConfig
    fromStartup (int argc, const char* argv[])
    {
        MemoryConfig config;
        const char* env = std::getenv ("COOKMEM_DEBUG_MEMORY");
        config.debug = env != nullptr && std::strcmp (env, "1") == 0;
        for (int i = 1; i < argc; ++i)
        {
            if (std::strcmp (argv[i], "--debug-memory") == 0)
            {
                config.debug = true;
            }
        }
        return config;
    }
};

// The same request handler as before, with its rare leak.
static void
handleRequest (Context& memCtx, int id)
{
    void* header = memCtx.allocate (40);
    void* body = memCtx.allocate (200);
    if (id % 3 != 0)
    {
        memCtx.deallocate (body);
    }
    memCtx.deallocate (header);
}

int
main (int argc, const char* argv[])
{
    MemoryConfig config = MemoryConfig::fromStartup (argc, argv);

    cookmem::MmapArena arena;
    NumberingLogger logger (config.debug);
    Context memCtx (arena, logger, config.debug ? cookmem::MAX_PADDING_SIZE : 0);
    std::cout << (config.debug ? "Debug run: padding, numbering and leak reports\n"
                               : "Normal run: no checks\n");

    for (int id = 1; id <= 3; ++id)
    {
        handleRequest (memCtx, id);
        if (config.debug)
        {
            memCtx.forEachAllocation ([id, &logger] (void* ptr, std::size_t size)
            {
                std::cout << "Request " << id << ": allocation #" << logger.numberOf (ptr)
                          << " (" << size << " bytes) was not freed\n";
            });
        }
        memCtx.releaseAll ();
    }
    return 0;
}
