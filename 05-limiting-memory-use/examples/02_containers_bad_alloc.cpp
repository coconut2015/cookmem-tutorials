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
// Containers under a limit: cookmem::Allocator throws std::bad_alloc when
// the context cannot allocate, as the standard requires.  The container
// and the context stay usable.
#include <iostream>
#include <new>
#include <vector>

#include <cookmem.h>

typedef cookmem::CachedMemContext<> Context;

int
main ()
{
    Context memCtx;
    memCtx.setFootprintLimit (4 * 1024 * 1024);     // 4 MB

    cookmem::Allocator<int> allocator (memCtx);
    std::vector<int, cookmem::Allocator<int>> values (allocator);
    try
    {
        for (int i = 0; ; ++i)
        {
            values.push_back (i);
        }
    }
    catch (const std::bad_alloc&)
    {
        // push_back gives the strong guarantee: the vector is unchanged by
        // the push_back that failed.
        std::cout << "Limit reached with " << values.size () << " values\n";
    }

    // The vector is still valid, and smaller work still fits.
    values.resize (1000);
    values.shrink_to_fit ();
    // Expect output 1000 values kept
    std::cout << values.size () << " values kept\n";
    return values.size () == 1000 ? 0 : 1;
}
