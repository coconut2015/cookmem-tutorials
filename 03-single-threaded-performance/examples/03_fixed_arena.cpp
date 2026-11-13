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
// No system calls on the hot path: handle each request in a buffer that is
// allocated once, and fall back to a regular context only for a request too
// large for the buffer.
#include <cstring>
#include <iostream>

#include <cookmem.h>

typedef cookmem::MemContext<cookmem::FixedArena, cookmem::NoActionMemLogger> FixedContext;
typedef cookmem::CachedMemContext<> FallbackContext;

// Process a request that needs `size` bytes of scratch memory.
static void
handleRequest (FixedContext& fast, FallbackContext& fallback, std::size_t size)
{
    void* scratch = fast.allocate (size);
    const char* where = "buffer";
    if (scratch == nullptr)
    {
        // The buffer is full: use the fallback context for this request.
        scratch = fallback.allocate (size);
        where = "fallback";
    }
    std::memset (scratch, 0, size);     // ... the actual work ...
    std::cout << size << " bytes from the " << where << "\n";

    // The request is done.
    if (where[0] == 'b')
    {
        fast.releaseAll ();
    }
    else
    {
        fallback.deallocate (scratch);
    }
}

int
main ()
{
    // A 256 KB buffer, allocated once.  FixedArena only manages it; the
    // buffer must outlive the context.
    static char buffer[256 * 1024];
    cookmem::FixedArena arena (buffer, sizeof (buffer));
    cookmem::NoActionMemLogger logger;
    FixedContext fast (arena, logger);
    FallbackContext fallback;

    // Expect output 1000 bytes from the buffer
    handleRequest (fast, fallback, 1000);
    // Expect output 100000 bytes from the buffer
    handleRequest (fast, fallback, 100000);
    // Expect output 1000000 bytes from the fallback
    handleRequest (fast, fallback, 1000000);
    return 0;
}
