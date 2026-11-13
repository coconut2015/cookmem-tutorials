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
// Standard containers allocating from a memory context.
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <cookmem.h>

typedef cookmem::CachedMemContext<> Context;

int
main ()
{
    // The application owns the context.  CachedMemContext keeps released
    // segments for reuse.
    Context memCtx;
    {
        cookmem::Allocator<int> allocator (memCtx);
        std::vector<int, cookmem::Allocator<int>> numbers (allocator);
        for (int i = 1; i <= 5; ++i)
        {
            numbers.push_back (i * i);
        }

        // An allocator converts to other element types, so containers of
        // different types can share the context.
        typedef std::basic_string<char, std::char_traits<char>, cookmem::Allocator<char>> String;
        std::list<String, cookmem::Allocator<String>> names (allocator);
        names.push_back (String ("a string long enough to need its own memory", allocator));

        // Expect output 5 numbers, 1 names
        std::cout << numbers.size () << " numbers, " << names.size () << " names" << std::endl;
        std::cout << "Footprint: " << memCtx.getFootprint () << " bytes" << std::endl;
    }
    // The containers are destroyed before the context.
    return 0;
}
