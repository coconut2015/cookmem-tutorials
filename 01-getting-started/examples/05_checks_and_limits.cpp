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
// Checks and limits: padding catches writes past the end of an allocation,
// a footprint limit caps the memory of a context, and forEachAllocation
// reports what was not freed.
#include <cstring>
#include <iostream>

#include <cookmem.h>

int
main ()
{
    // Padding after each allocation is checked when the memory is freed.
    {
        cookmem::SimpleMemContext<> memCtx (cookmem::MAX_PADDING_SIZE);
        char* buffer = static_cast<char*> (memCtx.allocate (10));
        std::memset (buffer, 'x', 11);      // one byte too many
        try
        {
            memCtx.deallocate (buffer);
        }
        catch (const cookmem::Exception& ex)
        {
            // Expect output Caught: padding byte got modified.
            std::cout << "Caught: " << ex.getMessage () << "\n";
        }
        // After an exception, the context must not be used again.
    }

    // A footprint limit makes allocations fail instead of using more memory.
    {
        cookmem::SimpleMemContext<> memCtx;
        memCtx.setFootprintLimit (1024 * 1024);     // 1 MB
        void* huge = memCtx.allocate (10 * 1024 * 1024);
        // Expect output 10 MB request: failed
        std::cout << "10 MB request: " << (huge ? "succeeded" : "failed") << "\n";
    }

    // forEachAllocation lists the allocations that were not freed.
    {
        cookmem::SimpleMemContext<> memCtx (cookmem::MAX_PADDING_SIZE);
        void* used = memCtx.allocate (24);
        memCtx.allocate (100);                      // never freed
        memCtx.deallocate (used);

        // Expect output Not freed: 100 bytes
        memCtx.forEachAllocation ([] (void*, std::size_t size)
        {
            std::cout << "Not freed: " << size << " bytes\n";
        });
    }
    return 0;
}
