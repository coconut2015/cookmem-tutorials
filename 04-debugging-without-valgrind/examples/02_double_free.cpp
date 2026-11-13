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
// Freeing the same allocation twice throws, in every build.
#include <iostream>

#include <cookmem.h>

int
main ()
{
    cookmem::SimpleMemContext<> memCtx;
    void* buffer = memCtx.allocate (100);
    memCtx.deallocate (buffer);

    try
    {
        memCtx.deallocate (buffer);     // the second free
    }
    catch (const cookmem::Exception& ex)
    {
        // Expect output Caught: potentially freeing an unused pointer
        std::cout << "Caught: " << ex.getMessage () << "\n";
        return ex.getError () == cookmem::MEM_ERROR_DOUBLE_FREE ? 0 : 1;
    }

    // The check is best effort: if the memory had been allocated again in
    // between, the second free would free that new allocation instead.
    std::cout << "The double free was not detected\n";
    return 1;
}
