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
// std::pmr containers allocating from a memory context.  This needs C++17.
#include <iostream>
#include <map>
#include <memory_resource>
#include <string>
#include <vector>

#include <cookmem.h>

typedef cookmem::SimpleMemContext<> Context;

int
main ()
{
    Context memCtx;
    cookmem::MemResource<Context> resource (memCtx);
    {
        // The container type does not depend on the context type, and
        // the strings inside the map use the same resource.
        std::pmr::map<int, std::pmr::string> names (&resource);
        names[1] = "one";
        names[2] = "a string long enough to need its own memory";

        std::pmr::vector<int> numbers ({ 1, 4, 9 }, &resource);

        // Expect output 2 names, 3 numbers
        std::cout << names.size () << " names, " << numbers.size () << " numbers" << std::endl;

        // Count the allocations in the context.
        std::size_t count = memCtx.forEachAllocation ([] (void*, std::size_t)
        {
        });
        std::cout << count << " allocations in memCtx" << std::endl;
    }
    return 0;
}
