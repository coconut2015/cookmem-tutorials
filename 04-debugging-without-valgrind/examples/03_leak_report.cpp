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
// A leak report for each request: give each request its own context, and
// before releasing it, list the allocations the request forgot to free.
#include <cstring>
#include <iostream>

#include <cookmem.h>

typedef cookmem::SimpleMemContext<> Context;

// A request handler with a bug: on one path it forgets a free.
static void
handleRequest (Context& memCtx, int id)
{
    char* header = static_cast<char*> (memCtx.allocate (40));
    char* body = static_cast<char*> (memCtx.allocate (200 + id));
    std::memset (header, 0, 40);
    std::memset (body, 0, 200 + id);
    if (id % 3 != 0)
    {
        memCtx.deallocate (body);       // the bug: skipped when id % 3 == 0
    }
    memCtx.deallocate (header);
}

int
main ()
{
    // Padding stores the exact size of each allocation, so the report
    // shows the sizes that were requested.
    Context memCtx (cookmem::MAX_PADDING_SIZE);
    int leakingRequests = 0;
    for (int id = 1; id <= 6; ++id)
    {
        handleRequest (memCtx, id);

        std::size_t leaks = memCtx.forEachAllocation ([id] (void*, std::size_t size)
        {
            std::cout << "Request " << id << " leaked " << size << " bytes\n";
        });
        if (leaks != 0)
        {
            ++leakingRequests;
        }
        memCtx.releaseAll ();   // the next request starts clean
    }
    // Expect output Request 3 leaked 203 bytes, then request 6
    std::cout << leakingRequests << " of 6 requests leaked\n";
    return leakingRequests == 2 ? 0 : 1;
}
